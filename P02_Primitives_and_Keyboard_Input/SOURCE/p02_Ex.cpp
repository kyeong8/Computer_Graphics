#include "glSetup.h"
#include <iostream>

void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Color
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Control variable
GLenum	mode = GL_POINTS;
bool	stipple = false;
bool	fill = true;

int
main(int argc, char* argv[])
{
	// Orthographics viewing
	perspectiveView = false;

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return  -1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Usage
	std::cout << std::endl;
	std::cout << "Keyboard Input: 1 GL_POINTS" << std::endl;
	std::cout << "Keyboard Input: 2 GL_QUADS" << std::endl;
	std::cout << "Keyboard Input: 3 GL_QUAD_STRIP" << std::endl;
	std::cout << std::endl;
	std::cout << "Keyboard Input: f for polygon filling on/off" << std::endl;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		render(window);				// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
		glfwPollEvents();			// Events
	}

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void
render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Stipple
	if (stipple)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);
	}
	else    glDisable(GL_LINE_STIPPLE);

	// Polygon mode
	if (fill)	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Point size
	glPointSize(5 * dpiScaling);

	// Line width
	glLineWidth(5 * dpiScaling);

	// Drawing with various primitive types
	if (mode == GL_QUADS)
	{
		glBegin(mode);
		{
			glColor3f(1, 0, 0);
			glVertex2f(-1.5, -0.5);
			glVertex2f(-1.5, 0.5);
			glVertex2f(-0.5, 0.5);
			glVertex2f(-0.5, -0.5);
			glColor3f(0, 1, 0);
			glVertex2f(0.5, -0.5);
			glVertex2f(0.5, 0.5);
			glVertex2f(1.5, 0.5);
			glVertex2f(1.5, -0.5);
		}
		glEnd();
	}
	else if (mode == GL_QUAD_STRIP)
	{
		glBegin(mode);
		{
			glColor3f(1, 0, 0);
			glVertex2f(-1.5, -0.5);
			glVertex2f(-1.5, 0.5);
			glColor3f(0, 1, 0);
			glVertex2f(-0.5, -0.5);
			glVertex2f(-0.5, 0.5);
			glVertex2f(0.5, -0.5);
			glVertex2f(0.5, 0.5);
			glColor3f(0, 0, 1);
			glVertex2f(1.5, -0.5);
			glVertex2f(1.5, 0.5);
		}
		glEnd();
	}
	else
	{
		glBegin(mode);
		{
			glColor3f(1, 0, 0);
			glVertex2f(-1.5, -0.5);
			glVertex2f(-1.5, 0.5);
			glColor3f(0, 1, 0);
			glVertex2f(-0.5, 0.5);
			glVertex2f(-0.5, -0.5);
			glVertex2f(0.5, -0.5);
			glVertex2f(0.5, 0.5);
			glColor3f(0, 0, 1);
			glVertex2f(1.5, 0.5);
			glVertex2f(1.5, -0.5);
		}
		glEnd();
	}
	
}

void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			// Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			// Control for stippling and polygon filling
		case GLFW_KEY_F: fill = !fill;			break;

			// Numeric keys to specify the primitives
		case GLFW_KEY_1:	mode = GL_POINTS;			break;
		case GLFW_KEY_2:	mode = GL_QUADS;			break;
		case GLFW_KEY_3:	mode = GL_QUAD_STRIP;		break;

		}
	}
}
