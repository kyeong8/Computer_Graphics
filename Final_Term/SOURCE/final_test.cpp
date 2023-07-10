#include <GL/glew.h>				// OpenGL Extention Wrangler Library
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
vec3	eye(2.8, 3.5, 2.8);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

// Global coordinate frame
float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

// Selected example
int selection = 1;
float m = 0.125;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };
GLfloat mat_blue[4] = { 0.1f, 0.1f, 1.0f, 1.0f };
GLfloat mat_red[4] = { 1.0f, 0.1f, 0.1f, 1.0f };

// Play configuration
bool	pause = true;

float	timeStep = 1.0f / 120;		// 120fps
float	period = 4.0f;

// Current frame
int frame = 0;

// OpenGL texture unit
GLuint	texID[4];

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

void
loadCheckerboardTexture()
{
	const int	w = 64;	const int	h = 64;
	GLubyte checkerboard[w][h];

	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			checkerboard[i][j] = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, w, h,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, checkerboard);
}

glm::vec3 vnormal[36][18];		// Vertex noraml
glm::vec3 fnormal[36][18];		// Face noraml
glm::vec3 p[36][18];
glm::vec4 w[36];

// Sphere, cylinder
GLUquadricObj* sphere = NULL;

void
init()
{
	glGenTextures(4, texID);

	// Checkerboard texture
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadCheckerboardTexture();

	float		theta1 = 10;	// Degree
	float		theta2 = 20;	// Degree
	glm::vec3	axis1(0, 1, 0);
	glm::vec3	axis2(0, 0, 1);

	glm::vec4	temp;

	w[0] = glm::vec4(1, 1, 0, 1);

	// Identity matrixr
	glm::mat4	M(1.0);
	glm::mat4	R(1.0);

	glm::vec3 v;
	glm::vec3 v1;
	glm::vec3 v2;

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

			vnormal[i][j] = glm::vec3(0, 0, 0);
		}
		temp = M * glm::vec4(axis2.x, axis2.y, axis2.z, 1);
		axis2 = glm::vec3(temp.x, temp.y, temp.z);
	}

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			v1 = glm::vec3(p[i][(j + 1) % 18].x - p[i][j].x, p[i][(j + 1) % 18].y - p[i][j].y, p[i][(j + 1) % 18].z - p[i][j].z);
			v2 = glm::vec3(p[(i + 1) % 36][(j + 1) % 18].x - p[i][j].x, p[(i + 1) % 36][(j + 1) % 18].y - p[i][j].y, p[(i + 1) % 36][(j + 1) % 18].z - p[i][j].z);
			v = normalize(cross(v2, v1));

			fnormal[i][j] = v;

			vnormal[i][j] += v;
			vnormal[(i + 1) % 36][j] += v;
			vnormal[i][(j + 1) % 18] += v;
			vnormal[(i + 1) % 36][(j + 1) % 18] += v;
		}
	}

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			vnormal[i][j] = normalize(vnormal[i][j]);
		}
	}

	// Prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);

	// Keyboard
	cout << endl;
	cout << "Building/Drawing a Polygonal Torus Model" << endl;
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: l for rotation of lights" << endl;
	cout << "Keyboard input: arrow left, right for Sweep angle control around the y-axis" << endl;
	cout << "Keyboard input: arrow up, down for shininess coefficient control" << endl;
	cout << "Keyboard input: p for a point light on/off" << endl;
	cout << "Keyboard input: a for Set the value of shininess coefficient to 15" << endl;
	cout << "Keyboard input: s for Set the value of shininess coefficient to 50" << endl;
	cout << endl;
}

// Draw a sphere using a GLU quadric
void
drawSphere(float radius, int slices, int stacks)
{
	gluSphere(sphere, radius, slices, stacks);
}

void
quit()
{
	gluDeleteQuadric(sphere);
	deleteMesh();
}

void
deleteMesh()
{
	if (p)	delete[] p;
	if (w)		delete[] w;
	if (vnormal) { delete[] vnormal; }
	if (fnormal) { delete[] fnormal; }
}


// Material
void
setupMaterial()
{
	// Material
	//GLfloat mat_ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
	GLfloat mat_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess = 128;

	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// Light
void
setupLight(const vec4& p)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//GLfloat ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat diffuse[4] = { 0.5f, 0.5f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
}

// Draw a Torus
void
drawTorus()
{
	glPointSize(3 * dpiScaling);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_QUADS);

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glNormal3fv(value_ptr(vnormal[i][j]));
			glVertex3fv(value_ptr(p[i][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][j]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][(j + 1) % 18]));

			glNormal3fv(value_ptr(vnormal[i][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[i][(j + 1) % 18]));
		}
	}

	glEnd();
}

// Draw a Torus
void
drawTorusnwire()
{
	glPointSize(3 * dpiScaling);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < 36; i++)
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

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glNormal3fv(value_ptr(vnormal[i][j]));
			glVertex3fv(value_ptr(p[i][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][j]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][(j + 1) % 18]));

			glNormal3fv(value_ptr(vnormal[i][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[i][(j + 1) % 18]));
		}
	}

	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
}

// Draw a Torus
void
drawTorusnwirennormal()
{
	glPointSize(3 * dpiScaling);

	glBegin(GL_LINES);

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glVertex3fv(value_ptr(p[i][j]));
			glVertex3f(p[i][j].x + 0.5 * vnormal[i][j].x, p[i][j].y + 0.5 * vnormal[i][j].y, p[i][j].z + 0.5 * vnormal[i][j].z);
		}
	}

	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	for (int i = 0; i < 36; i++)
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

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			glNormal3fv(value_ptr(vnormal[i][j]));
			glVertex3fv(value_ptr(p[i][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][j]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][j]));

			glNormal3fv(value_ptr(vnormal[(i + 1) % 36][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[(i + 1) % 36][(j + 1) % 18]));

			glNormal3fv(value_ptr(vnormal[i][(j + 1) % 18]));
			glVertex3fv(value_ptr(p[i][(j + 1) % 18]));
		}
	}

	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void
drawCheckerTorus()
{	
	//glDisable(GL_LIGHTING);

	glPointSize(3 * dpiScaling);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_QUADS);

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			if ((i + j) % 2 == 0)
			{
				glColor3f(1, 1, 1);
				glNormal3fv(value_ptr(vnormal[i][j]));
				glTexCoord2f(0, 0);
				glVertex3fv(value_ptr(p[i][j]));

				glNormal3fv(value_ptr(vnormal[(i + 1) % 36][j]));
				glTexCoord2f(m, 0);
				glVertex3fv(value_ptr(p[(i + 1) % 36][j]));

				glNormal3fv(value_ptr(vnormal[(i + 1) % 36][(j + 1) % 18]));
				glTexCoord2f(m, m);
				glVertex3fv(value_ptr(p[(i + 1) % 36][(j + 1) % 18]));

				glNormal3fv(value_ptr(vnormal[i][(j + 1) % 18]));
				glTexCoord2f(0, m);
				glVertex3fv(value_ptr(p[i][(j + 1) % 18]));
			}
			else
			{
				glColor3f(1, 1, 1);
				glNormal3fv(value_ptr(vnormal[i][j]));
				glTexCoord2f(m, 0);
				glVertex3fv(value_ptr(p[i][j]));

				glNormal3fv(value_ptr(vnormal[(i + 1) % 36][j]));
				glTexCoord2f(m+m, 0);
				glVertex3fv(value_ptr(p[(i + 1) % 36][j]));

				glNormal3fv(value_ptr(vnormal[(i + 1) % 36][(j + 1) % 18]));
				glTexCoord2f(m+m, m);
				glVertex3fv(value_ptr(p[(i + 1) % 36][(j + 1) % 18]));

				glNormal3fv(value_ptr(vnormal[i][(j + 1) % 18]));
				glTexCoord2f(m, m);
				glVertex3fv(value_ptr(p[i][(j + 1) % 18]));
			}
		}
	}

	glEnd();

	//cout << value_ptr(p[0][17]) << endl;
	//cout << value_ptr(p[35][17]) << endl;
	//cout << value_ptr(p[35][0]) << endl;
	//cout << value_ptr(p[0][0]) << endl;
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

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);

	vec4	lightP(2, 2.5, 2, 1);

	glEnable(GL_LIGHT0);

	// Lighting
	setupLight(lightP);

	// Material
	setupMaterial();
	
	// Draw
	switch (selection)
	{
	case 1:
		glDisable(GL_TEXTURE_2D);
		drawTorus();
		break;
	case 2:
		glDisable(GL_TEXTURE_2D);
		drawTorusnwire();
		break;
	case 3:
		glDisable(GL_TEXTURE_2D);
		drawTorusnwirennormal();
		break;
	case 4:
		{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texID[0]);	// Checkerboard texture
		drawCheckerTorus();
		}
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

		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;

		case GLFW_KEY_UP:		m += 0.01;	break;
		case GLFW_KEY_DOWN:		m -= 0.01;	break;
		}
	}
}
