#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "material.h"

// This file lists all primitives with support in the shader.
// Conveniently, the C++ standard requires structs to maintain their
// order of fields through inheritance, so we know that Primitive's
// fields will always be at the start of all of Primitive's 'children'.

struct alignas(16) Primitive {
	// Material ID
	alignas(16) uint m;
	// Origin
	alignas(16) vec4 o;

	inline Primitive(uint m, vec3 o) :
		m(m), o(o, 0) {}
};

struct alignas(16) Sphere : Primitive {
	// Radius squared
	alignas(16) float r2;

	inline Sphere() : Sphere(0, vec3(), 0) {}

	inline Sphere(uint m, vec3 o, float r) :
		Primitive(m, vec4(o, 0)), r2(r*r) {}
};
