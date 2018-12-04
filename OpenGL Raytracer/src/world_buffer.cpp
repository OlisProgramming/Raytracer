#include "world_buffer.h"

#include <iostream>
#include <GL/glew.h>
#include "primitive.h"

// swb = Static World Buffer
// Contains all world data that will not change (so we can use GL_STATIC_DRAW).
GLuint swbSpheres;
constexpr GLuint swbSpheresBindingIndex = 1;

// smb = Static Material Buffer
GLuint smb;
constexpr GLuint smbBindingIndex = 2;

void initWorldBuffer(GLuint shader) {
	// Bind the UBO to the shader at a set binding point.
	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticWorldBufferSpheres"), swbSpheresBindingIndex);

	// Get size of CPU and GPU versions of this UBO.
	//GLint size;
	//glGetActiveUniformBlockiv(shader, swbSpheresBindingIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	//std::cout << "GPU: " << size << "B\nCPU: " << sizeof(Sphere) << "B\n\n";

	// Create the UBO
	Sphere spheres[3];
	spheres[0] = Sphere(0, vec3(), 1.f);
	spheres[1] = Sphere(0, vec3(-0.7f, 0.f, 0.7f), 0.5f);
	spheres[2] = Sphere(0, vec3(0.7f, 0.0f, 0.7f), 0.5f);

	glGenBuffers(1, &swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, swbSpheres);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * 3, spheres, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, swbSpheresBindingIndex, swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	////

	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticMaterialBuffer"), smbBindingIndex);

	glGenBuffers(1, &smb);
	glBindBuffer(GL_UNIFORM_BUFFER, smb);
	Material material(vec3(0, 1, 1), vec3(1, 1, 1), 32.f);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), &material, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, smbBindingIndex, smb);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
