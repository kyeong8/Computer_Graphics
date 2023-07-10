#include "glSetup.h"

#ifdef  _WIN32
#define _USE_MATH_DEFINES	// To include the definition of M_PI in math.h
#endif

#include <glm/glm.hpp>				// OpenGL Mathematics
#include <glm/gtc/type_ptr.hpp>		// value_ptr()
using namespace glm;

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


void	init();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

bool	readMesh(const char* filename);
void	deleteMesh();

// Carmera configuation
vec3	eye(2, 2, 2);
vec3	center(0, 0.2f, 0);
vec3	up(0, 0, 1);

// Light configuration
vec4	light(5.0, 5.0, 0.0, 1);	// Light position
// Global coordinate frame
float	AXIS_LENGTH = 1.75;
float	AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Selected example
int selection = 1;


// Play configuration
bool	pause = true;

float	timeStep = 1.0f / 120;	// 120fps
float	period = 8.0;
float	theta = 0;

// Drawing parameters
bool	depthTest = true;	// Depth test
bool	polygonFill = true;	// Polygon mode

int
main(int argc, char* argv[])
{
	// FPS control: vsync should be 0 for precise time stepping
	vsync = 0;	// 0 for immediate mode (Tearing possible)

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return	-1;

	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	// Turn off back face culling for cylinder
	glDisable(GL_CULL_FACE);

	// Viewport and perspecive setting
	reshape(window, windowW, windowH);

	// Initialization - Main loop - Finalization
	//
	init();

	// Main loop
	float	previous = (float)glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();			// Events

		// Time passed during a single loop
		float	now = (float)glfwGetTime();
		float	delta = now - previous;
		previous = now;

		// Time passed after the previous frame
		elapsed += delta;

		// Deal with the current frame
		if (elapsed > timeStep)
		{
			// Rotate the line segment if not paused
			if (!pause)
			{
				// One rotation per period
				theta += float(2.0 * M_PI) / period * elapsed;
			}

			elapsed = 0;		// Reset the elapsed time
		}

		render(window);				// Draw one frame
		glfwSwapBuffers(window);	// Swap buffers
	}

	// Finalization
	quit();

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void
init()
{
	// Prepare mesh
	readMesh("m01_bunny.off");

	// Keyboard
	cout << endl;
	cout << "Keyboard Input: d for depth test on/off" << endl;
	cout << "Keyboard Input: f for polygon fill on/off" << endl;
	cout << endl;
	cout << "Keyboard Input: 4 for bunny" << endl;
}

void
quit()
{
	// Delete mesh
	deleteMesh();
}

// Light
void
setupLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));

	// For disk
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
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

// Read a mesh from a given OFF file
int		nVertices = 0, nFaces = 0, nEdges = 0;
vec3* vertex = NULL;
int* face[3] = { NULL, NULL, NULL };
vector<vector<int>> edge;

bool
readMesh(const char* filename)
{
	ifstream	is(filename);
	if (is.fail())	return	false;

	char	magicNumber[256];
	is >> magicNumber;

	// # vertices, # faces, # edges
	is >> nVertices >> nFaces >> nEdges;
	cout << "# vertices = " << nVertices << endl;
	cout << "# faces = " << nFaces << endl;
	cout << "# Edges = " << nEdges << endl;

	// Vertices
	vertex = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
	{
		is >> vertex[i].x >> vertex[i].y >> vertex[i].z;
		vector<int> v;
		edge.push_back(v);
	}

	// Faces
	face[0] = new int[nFaces];	// Only support triangles
	face[1] = new int[nFaces];
	face[2] = new int[nFaces];

	int k;
	int n;
	for (int i = 0; i < nFaces; i++)
	{
		is >> n >> face[0][i] >> face[1][i] >> face[2][i];
		for (int j = 0; j < 3; j++)
		{
			k = (j + 1) % 3;	// endpoints ot the edge (index j, k) 
			if (face[j][i] > face[k][i] && !count(edge[face[j][i]].begin(), edge[face[j][i]].end(), face[k][i]))	// Small index vertices and non - existent
				edge[face[j][i]].push_back(face[k][i]);
			else if (face[k][i] > face[j][i] && !count(edge[face[k][i]].begin(), edge[face[k][i]].end(), face[j][i]))
				edge[face[k][i]].push_back(face[j][i]);
		}
		if (n != 3) cout << "# vertices of the " << i << "-th faces = " << n << endl;
	}

	int total = 0;	// count all edges
	for (int i = 0; i < nVertices; i++) total += edge[i].size();

	cout << "count of edge = " << total << endl;
	return	true;
}

// Delete the vertices and faces of the mesh
void
deleteMesh()
{
	if (vertex) { delete[] vertex; vertex = NULL; }
	if (face[0]) { delete[] face[0]; face[0] = NULL; }
	if (face[1]) { delete[] face[1]; face[1] = NULL; }
	if (face[2]) { delete[] face[2]; face[2] = NULL; }
}

// Draw the vertices of the mesh only
void
drawMesh()
{
	// Color of the points
	setDiffuseColor(vec3(0, 0, 0));

	// Place the bunny model
	glRotatef(90, 0, 0, 1);
	glRotatef(90, 1, 0, 0);

	// Draw all the vertices of the bunny model
	//glPointSize(3 * dpiScaling);

	glBegin(GL_LINES);
	for (int i = 0; i < nVertices; i++)
	{
		for (int j = 0; j < edge[i].size(); j++)	// vertices that make up all edges
		{
			glVertex3f(vertex[i].x, vertex[i].y, vertex[i].z);
			glVertex3f(vertex[edge[i][j]].x, vertex[edge[i][j]].y, vertex[edge[i][j]].z);
		}
	}
	glEnd();
}

void
render(GLFWwindow* window)
{
	// Depth test
	if (depthTest)	glEnable(GL_DEPTH_TEST);
	else            glDisable(GL_DEPTH_TEST);

	// Polygon mode
	if (polygonFill)	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modeview matrix
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

	// Rotation about the z-axis
	glRotatef(degrees(theta), 0, 0, 1); // Radian to degree

	// Draw sphere using gluSphere() after setting up the material
	switch (selection)
	{
	case 4: drawMesh();										break;
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

			// Example selection
		case GLFW_KEY_4: selection = 4; break;

			// Play on/off
		case GLFW_KEY_SPACE: pause = !pause; break;

			// Depth test on/off
		case GLFW_KEY_D: depthTest = !depthTest;	break;

			// Polygon fill on/off
		case GLFW_KEY_F: polygonFill = !polygonFill;	break;
		}
	}
}
