#pragma once

#include <glm/glm.hpp>
using namespace glm;

// Things that should be vec3s are actually vec4s because GLSL needs alignment on orders of 4 bytes.
// We're using alignas(16) on each variable for the same reason.

struct alignas(16) Material {
	// Diffuse colour
	alignas(16) vec4 diffuse;
	// Specular colour
	alignas(16) vec4 specular;
	// Specular shininess
	alignas(16) float shininess;

	inline Material(vec3 diffuse, vec3 specular, float shininess) :
		diffuse(diffuse, 0), specular(specular, 0), shininess(shininess) {}
};