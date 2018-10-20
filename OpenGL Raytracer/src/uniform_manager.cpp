#include "uniform_manager.h"

UniformData uniformData;
GLint aspectLoc;

void init_uniforms(GLuint prog) {
	aspectLoc = glGetUniformLocation(prog, "aspect");
}

void update_uniforms() {
	glUniform1f(aspectLoc, 1024.f / 768.f);
}