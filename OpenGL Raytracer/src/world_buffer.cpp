#include "world_buffer.h"

#include <iostream>
#include <GL/glew.h>
#include "primitive.h"

// swb = Static World Buffer
// Contains all world data that will not change (so we can use GL_STATIC_DRAW).

GLuint swbSpheres;
GLuint swbSpheresBindingPoint;

void initWorldBuffer(GLuint shader) {
	// Bind the UBO to the shader at a set binding point.
	swbSpheresBindingPoint = glGetUniformBlockIndex(shader, "StaticWorldDataSpheres");
	glUniformBlockBinding(shader, swbSpheresBindingPoint, swbSpheresBindingPoint);
	
	GLint s;
	glGetActiveUniformBlockiv(shader, swbSpheresBindingPoint,
		GL_UNIFORM_BLOCK_DATA_SIZE, &s);
	std::cout << s << " bytes in buffer; " << sizeof(Sphere) << " bytes in Sphere" << std::endl;

	// Create the UBO
	glGenBuffers(1, &swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, swbSpheres);
	Material material(vec3(1,0,1), vec3(1,1,1), 32.f);
	Sphere sphere(material, vec3(), 1.f);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Sphere), &sphere, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, swbSpheresBindingPoint, swbSpheres);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
