#version 330 core

#define EPSILON 1e-7
#define LARGE_EPSILON 1e-3

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

struct PointLight {
	// Origin
	vec3 o;
	// x^2 light attenuation coefficient (effective brightness)
	float atten;
};

////////////////////////////////

layout (std140) uniform StaticMaterialBuffer {
	Material materials[2];
} smb;

layout (std140) uniform StaticWorldBufferSpheres {
	Sphere spheres[10];
} swdSpheres;

////////////////////////////////

// Returns the value for t such that the position of intersection
// can be represented by (r.o + t*r.d).
// If there are no intersections, return -1.f.
// Pass by value because the ray and sphere are very small objects.
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

// Calculates the direction of the normal to the sphere at this particular
// point on the sphere. In this case it's as simple as finding the normalised
// vector from the centre of the sphere to the intersection.
vec3 normalToSphere(vec3 point, Sphere sphere) {
	return normalize(point - sphere.o.xyz);
}

// Calculates the resultant colour that the ray detects.
// (0,0,0,1) if no intersections.
// Also takes in a parameter to specify exactly how many more
// reflected rays we are allowed to generate.
vec4 calculateRay(Ray r) {
	vec4 ret = vec4(0, 0, 0, 1);  // return value

	PointLight l;
	l.o = vec3(5.f, -1.f, 0.5f);
	l.atten = 10;

	int reflections = 5;
	vec3 reflectionMultiplier = vec3(1, 1, 1);

	while (reflections > 0) {
		--reflections;

		// The closest value for depth we found to the start of the ray.
		float hitDepth = 1e38;  // reasonably large float value approximating infinity

		vec3 result = vec3(0, 0, 0);
		Material m;
		vec3 intersectionPoint;
		vec3 reflectDir;
		vec3 normal;

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
		}

		if (dot(result, result) > EPSILON) {
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