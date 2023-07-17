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

void	deleteMesh();

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
bool	pause = true;

float	timeStep = 1.0f / 120;		// 120fps
float	period = 4.0f;

// Current frame
int frame = 0;

int sweep_angle = 36;
int sweep = 18;

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

	// Finalization
	quit();

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

vec3 p[36][18];
vec4 w[36];

void
init()
{
	float		theta1 = 10;	// Degree
	float		theta2 = 20;	// Degree
	glm::vec3	axis1(0, 1, 0);
	glm::vec3	axis2(0, 0, 1);

	glm::vec4	temp;

	w[0] = glm::vec4(1, 1, 0, 1);

	// Identity matrix
	glm::mat4	M(1.0);
	glm::mat4	R(1.0);

	M = glm::rotate(M, glm::radians(theta1), axis1); // Radians

	for (int i = 0; i < 36; i++)
	{
		w[(i + 1) % 36] = M * w[i];

		for (int j = 0; j < 18; j++)
		{
			glm::vec4	std2(w[i].x, w[i].y + 0.5, w[i].z, 1);

			R = glm::translate(R, glm::vec3(w[i].x, w[i].y, w[i].z));
			R = glm::rotate(R, glm::radians(theta2), axis2);				// Radians
			R = glm::translate(R, -glm::vec3(w[i].x, w[i].y, w[i].z));

			std2 = R * std2;
			p[i][j] = glm::vec3(std2.x, std2.y, std2.z);
		}
		temp = M * glm::vec4(axis2.x, axis2.y, axis2.z, 1);
		axis2 = glm::vec3(temp.x, temp.y, temp.z);
	}

	// Keyboard
	cout << endl;
	cout << "Building/Drawing a Polygonal Torus Model" << endl;
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: arrow left, right for Sweep angle control around the y-axis" << endl;
	cout << "Keyboard input: arrow up, down for Torus Latitude control only when keyboard input 5" << endl;
	cout << endl;
	cout << "Keyboard input: 1 for Draw the wireframe only" << endl;
	cout << "Keyboard input: 2 for Draw the quads and the wireframe" << endl;
	cout << "Keyboard input: 3 for Two-sided constant shading" << endl;
	cout << "Keyboard input: 4 for Two-sided constant shading available in rotation animation" << endl;
	cout << "Keyboard input: 5 for Check the angle of the vector from the center of the polygon to the normal vector and COP." << endl;
}

void
quit()
{
	deleteMesh();
}

void
deleteMesh()
{
	for (int i = 0; i < 36; i++)
	{
		if (p[i])	delete[] p[i];
		if (w)		delete[] w;
		for (int j = 0; j < 18; j++)
		{
			p[i][j] = glm::vec3(NULL, NULL, NULL);
		}
	}

}

glm::vec3 v;
glm::vec3 v1;
glm::vec3 v2;
glm::vec3 n;
glm::vec3 ct;
float sinTheta;
float cosTheta;
float theta;

//Draw the wireframe only
void
drawwireframe()
{
	glPointSize(3 * dpiScaling);

	// Color of the points
	glColor3f(0, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
		}
	}

	glEnd();
}

//Draw the quads and the wireframe
void
drawquadsNwireframe()
{
	glPointSize(3 * dpiScaling);

	// Color of the points
	glColor3f(0, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
		}
	}

	glEnd();

	// Color of the points
	glColor3f(0, 0, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
		}
	}

	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
}

// Draw a Torus
void
drawTorus()
{
	glPointSize(3 * dpiScaling);

	// Color of the points
	glColor3f(0, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
		}
	}

	glEnd();


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			v1 = glm::vec3(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			v2 = glm::vec3(p[(i + 1) % 36][(j + 1) % 18].x - p[i][j].x, p[(i + 1) % 36][(j + 1) % 18].y - p[i][j].y, p[(i + 1) % 36][(j + 1) % 18].z - p[i][j].z);
			n = normalize(cross(v2, v1));

			ct.x = (p[i][j].x + p[i][(j + 1) % 18].x + p[(i + 1) % 36][j].x + p[(i + 1) % 36][(j + 1) % 18].x) / 4;
			ct.y = (p[i][j].y + p[i][(j + 1) % 18].y + p[(i + 1) % 36][j].y + p[(i + 1) % 36][(j + 1) % 18].y) / 4;
			ct.z = (p[i][j].z + p[i][(j + 1) % 18].z + p[(i + 1) % 36][j].z + p[(i + 1) % 36][(j + 1) % 18].z) / 4;
			v = normalize(eye - ct);

			cosTheta = dot(n, v);
			sinTheta = length(cross(n, v));
			theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);


			if (0 <= theta && theta <= 90)	glColor3f(0, 0, 1);

			else							glColor3f(1, 0, 0);

			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
		}
	}

	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
}


// rotate the view point
void
drawTorus_rv()
{
	glPointSize(3 * dpiScaling);

	// Color of the points
	glColor3f(0, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
		}
	}

	glEnd();

	float		theta1 = frame / period;	// Degree
	glm::vec3	axis(0, 1, 0);
	glm::vec3	result;
	glm::mat4	R(1.0);
	glm::vec3 cp;
	cp = eye;

	R = glm::rotate(R, glm::radians(-theta1), axis);
	glm::vec4 temp(cp.x, cp.y, cp.z, 1);
	result = R * temp;

	cp.x = result.x;
	cp.y = result.y;
	cp.z = result.z;

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			v1 = glm::vec3(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			v2 = glm::vec3(p[(i + 1) % 36][(j + 1) % 18].x - p[i][j].x, p[(i + 1) % 36][(j + 1) % 18].y - p[i][j].y, p[(i + 1) % 36][(j + 1) % 18].z - p[i][j].z);
			n = normalize(cross(v2, v1));

			ct.x = (p[i][j].x + p[i][(j + 1) % 18].x + p[(i + 1) % 36][j].x + p[(i + 1) % 36][(j + 1) % 18].x) / 4;
			ct.y = (p[i][j].y + p[i][(j + 1) % 18].y + p[(i + 1) % 36][j].y + p[(i + 1) % 36][(j + 1) % 18].y) / 4;
			ct.z = (p[i][j].z + p[i][(j + 1) % 18].z + p[(i + 1) % 36][j].z + p[(i + 1) % 36][(j + 1) % 18].z) / 4;
			v = normalize(cp - ct);

			cosTheta = dot(n, v);
			sinTheta = length(cross(n, v));
			theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);


			if (0 <= theta && theta <= 90)	glColor3f(0, 0, 1);

			else							glColor3f(1, 0, 0);

			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
		}
	}

	glEnd();

	glDisable(GL_POLYGON_OFFSET_FILL);
}


void
test()
{
	glPointSize(3 * dpiScaling);

	// Color of the points
	glColor3f(0, 0, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < sweep; j++)
		{
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
		}
	}

	glEnd();


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glm::vec3		cp(3.5, 3.0, 3.5);

	for (int i = 0; i < sweep_angle; i++)
	{
		for (int j = 0; j < sweep; j++)
		{

			v1 = glm::vec3(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			v2 = glm::vec3(p[(i + 1) % 36][(j + 1) % 18].x - p[i][j].x, p[(i + 1) % 36][(j + 1) % 18].y - p[i][j].y, p[(i + 1) % 36][(j + 1) % 18].z - p[i][j].z);
			n = normalize(cross(v2, v1));

			ct.x = (p[i][j].x + p[i][(j + 1) % 18].x + p[(i + 1) % 36][j].x + p[(i + 1) % 36][(j + 1) % 18].x) / 4;
			ct.y = (p[i][j].y + p[i][(j + 1) % 18].y + p[(i + 1) % 36][j].y + p[(i + 1) % 36][(j + 1) % 18].y) / 4;
			ct.z = (p[i][j].z + p[i][(j + 1) % 18].z + p[(i + 1) % 36][j].z + p[(i + 1) % 36][(j + 1) % 18].z) / 4;
			v = normalize(cp - ct);

			cosTheta = dot(n, v);
			sinTheta = length(cross(n, v));
			theta = (float)atan2(sinTheta, cosTheta) * float(180.0 / M_PI);


			glColor3f(0, 1, 0);
			glBegin(GL_LINES);
			glVertex3f(ct.x, ct.y, ct.z);
			glVertex3f(ct.x + n.x, ct.y + n.y, ct.z + n.z);
			glEnd();


			glBegin(GL_LINES);
			glVertex3f(ct.x, ct.y, ct.z);
			glVertex3f(cp.x, cp.y, cp.z);
			glEnd();


			if (0 <= theta && theta <= 90)	glColor3f(0, 0, 1);

			else							glColor3f(1, 0, 0);

			glBegin(GL_QUADS);
			glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
			glVertex3f(p[i][(j + 1) % 18].x, p[i][(j + 1) % 18].y, p[i][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][(j + 1) % 18].x, p[(i + 1) % 36][(j + 1) % 18].y, p[(i + 1) % 36][(j + 1) % 18].z);
			glVertex3f(p[(i + 1) % 36][j].x, p[(i + 1) % 36][j].y, p[(i + 1) % 36][j].z);
			glEnd();
		}
	}



	glDisable(GL_POLYGON_OFFSET_FILL);
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

	float		theta1 = frame / period;	// Degree
	glm::vec3	axis(0, 1, 0);
	glRotatef(theta1, axis.x, axis.y, axis.z);

	// Draw
	switch (selection)
	{
	case 1: 
		drawwireframe();
		eye = glm::vec3(3.5, 3.0, 3.5); 
		break;
	case 2:	
		drawquadsNwireframe();
		eye = glm::vec3(3.5, 3.0, 3.5); 
		break;
	case 3: 
		drawTorus();
		eye = glm::vec3(3.5, 3.0, 3.5); 
		break;
	case 4: 
		drawTorus_rv();
		eye = glm::vec3(3.5, 3.0, 3.5); 
		break;
	case 5: 
		test();
		eye = glm::vec3 (8, 6.5, 0);
		break;
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

		case GLFW_KEY_RIGHT:		if (sweep_angle < 36) sweep_angle += 1; break;
		case GLFW_KEY_LEFT:			if (sweep_angle > 0) sweep_angle -= 1; break;
		case GLFW_KEY_UP:			if (sweep < 18) sweep += 1; break;
		case GLFW_KEY_DOWN:			if (sweep > 0) sweep -= 1; break;

		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;
		case GLFW_KEY_5: selection = 5; break;
		}
	}
}
