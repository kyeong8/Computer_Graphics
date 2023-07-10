#include "glSetup.h"
#include <glm/gtx/string_cast.hpp>			// glm::to_string()
#include <glm/glm.hpp>				// OpenGL Mathematics
#include <glm/gtc/type_ptr.hpp>		// value_ptr()
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void	init();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

bool	readMesh(const char* filename);
void	deleteMesh();

// Carmera configuation
struct Camera
{
	vec3	e;		// Eye position
	vec3	a;		// At position
	vec3	u;		// Up vector (given -> orthonormal)

	vec3	f;		// Forward direction
	vec3	r;		// Right direction
}	C;

// Light configuration
vec4	light(0.0, 0.0, 0.8, 0);	// Light position

// Global coordinate frame
float	AXIS_LENGTH = 1.25;
float	AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// control variable
int		selection = 2;				// Selected example

// Predefined field of view angles in the y direction
float	fovys[6] = { 46.4f, 37.8f, 27.0f, 16.1f, 11.4f, 6.9f };
int		m_fovys = 6;
int		i_fovys = 1;

int
main(int argc, char* argv[])
{
	// Orthographic projection
	perspectiveView = true;

	// Initial camera setting
	C.e = vec3(0, 0, 3);	// Eye position
	C.a = vec3(0, 0, 0);	// At position
	C.u = vec3(0, 1, 0);	// Up vector (given)

	// Field of view of 35mm lens in degree
	fovy = fovys[i_fovys];

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL)	return	-1;

	// Callbacks
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
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();			// Events

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

// Sphere, cylinder
GLUquadricObj* sphere = NULL;

void
init()
{
	// Forward and backward direction for navigation
	C.f = normalize(C.a - C.e);			// Forward direction
	C.r = normalize(cross(C.f, C.u));	// Right direction
	C.u = normalize(cross(C.r, C.f));	// Up vector (orthogonal to f and r)

	// Prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GL_FALSE);

	// Prepare mesh
	readMesh("m01_bunny.off");

	// Keyboard
	cout << endl;
	cout << "Keyboard input: up, down, left, right for viewing" << endl;
	cout << "Keyboard input: p for perspective/orthographic viewing" << endl;
	cout << "Keyboard input: f to change field of view angle" << endl;
	cout << endl;
	cout << "Keyboard input: 1 for spheres" << endl;
	cout << "Keyboard input: 2 for flat bunny" << endl;
	cout << "Keyboard input: 3 for smooth bunny" << endl;
}

void
quit()
{
	// Delete quadric shapes
	gluDeleteQuadric(sphere);

	// Delete mesh
	deleteMesh();
}

// Material
void
setupColoredMaterial(const vec3& color)
{
	// Material
	GLfloat	mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// Draw a sphere using a GLU quadric
void
drawSphere(float radius, int slices, int stacks)
{
	gluSphere(sphere, radius, slices, stacks);
}

// Read a mesh from a given OFF file
int nVertices = 0, nFaces = 0, nEdges = 0;
vec3* vertex = NULL;
vec3* vnormal = NULL;	// Vertex normal
vec3* fnormal = NULL;	// Face normal
int* face[3] = { NULL, NULL, NULL };

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

	// Vertices
	vertex = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		is >> vertex[i].x >> vertex[i].y >> vertex[i].z;

	// Vertex normals
	vnormal = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		vnormal[i] = vec3(0, 0, 0);

	// Face normals
	fnormal = new vec3[nFaces];

	// Faces
	face[0] = new int[nFaces];	// Only support triangles
	face[1] = new int[nFaces];
	face[2] = new int[nFaces];

	int n;
	for (int i = 0; i < nFaces; i++)
	{
		is >> n >> face[0][i] >> face[1][i] >> face[2][i];
		if (n != 3) cout << "# vertices of the " << i << "-th faces = " << n << endl;

		// Normal vector of the face
		vec3	v1 = vertex[face[1][i]] - vertex[face[0][i]];
		vec3	v2 = vertex[face[2][i]] - vertex[face[0][i]];
		vec3	v = normalize(cross(v1, v2));

		fnormal[i] = v;

		// Add it to the normal vector of each vertex
		vnormal[face[0][i]] += v;
		vnormal[face[1][i]] += v;
		vnormal[face[2][i]] += v;
	}

	// Normalizaiton of the normal vectors
	for (int i = 0; i < nVertices; i++)
		vnormal[i] = normalize(vnormal[i]);

	return	true;
}

// Delete the vertices and faces of the mesh
void
deleteMesh()
{
	if (vertex) { delete[] vertex; vertex = NULL; }
	if (vnormal) { delete[] vnormal; vnormal = NULL; }
	if (fnormal) { delete[] fnormal; fnormal = NULL; }
	if (face[0]) { delete[] face[0]; face[0] = NULL; }
	if (face[1]) { delete[] face[1]; face[1] = NULL; }
	if (face[2]) { delete[] face[2]; face[2] = NULL; }
}

// Draw a flat mesh by specifying its face normal vectors
void
drawFlatMesh()
{
	// Geometry
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
	{
		glNormal3fv(value_ptr(fnormal[i]));
		for (int j = 0; j < 3; j++)
			glVertex3fv(value_ptr(vertex[face[j][i]]));
	}
	glEnd();
}

// Draw a smooth mesh by specifying its vertex normal vectors
void
drawSmoothMesh()
{
	// Geometry
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
		for (int j = 0; j < 3; j++)
		{
			glNormal3fv(value_ptr(vnormal[face[j][i]]));
			glVertex3fv(value_ptr(vertex[face[j][i]]));
		}
	glEnd();
}

// Light
void
setupLight(const vec4& p)
{
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));
}

void
render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modeview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(C.e[0], C.e[1], C.e[2], C.a[0], C.a[1], C.a[2], C.u[0], C.u[1], C.u[2]);

	// Axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);


	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Lighting
	//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	setupLight(light);

	// Draw objects
	//
	setupColoredMaterial(vec3(0.95, 0.95, 0.95));

	glPushMatrix();

	glScalef(0.7f, 0.7f, 0.7f);

	switch (selection)
	{
	case 1: drawSphere(0.7f, 64, 64);	break;
	case 2: drawFlatMesh();				break;
	case 3: drawSmoothMesh();			break;
	}

	glPopMatrix();
}


void
view_rotation(float x)
{
	glm::mat4	M(1.0);
	glm::mat4	R(1.0);


	M = glm::translate(M, C.e);
	M = glm::rotate(M, glm::radians(x), C.u);
	M = glm::translate(M, -C.e);
	C.a = glm::vec3(M * glm::vec4(C.a, 1));


	R = glm::rotate(R, glm::radians(x), C.u);
	C.f = glm::vec3(R * glm::vec4(C.f, 1));
	normalize(C.f);

	C.r = glm::vec3(R * glm::vec4(C.r, 1));
	normalize(C.r);
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

			// Perspective/orthographic view
		case GLFW_KEY_P:
			perspectiveView = !perspectiveView;
			reshape(window, windowW, windowH);
			break;

			// Field of view angle in the y direction
		case GLFW_KEY_F:
			i_fovys = (i_fovys + 1) % m_fovys;
			fovy = fovys[i_fovys];
			reshape(window, windowW, windowH);
			break;

			// Example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		}
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && !(mods & GLFW_MOD_SHIFT) && !(mods & GLFW_MOD_CONTROL))
	{
		switch (key)
		{
			// Navigation
		case GLFW_KEY_UP:		cout << "C location = " << to_string(C.e) << endl; C.e += 0.1f * C.f;	C.a += 0.1f * C.f; 	break;
		case GLFW_KEY_DOWN:		cout << "C location = " << to_string(C.e) << endl; C.e -= 0.1f * C.f;	C.a -= 0.1f * C.f;	break;
		case GLFW_KEY_LEFT:		C.e -= 0.1f * C.r;	C.a -= 0.1f * C.r;	break;
		case GLFW_KEY_RIGHT:	C.e += 0.1f * C.r;	C.a += 0.1f * C.r;	break;
		
		}
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && (mods & GLFW_MOD_SHIFT))
	{
		switch (key)
		{
			// Navigation
		case GLFW_KEY_LEFT:		view_rotation(10);	break;
		case GLFW_KEY_RIGHT:	view_rotation(-10);	break;
		}
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && (mods & GLFW_MOD_CONTROL))
	{
		switch (key)
		{
			// Navigation
		case GLFW_KEY_UP:		 C.e += 0.1f * C.u;  C.a += 0.1f * C.u; 	break;
		case GLFW_KEY_DOWN:		 C.e -= 0.1f * C.u;  C.a -= 0.1f * C.u;		break;
		}
	}
}

//#include <glm/gtx/string_cast.hpp>			// glm::to_string()
//cout << "C.e = " << to_string(C.e) << "C.a = " << to_string(C.a) << endl;
//C.u = glm::vec3(0, 1, 0);
//C.e -= 0.1f * C.u;