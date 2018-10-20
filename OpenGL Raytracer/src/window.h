#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>

class Window {
private:
	GLFWwindow *wnd;

	long framesPerFPSMessage = 1000;
	// Every 'framesPerFPSMessage'th frame, this gets updated to now().
	// This is used to calculate FPS.
	std::chrono::high_resolution_clock::time_point timeNFramesAgo;
	// Increments each frame.
	long frameCount = 0L;

public:
	Window();
	~Window();
	void mainloop();

private:
	void loadShaders();
};