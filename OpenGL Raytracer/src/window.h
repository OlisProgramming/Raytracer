#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
private:
	GLFWwindow *wnd;

public:
	Window();
	~Window();
	void mainloop();

private:
	void loadShaders();
};