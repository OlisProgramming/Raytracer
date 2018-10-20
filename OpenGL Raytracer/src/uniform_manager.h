#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct UniformData {
	float aspect;
	// Projects a camera centred at the origin pointing in the +x direction to
	// any arbitrary position and rotation.
	glm::mat4 camView;
};
extern UniformData uniformData;

void init_uniforms(GLuint shaderProgram);
void update_uniforms();