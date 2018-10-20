#version 330 core

uniform float aspect;

in vec2 pos;
out vec4 colour;

struct Ray {
	// Origin
	vec3 o;
	// Normalised direction vector
	vec3 d;
};

struct Sphere {
	// Origin
	vec3 o;
	// Radius squared
	float r2;
};

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
	vec3 v = r.o - s.o;
	float dot_v_d = dot(v, r.d);

	// First, calculate the discriminant so we know whether real
	// solutions exist.
	float discriminant = dot_v_d * dot_v_d - dot(v, v) + s.r2;

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

void main() {
	// Calculate the ray coming from the camera through this pixel.
	// We look in the +x direction.
	// +y is right,
	// +z is up.
	Ray r;
	r.o = vec3(0.f, 0.f, 0.f);
	r.d = normalize(vec3(1.f, pos.x * aspect, pos.y));

	Sphere s;
	s.o = vec3(5.f, 0.f, 0.f);
	s.r2 = 1.f;

	// Set the output colour to RGBA: R(x pos) G(y pos) B(0) A(1).
    //colour = vec4(pos/2.f+0.5f, 0.f, 1.f);
	float t = nearestIntersectionRaySphere(r, s);
	if (t < 0) colour = vec4(0.f, 0.f, 0.f, 1.f);
	else colour = vec4(1.f, 0.f, 0.f, 1.f);
}