#pragma once

#include "stdio.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class Window
{
public:
	Window();

	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLint getBufferWidth() { 
		return (GLint) (bufferWidth);
	}
	GLint getBufferHeight() { 
		return (GLint) (bufferHeight);
	}

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getKeys() { 
		return keys; 
	}

	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;
	GLint width;
	GLint height;
	GLint bufferWidth;
	GLint bufferHeight;

	bool keys[1024] = { 0 };

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};