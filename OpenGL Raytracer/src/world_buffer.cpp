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
	//{ GLint size = 0;
	//glGetActiveUniformBlockiv(shader, swbSpheresBindingIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	//std::cout << "(SWBS)\nGPU: " << size << "B\nCPU: " << sizeof(Sphere) * 3 << "B\n\n"; }

	// Create the UBO
	Sphere spheres[10];
	spheres[0] = Sphere(0, vec3(), 1.f);
	spheres[1] = Sphere(1, vec3(-0.7f, 0.f, 0.7f), 0.5f);
	spheres[2] = Sphere(1, vec3(0.7f, 0.0f, 0.7f), 0.5f);

	spheres[3] = Sphere(0, vec3(1.7f, 2.0f, 0.7f), 0.5f);
	spheres[4] = Sphere(1, vec3(0.7f, 2.0f, 0.7f), 0.5f);
	spheres[5] = Sphere(0, vec3(0.7f, -2.0f, 1.7f), 0.5f);
	spheres[6] = Sphere(1, vec3(1.7f, -2.0f, 1.7f), 0.5f);
	spheres[7] = Sphere(0, vec3(1.7f, 2.0f, 0.7f), 0.5f);
	spheres[8] = Sphere(1, vec3(0.7f, 2.0f, 0.7f), 0.5f);
	spheres[9] = Sphere(0, vec3(0.7f, 3.0f, 1.7f), 0.5f);

	glGenBuffers(1, &swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, swbSpheres);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * 10, spheres, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, swbSpheresBindingIndex, swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	////

	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticMaterialBuffer"), smbBindingIndex);

	//{ GLint size = 0;
	//glGetActiveUniformBlockiv(shader, smbBindingIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	//std::cout << "(SMB)\nGPU: " << size << "B\nCPU: " << sizeof(Material) * 2 << "B\n\n"; }

	glGenBuffers(1, &smb);
	glBindBuffer(GL_UNIFORM_BUFFER, smb);
	Material materials[2];
	materials[0] = Material(vec3(1, 0, 0), vec3(.8f, .3f, .3f), 32.f);
	materials[1] = Material(vec3(0, 0, 0), vec3(.4f, .4f, .4f), 32.f);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material) * 2, materials, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, smbBindingIndex, smb);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
