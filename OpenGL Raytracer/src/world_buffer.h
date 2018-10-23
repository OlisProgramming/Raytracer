#pragma once

#include <GL/glew.h>

// Initialises buffers on the GPU to store the world data.
// For now, this will be a UBO. If worlds end up getting too much
// bigger, we may need to change this to an SSBO.
void initWorldBuffer(GLuint shader);
