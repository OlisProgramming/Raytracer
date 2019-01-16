#pragma once

#include <GL/glew.h>
#include <vector>

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

template<class T> class WorldBuffer {
private:
	const char *bufName;
	GLuint ubo, shader;
	int uboBindingIndex;
	std::vector<T> contents;
	bool trackSize;
public:
	// If trackSize is enabled, another uniform is set to track the size of the buffer (since it can change).
	WorldBuffer<T>(GLuint shader, const char *bufName, bool trackSize = true);
	inline std::vector<T>& getContents() { return contents; }
	const void flushEntireContents();
};

// Initialises buffers on the GPU to store the world data.
// For now, this will be a UBO. If worlds end up getting too much
// bigger, we may need to change this to an SSBO.
void initWorldBuffer(GLuint shader);