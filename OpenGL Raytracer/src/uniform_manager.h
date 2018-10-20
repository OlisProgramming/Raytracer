#pragma once

#include <GL/glew.h>

struct UniformData {
	float aspect;
};
extern UniformData uniformData;

void init_uniforms(GLuint shaderProgram);
void update_uniforms();