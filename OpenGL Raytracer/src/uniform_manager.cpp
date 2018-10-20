#include "uniform_manager.h"

#include <glm/gtc/type_ptr.hpp>

UniformData uniformData;
GLint aspectLoc, camViewLoc;

void init_uniforms(GLuint prog) {
	aspectLoc = glGetUniformLocation(prog, "aspect");
	camViewLoc = glGetUniformLocation(prog, "camView");
}

void update_uniforms() {
	glUniform1f(aspectLoc, uniformData.aspect);
	glUniformMatrix4fv(camViewLoc, 1, GL_FALSE, glm::value_ptr(uniformData.camView));
}