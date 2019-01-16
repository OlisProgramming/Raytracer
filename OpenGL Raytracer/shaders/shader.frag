#version 330 core

#define EPSILON 1e-7
#define LARGE_EPSILON 1e-4

#define MAX_REFLECTIONS 5

#define CULLING

uniform float aspect;
uniform mat4 camView;

in vec2 pos;
out vec4 colour;

// We can't use vec3's for UBOs/SSBOs because vec3 isn't on the right alignment.

// If there is no diffuse component, or no specular component (for example),
// simply set their values to (0, 0, 0).
struct Material {
	// Diffuse colour
	vec4 diffuse;
	// Specular colour, w component is shininess
	vec4 specular;
};

struct Ray {
	// Origin
	vec3 o;
	// Normalised direction vector
	vec3 d;
};

struct Sphere {
	// Material ID
	uint m;
	// Origin
	vec4 o;
	// Radius squared
	float r2;
};

struct Triangle {
	// Material ID
	uint m;
	// Position vectors of vertices (v0 stored as o inherited from Primitive in C++ side)
	vec4 v0, v1, v2;
};

struct PointLight {
	// Origin
	vec3 o;
	// x^2 light attenuation coefficient (effective brightness)
	float atten;
};

////////////////////////////////

layout (std140) uniform StaticMaterialBuffer {
	Material materials[3];
} smb;

layout (std140) uniform StaticWorldBufferSpheres {
	Sphere spheres[10];
} swdSpheres;

layout (std140) uniform StaticWorldBufferTris {
	Triangle tris[12];
} swdTris;

////////////////////////////////

// Returns the value for t such that the position of intersection
// can be represented by (r.o + t*r.d).
// If there are no intersections, return -1.f.
float nearestIntersectionRaySphere(Ray r, Sphere s) {
	// The solutions to the equation representing the intersection
	// between these two objects can be written as:
	// -(v . d) +- sqrt[(v . d)^2 - (v^2 - r^2)]
	// where
	//	o = Ray origin
	//	d = Ray direction
	//	c = Sphere origin
	//	r = Sphere radius
	//	v = (o - c);

	// Precalculate some useful data.
	vec3 v = r.o.xyz - s.o.xyz;
	float dot_v_d = dot(v.xyz, r.d.xyz);

	// First, calculate the discriminant so we know whether real
	// solutions exist.
	float discriminant = dot_v_d * dot_v_d - dot(v.xyz, v.xyz) + s.r2;

	// If the discriminant is <0, there are no solutions.
	if (discriminant < 0.f) {
		return -1.f;
	}

	// Calculate the two solutions.
	float sqrt_discriminant = sqrt(discriminant);
	float t1 = sqrt_discriminant - dot_v_d;
	float t2 = -sqrt_discriminant - dot_v_d;

	// We only care about the smallest positive solution,
	// as we don't care about rays behind the camera,
	// and we only care about the first thing the ray hits.
	if (t1 <= 0.f && t2 <= 0.f) return -1.f;
	if (t1 > t2) {
		if (t2 <= 0.0f) return t1;
		return t2;
	}
	if (t1 <= 0.0f) return t2;
	return t1;
}

// Stores the value for t in the pointer supplied such that the position of intersection
// can be represented by (r.o + t*r.d).
// Also stores the barycentric coordinate of the intersection in the pointers supplied
// that are named u and v. 0<=u<=1; 0<=v<=1; u+v<=1. The triangle's vertices map to UV as following:
// v0: u=v=0; v1:u=1,v=0; v2:u=0,v=1.
// Return true only if there was an intersection.
// Uses the Moeller-Trombore ray-triangle intersection algorithm.
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool nearestIntersectionRayTri(Ray r, Triangle tri, out float t, out float u, out float v) {
	// Calculate vectors v0->v1 and v0->v2.
	vec3 v0v1 = tri.v1.xyz - tri.v0.xyz;
	vec3 v0v2 = tri.v2.xyz - tri.v0.xyz;

	vec3 pvec = cross(r.d, v0v2);
	float det = dot(v0v1, pvec);

#ifdef CULLING
	// If determinant is small or negative, the triangle is backfacing and so we don't render it.
	if (det < EPSILON) return false;
#else
	// If the determinant is small, the ray is very close to being parallel with the triangle and so we don't render it.
	if (abs(det) < EPSILON) return false;
#endif

	// Only perform this division once.
	// Where we would use something/det, just use something*invDet instead -- it's faster.
	float invDet = 1/det;

	vec3 tvec = r.o - tri.v0.xyz;
	u = dot(tvec, pvec) * invDet;
	// If the barycentric coordinate was invalid, return false.
	// This means that when the ray passes through the triangle's plane, it doesn't intersect with the triangle itself.
	if (u < 0 || u > 1) return false;

	vec3 qvec = cross(tvec, v0v1);
	v = dot(r.d, qvec) * invDet;
	// If the combination of barycentric coordinates were invalid, return false.
	if (v < 0 || u+v > 1) return false;

	t = dot(v0v2, qvec) * invDet;
	return true;
}

// Calculates the direction of the normal to the sphere at this particular
// point on the sphere. In this case it's as simple as finding the normalised
// vector from the centre of the sphere to the intersection.
vec3 normalToSphere(vec3 point, Sphere sphere) {
	return normalize(point - sphere.o.xyz);
}

// Calculates the direction of the normal to the triangle. It's the same
// at any point on the triangle. Calculated by finding the cross product
// of two of the vectors along the sides of the triangle.
vec3 normalToTri(Triangle tri) {
	return normalize(cross(tri.v1.xyz-tri.v0.xyz, tri.v2.xyz-tri.v0.xyz));
}

// Does this particular ray hit anything before a defined t?
// If so, return key values for the result of this ray.
// Does not compute reflections etc.
void castRay(Ray r, float tMax, out bool hit, out Material m, out vec3 intersectionPoint, out vec3 reflectDir, out vec3 normal) {
	// The closest value for depth we found to the start of the ray.
	float hitDepth = tMax;  // reasonably large float value approximating infinity to detect rays from almost infinitely far away
	hit = false;

	for (int i = 0; i < 10; i++) {
		Sphere s = swdSpheres.spheres[i];

		float t = nearestIntersectionRaySphere(r, s);
		// t>epsilon not t>0: why?
		// If we reflect a ray, it might intersect the surface it just reflected off of.
		// Due to floating point rounding errors it might end up giving us a value above zero
		// for t.
		if (t > LARGE_EPSILON && t < hitDepth) {
			hitDepth = t;
			m = smb.materials[s.m];
			intersectionPoint = r.o + t*r.d;
			normal = normalToSphere(intersectionPoint, s);
			hit = true;
		}
	}

	for (int i = 0; i < 12; i++) {
		Triangle tri = swdTris.tris[i];

		float t, u, v;
		bool didHit = nearestIntersectionRayTri(r, tri, t, u, v);
		if (didHit && t > LARGE_EPSILON && t < hitDepth) {
			hitDepth = t;
			m = smb.materials[tri.m];
			intersectionPoint = r.o + t*r.d;
			normal = normalToTri(tri);
			hit = true;
		}
	}
}

// Perform the Phong shading algorithm on this fragment.
// Also calculates shadow ray to determine whether any light should come from the source.
void light(Ray r, Material m, vec3 intersectionPoint, vec3 normal, out vec3 reflectDir, out vec3 result) {
	PointLight l;
	l.o = vec3(4.f, -1.f, 0.5f);
	l.atten = 10;
	
	// Does the light source have direct line of sight to the intersection point?
	Ray shadowRay;
	shadowRay.o = intersectionPoint;
	shadowRay.d = normalize(l.o - intersectionPoint);
	shadowRay.o += 0.05*shadowRay.d;
	float distanceBetween = distance(l.o, intersectionPoint)-0.05;
	// Advance the ray by a small amount to remove artifacts with the ray colliding with the object it just came off of.
	bool hit;
	Material mUNUSED;
	vec3 vecUNUSED1, vecUNUSED2, vecUNUSED3;
	castRay(shadowRay, distanceBetween, hit, mUNUSED, vecUNUSED1, vecUNUSED2, vecUNUSED3);
	if (hit) return;

	// We'll model light intensity with the Phong reflection model.
	vec3 ambientIntensity = vec3(1,1,1) * 0.05f;

	// The intensity of the light from diffuse reflection at a certain point is affected
	// by the angle the light makes with the material's normal.
	vec3 pointToLight = l.o - intersectionPoint;
	float diffuseIntensityMultiplier = max(dot(normalize(pointToLight), normal), 0.f);
	vec3 diffuseIntensity = m.diffuse.xyz * diffuseIntensityMultiplier;

	reflectDir = reflect(-normalize(pointToLight), normal);
	vec3 specularIntensity = m.specular.xyz * pow(max(-dot(r.d, reflectDir), 0.f), m.specular.w);

	// Intensity is calculated with the attenuation equation 1/d^2 (inverse-square law).
	float distanceFromLight = length(pointToLight);
	float intensityMultiplier = l.atten / (distanceFromLight * distanceFromLight);

	result = ambientIntensity + diffuseIntensity + specularIntensity;
	result *= intensityMultiplier;
}

// Calculates the resultant colour that the ray detects.
// (0,0,0,1) if no intersections.
// Also takes in a parameter to specify exactly how many more
// reflected rays we are allowed to generate.
vec4 calculateRay(Ray r) {
	vec4 ret = vec4(0, 0, 0, 1);  // return value

	int reflections = MAX_REFLECTIONS;
	vec3 reflectionMultiplier = vec3(1, 1, 1);

	while (reflections > 0) {
		--reflections;

		vec3 result = vec3(0, 0, 0);
		vec3 intersectionPoint;
		vec3 reflectDir;
		vec3 normal;
		Material m;
		bool hit;

		castRay(r, 500, hit, m, intersectionPoint, reflectDir, normal);

		if (hit) {
			light(r, m, intersectionPoint, normal, reflectDir, result);
			ret += vec4(result * reflectionMultiplier, 0);
			
			// Calculate a reflected ray if there is any specular reflection.
			if (dot(reflectionMultiplier*m.specular.xyz, reflectionMultiplier*m.specular.xyz) > EPSILON) {
				r.d = normalize(reflect(normalize(intersectionPoint-r.o), normal));
				r.o = intersectionPoint;
				reflectionMultiplier *= m.specular.xyz * 0.8f;
			} else {
				reflections = 0;
			}
		} else {
			reflections = 0;
		}
	}

	return ret;
}

void main() {
	// Calculate the ray coming from the camera through this pixel.
	// We look in the +x direction.
	// +y is right,
	// +z is up.
	Ray r;
	r.o = (camView * vec4(0.f, 0.f, 0.f, 1.f)).xyz;
	r.d = normalize((camView * vec4(1.f, pos.x * aspect, pos.y, 1.f)).xyz - r.o.xyz);

	colour = calculateRay(r);
}