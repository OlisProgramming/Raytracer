#pragma once

#include <GL/glew.h>

// See shader.frag for more info on these defines.
#ifndef UBO_SIZE
# define UBO_SIZE 1024
#endif

#ifndef SIZE_SMB
# define SIZE_SMB UBO_SIZE
#endif

#ifndef SIZE_SWB_SPHERES
# define SIZE_SWB_SPHERES UBO_SIZE
#endif

#ifndef SIZE_SWB_TRIS
# define SIZE_SWB_TRIS UBO_SIZE
#endif

// Initialises buffers on the GPU to store the world data.
// For now, this will be a UBO. If worlds end up getting too much
// bigger, we may need to change this to an SSBO.
void initWorldBuffer(GLuint shader);
