#include "world_buffer.h"

#include <iostream>
#include <GL/glew.h>
#include "primitive.h"

// swb = Static World Buffer
// Contains all world data that will not change (so we can use GL_STATIC_DRAW).
GLuint swbSpheres;
constexpr GLuint swbSpheresBindingIndex = 1;

GLuint swbTris;
constexpr GLuint swbTrisBindingIndex = 2;

// smb = Static Material Buffer
GLuint smb;
constexpr GLuint smbBindingIndex = 3;

void initWorldBuffer(GLuint shader) {
	// Bind the UBO to the shader at a set binding point.
	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticWorldBufferSpheres"), swbSpheresBindingIndex);

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
	
	// First, generate the buffers with a large size.
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere) * SIZE_SWB_SPHERES, NULL, GL_STATIC_DRAW);
	// Then, fill the start of the buffer with our data.
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Sphere) * 10, spheres);

	glBindBufferBase(GL_UNIFORM_BUFFER, swbSpheresBindingIndex, swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Tell the shader how many spheres we've loaded into its buffer.
	glUniform1i(glGetUniformLocation(shader, "currentSizeSwbSpheres"), 10);

	////

	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticWorldBufferTris"), swbTrisBindingIndex);

	glGenBuffers(1, &swbTris);
	glBindBuffer(GL_UNIFORM_BUFFER, swbTris);
	Triangle tris[12];
	tris[0] = Triangle(2, vec3(-6, -6, -6), vec3(6, -6, -6), vec3(-6, 6, -6));
	tris[1] = Triangle(2, vec3(-6, 6, -6), vec3(6, -6, -6), vec3(6, 6, -6));

	tris[2] = Triangle(2, vec3(-6, -6, -6), vec3(-6, -6, 6), vec3(6, -6, -6));
	tris[3] = Triangle(2, vec3(-6, -6, 6), vec3(6, -6, 6), vec3(6, -6, -6));

	tris[4] = Triangle(2, vec3(-6, 6, -6), vec3(6, 6, -6), vec3(-6, 6, 6));
	tris[5] = Triangle(2, vec3(-6, 6, 6), vec3(6, 6, -6), vec3(6, 6, 6));

	tris[6] = Triangle(2, vec3(6, 6, -6), vec3(6, -6, -6), vec3(6, 6, 6));
	tris[7] = Triangle(2, vec3(6, 6, 6), vec3(6, -6, -6), vec3(6, -6, 6));

	tris[8] = Triangle(2, vec3(-6, 6, -6), vec3(-6, 6, 6), vec3(-6, -6, -6));
	tris[9] = Triangle(2, vec3(-6, 6, 6), vec3(-6, -6, 6), vec3(-6, -6, -6));

	tris[10] = Triangle(2, vec3(-6, -6, 6), vec3(-6, 6, 6), vec3(6, -6, 6));
	tris[11] = Triangle(2, vec3(-6, 6, 6), vec3(6, 6, 6), vec3(6, -6, 6));
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Triangle) * SIZE_SWB_TRIS, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Triangle) * 12, tris);
	glBindBufferBase(GL_UNIFORM_BUFFER, swbTrisBindingIndex, swbTris);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glUniform1i(glGetUniformLocation(shader, "currentSizeSwbTris"), 12);

	////

	glUniformBlockBinding(shader, glGetUniformBlockIndex(shader, "StaticMaterialBuffer"), smbBindingIndex);

	//{ GLint size = 0;
	//glGetActiveUniformBlockiv(shader, smbBindingIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	//std::cout << "(SMB)\nGPU: " << size << "B\nCPU: " << sizeof(Material) * 2 << "B\n\n"; }

	glGenBuffers(1, &smb);
	glBindBuffer(GL_UNIFORM_BUFFER, smb);
	Material materials[3];
	materials[0] = Material(vec3(1, 0, 0), vec3(.6f, .3f, .3f), 32.f);
	materials[1] = Material(vec3(0, 0, 0), vec3(.4f, .4f, .4f), 24.f);
	materials[2] = Material(vec3(1, 1, 1), vec3(.6f, .6f, .6f), 64.f);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Material) * SIZE_SMB, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material) * 3, materials);
	glBindBufferBase(GL_UNIFORM_BUFFER, smbBindingIndex, smb);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
