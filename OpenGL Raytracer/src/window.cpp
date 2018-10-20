#include "window.h"

#include <stdexcept>
#include <vector>

#include "readfile.h"
#include "uniform_manager.h"

Window::Window() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW");
	}

	// OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	wnd = glfwCreateWindow(1024, 768, "Raytracer", nullptr, nullptr);
	glfwMakeContextCurrent(wnd);
	glfwSwapInterval(0);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialise GLEW");
	}

	glViewport(0, 0, 1024, 768);
	loadShaders();
}

Window::~Window() {
	glfwDestroyWindow(wnd);
	glfwTerminate();
}

void Window::mainloop() {
	glClearColor(0, 0, 0, 1);

	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// These vertices define two triangles that fill the entire viewport.
	GLfloat vertices[] = {
		1, 1, 0,
		1, -1, 0,
		-1, 1, 0,
		-1, 1, 0,
		-1, -1, 0,
		1, -1, 0
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	timeNFramesAgo = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(wnd)) {
		// Update everything.
		update_uniforms();

		// Draw everything.
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(wnd);
		glfwPollEvents();

		// Update FPS counter.
		++frameCount;
		if (frameCount % framesPerFPSMessage == 0) {
			// Display an FPS message to the command line.
			auto now = std::chrono::high_resolution_clock::now();
			long long duration_us = std::chrono::duration_cast<std::chrono::microseconds>(now - timeNFramesAgo).count();
			timeNFramesAgo = now;

			// Time for one frame in microseconds.
			float frameTime_us = ((float)duration_us) / framesPerFPSMessage;
			float fps = 1000000.f / frameTime_us;
			printf("%d FPS\n", (int)fps);
		}
	}
}

void Window::loadShaders() {
	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

	char *vertSrc = readFile("shaders/shader.vert");
	char *fragSrc = readFile("shaders/shader.frag");

	glShaderSource(vertID, 1, &vertSrc, NULL);
	glShaderSource(fragID, 1, &fragSrc, NULL);
	glCompileShader(vertID);
	glCompileShader(fragID);

	delete[] vertSrc;
	delete[] fragSrc;

	GLint result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(vertID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> error(infoLogLength + 1);
		glGetShaderInfoLog(vertID, infoLogLength, NULL, &error[0]);
		printf("Vertex shader error: %s\n", &error[0]);
	}

	glGetShaderiv(fragID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> error(infoLogLength + 1);
		glGetShaderInfoLog(fragID, infoLogLength, NULL, &error[0]);
		printf("Fragment shader error: %s\n", &error[0]);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertID);
	glAttachShader(program, fragID);
	glLinkProgram(program);

	glGetProgramiv(program, GL_COMPILE_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> error(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, NULL, &error[0]);
		printf("Shader link error: %s\n", &error[0]);
	}

	glDeleteShader(vertID);
	glDeleteShader(fragID);

	glUseProgram(program);

	init_uniforms(program);
}
