#include "glSetup.h"

#ifdef  _WIN32
#define _USE_MATH_DEFINES	// To include the definition of M_PI in math.h
#endif

#include <glm/glm.hpp>				// OpenGL Mathmatics
#include <glm/gtx/string_cast.hpp>	// glm::to_string()
#include <glm/gtc/type_ptr.hpp>		// glm::value_ptr()
using namespace glm;

#include <iostream>
using namespace std;

void	init();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Camera configuation
vec3	eye(3.5, 3, 3.5);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

// Lignt configuation
vec4	light(5.0, 5.0, 5.0, 1);	// Light position

// Global coordinate frame
float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Selected example
int selection = 1;

// Play configuration
bool	pause = false;

float	timeStep = 1.0f / 120;		// 120fps
float	period = 4.0f;

// Current frame
int frame = 0;

int
main(int argc, char* argv[])
{
	// vsync should be 0 for precise time stepping.
	vsync = 0;

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return	-1;

	// callbacks
	glfwSetKeyCallback(window, keyboard);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Initialization - Main loop - Finalization
	//
	init();

	// Main loop
	float	previous = (float)glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();				// Events

		// Time passed during a single loop
		float	now = (float)glfwGetTime();
		float	delta = now - previous;
		previous = now;

		// Time passed after the previous frame
		elapsed += delta;

		// Deal with the currnet frame
		if (elapsed > timeStep)
		{
			// Animate 1 frame
			if (!pause) frame += 1;

			elapsed = 0;	// Reset the elapsed time
		}

		render(window);				// Draw one frame
		glfwSwapBuffers(window); 	// Swap buffers
	}


	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void
init()
{
	// Keyboard
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << endl;
}


// Light
void
setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

// Material
void
setupMaterial()
{
	// Material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 128;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void
setDiffuseColor(const vec3& color)
{
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}

// Draw a unit cube
void
drawCube()
{
	// Cube
	glBegin(GL_QUADS);

	// Front
	glNormal3f(0, 0, 1);

	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(0, 1, 1);

	// Back
	glNormal3f(0, 0, -1);

	glVertex3f(0, 1, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 0, 0);

	// Right
	glNormal3f(1, 0, 0);

	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(1, 1, 1);

	// Left
	glNormal3f(-1, 0, 0);

	glVertex3f(0, 1, 1);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);

	// Top
	glNormal3f(0, 1, 0);

	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);

	// Bottom
	glNormal3f(0, -1, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 0, 1);
	glVertex3f(0, 0, 1);

	glEnd();
}

void
two()
{
	float		s = 0.2f;
	glm::vec3	direction(1, 1, 0);
	direction = normalize(direction);	// Unit vector

	// Compute the rotation axis and angle between the x-axis and the given direction
	glm::vec3	axis = cross(glm::vec3(1, 0, 0), direction);
	float		sinTheta = length(axis);
	float		cosTheta = dot(glm::vec3(1, 0, 0), direction);
	float		theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);
	axis /= sinTheta;	// Unit axis

		// Identity matrix
	glm::mat4	M(1.0);

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(theta), axis); // Radians

	// M = M * scale(angle, axis)
	M = glm::scale(M, glm::vec3(s, 1, 1)); // Radians

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(-theta), axis); // Radians

	// Multiply M to the current matrix of OpenGL
	// (OpenGL employs right multiplication of matrix.)
	// (M need to be transposed and then fed in the row-major order.)
	// (Meanwhile, GLM represents M in the column-major order.)
	// (Thus, M can be fed into OpenGL without any modifications.)
	glMultMatrixf(value_ptr(M));

	setDiffuseColor(glm::vec3(1, 1, 1));		// Material
	drawCube();
}

void
three()
{
	float		s = 0.2f;
	glm::vec3	direction(1, 1, 0);
	glm::vec3	pivot(0.5, 0.5, 0.5);
	direction = normalize(direction);	// Unit vector

	// Compute the rotation axis and angle between the x-axis and the given direction
	glm::vec3	axis = cross(glm::vec3(1, 0, 0), direction);
	float		sinTheta = length(axis);
	float		cosTheta = dot(glm::vec3(1, 0, 0), direction);
	float		theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);
	axis /= sinTheta;	// Unit axis

		// Identity matrix
	glm::mat4	M(1.0);

	M = glm::translate(M, pivot);
	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(theta), axis); // Radians

	// M = M * scale(angle, axis)
	M = glm::scale(M, glm::vec3(s, 1, 1)); // Radians

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(-theta), axis); // Radians
	M = glm::translate(M, -pivot);

	// Multiply M to the current matrix of OpenGL
	// (OpenGL employs right multiplication of matrix.)
	// (M need to be transposed and then fed in the row-major order.)
	// (Meanwhile, GLM represents M in the column-major order.)
	// (Thus, M can be fed into OpenGL without any modifications.)
	glMultMatrixf(value_ptr(M));

	setDiffuseColor(glm::vec3(1, 1, 1));		// Material
	drawCube();
}

void
four()
{
	float		s = 0.2f;
	glm::vec3	direction(1, 1, 0);
	glm::vec3	pivot(0.5, 0.5, 0.5);
	glm::vec3	P(0.5, 0.5, 0);
	direction = normalize(direction);	// Unit vector

	// Compute the rotation axis and angle between the x-axis and the given direction
	glm::vec3	axis = cross(glm::vec3(1, 0, 0), direction);
	float		sinTheta = length(axis);
	float		cosTheta = dot(glm::vec3(1, 0, 0), direction);
	float		theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);
	axis /= sinTheta;	// Unit axis

		// Identity matrix
	glm::mat4	M(1.0);

	M = glm::translate(M, P);
	M = glm::translate(M, pivot);
	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(theta), axis); // Radians

	// M = M * scale(angle, axis)
	M = glm::scale(M, glm::vec3(s, 1, 1)); // Radians

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(-theta), axis); // Radians
	M = glm::translate(M, -pivot);

	// Multiply M to the current matrix of OpenGL
	// (OpenGL employs right multiplication of matrix.)
	// (M need to be transposed and then fed in the row-major order.)
	// (Meanwhile, GLM represents M in the column-major order.)
	// (Thus, M can be fed into OpenGL without any modifications.)
	glMultMatrixf(value_ptr(M));

	setDiffuseColor(glm::vec3(1, 1, 1));		// Material
	drawCube();
}

void
five()
{
	float		s = 0.2f;
	float		theta1 = frame * 4 / period;
	glm::vec3	direction(1, 1, 0);
	glm::vec3	pivot(0.5, 0.5, 0.5);
	glm::vec3	P(0.5, 0.5, 0);
	direction = normalize(direction);	// Unit vector

	// Compute the rotation axis and angle between the x-axis and the given direction
	glm::vec3	axis = cross(glm::vec3(1, 0, 0), direction);
	float		sinTheta = length(axis);
	float		cosTheta = dot(glm::vec3(1, 0, 0), direction);
	float		theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);
	axis /= sinTheta;	// Unit axis

	// Identity matrix
	glm::mat4	M(1.0);

	M = glm::rotate(M, glm::radians(theta1), glm::vec3(0, 0, 1)); // Radians

	M = glm::translate(M, P);
	M = glm::translate(M, pivot);
	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(theta), axis); // Radians

	// M = M * scale(angle, axis)
	M = glm::scale(M, glm::vec3(s, 1, 1)); // Radians

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(-theta), axis); // Radians
	M = glm::translate(M, -pivot);

	// Multiply M to the current matrix of OpenGL
	// (OpenGL employs right multiplication of matrix.)
	// (M need to be transposed and then fed in the row-major order.)
	// (Meanwhile, GLM represents M in the column-major order.)
	// (Thus, M can be fed into OpenGL without any modifications.)
	glMultMatrixf(value_ptr(M));

	setDiffuseColor(glm::vec3(1, 1, 1));		// Material
	drawCube();
}

void
six()
{
	float		s = 0.2f;
	float		theta1 = frame * 4 / period;
	glm::vec3	direction(1, 1, 0);
	glm::vec3	pivot(0.5, 0.5, 0.5);
	glm::vec3	P(0.5, 0.5, 0);
	direction = normalize(direction);	// Unit vector

	// Compute the rotation axis and angle between the x-axis and the given direction
	glm::vec3	axis = cross(glm::vec3(1, 0, 0), direction);
	float		sinTheta = length(axis);
	float		cosTheta = dot(glm::vec3(1, 0, 0), direction);
	float		theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);
	axis /= sinTheta;	// Unit axis

		// Identity matrix
	glm::mat4	M(1.0);
	M = glm::translate(M, (pivot + P));
	M = glm::rotate(M, glm::radians(theta1), glm::vec3(0, 0, 1)); // Radians
	M = glm::translate(M, -(pivot + P));
	M = glm::translate(M, P);
	M = glm::translate(M, pivot);
	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(theta), axis); // Radians

	// M = M * scale(angle, axis)
	M = glm::scale(M, glm::vec3(s, 1, 1)); // Radians

	// M = M * rotate(angle, axis)
	M = glm::rotate(M, glm::radians(-theta), axis); // Radians
	M = glm::translate(M, -pivot);

	// Multiply M to the current matrix of OpenGL
	// (OpenGL employs right multiplication of matrix.)
	// (M need to be transposed and then fed in the row-major order.)
	// (Meanwhile, GLM represents M in the column-major order.)
	// (Thus, M can be fed into OpenGL without any modifications.)
	glMultMatrixf(value_ptr(M));

	setDiffuseColor(glm::vec3(1, 1, 1));		// Material
	drawCube();
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

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	// Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	// Lighting
	setupLight();

	// Material
	setupMaterial();

	// Draw
	switch (selection)
	{
	case 1: drawCube();			break;
	case 2: two();				break;
	case 3: three();			break;
	case 4: four();				break;
	case 5: five();				break;
	case 6: six();				break;
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
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE);	break;

			// Play on/off
		case GLFW_KEY_SPACE:		pause = !pause;	break;

			// Example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;
		case GLFW_KEY_5: selection = 5; break;
		case GLFW_KEY_6: selection = 6; break;
		}
	}
}
