#include "glSetup.h"

#ifdef _WIN32
#define _USE_MATH_DEFINES   // To include the definition of M_PI in math.h
#endif // _WIN32

#include <glm/glm.hpp>            // OpenGL Mathematics
#include <glm/gtx/string_cast.hpp>   // glm::to_string()
#include <glm/gtc/type_ptr.hpp>      // glm::value_ptr()
using namespace glm;

#include <iostream>
using namespace std;

void   init();
void   quit();
void   render(GLFWwindow* window);
void   keyboard(GLFWwindow* window, int key, int code, int aciton, int mods);

// Camera configuation
vec3   eye(3.5, 3, 3.5);
vec3   center(0, 0, 0);
vec3   up(0, 1, 0);

// Light configuration
vec4   light(5.0, 5.0, 5.0, 1);   // Light position

// Global coordinate frame
float   AXIS_LENGTH = 3;
float   AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

// Selected example
int selection = 1;

// Paly configuration
bool   pause = false;

float   timeStep = 1.0f / 120;   // 120fps
float   period = 4.0f;

// Current frame
int frame = 0;

int
main(int argc, char* argv[])
{
    // vsync should be 0 for precise time stepping.
    vsync = 0;

    // Initialize the OpenGL system
    GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
    if (window == NULL) return   -1;

    // Callbacks
    glfwSetKeyCallback(window, keyboard);

    // Depth test
    glEnable(GL_DEPTH_TEST);

    // Normal vectors are normalized after transformation.
    glEnable(GL_NORMALIZE);

    // Viewport and perspective setting
    reshape(window, windowW, windowH);

    // Initialization - Main loop - Finalizaton
    //
    init();

    // Main loop
    float previous = (float)glfwGetTime();
    float elapsed = 0;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();         // Events

        // Time passed durring a single loop
        float   now = (float)glfwGetTime();
        float   delta = now - previous;
        previous = now;

        // Time passed after the previous frame
        elapsed += delta;

        // Deal with the current frame
        if (elapsed > timeStep)
        {
            // Animate 1 frame
            if (!pause) frame += 1;

            elapsed = 0;   // Reset the elapsed time
        }

        render(window);            // Draw one frame
        glfwSwapBuffers(window);   // Swap buffers
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
    // Prepare quadric shapes


    // Keyboard
    cout << endl;
    cout << "Keyboard input: space for play/pause" << endl;
    cout << endl;
    cout << "Keyboard input: 1 for rotation example" << endl;
    cout << "Keyboard input: 2 for rotation wrt a pivot example" << endl;
    cout << "Keyboard input: 3 for rotation wrt a pivot example usig GLM" << endl;
    cout << "Keyboard input: 4 for scaling example" << endl;
    cout << "Keyboard input: 5 for scaling wrt a pivot example" << endl;
    cout << "Keyboard input: 6 for scaling wrt a direction example" << endl;
    cout << "Keyboard input: 7 for a solar system example" << endl;
}

void
quit()
{
    // Delete quadric shapes

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
    GLfloat mat_shiniess = 128;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiniess);
}

void
setDiffuseColor(const vec3& color)
{
    GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}


//major Radius
float majorRadius = 1.2f;
//minor Radius
float minorRadius = 0.1f;
//height
float height = 0.6f;

// main axis
glm::vec3 mainAxis(0, 1, 0);

// point of Torus
glm::vec3 p[36][18];
// mainCircle of Torus
glm::vec3 mainCircle[36];
// subCircle of Torus
glm::vec3 subCircle[18];


void
getCircleAbout_Axis_At_Pivot(glm::vec3 circle[], int lines, glm::vec3 axis, glm::vec3 pivot) {
    //Get Theta
    float theta = 360.0f / lines;

    glm::vec4 temppoint;
    temppoint.x = circle[0].x;
    temppoint.y = circle[0].y;
    temppoint.z = circle[0].z;
    temppoint.w = 0.0f;


    // Identity matrix
    glm::mat4   M(1.0);
    // M = M * translate(pivot)
    M = glm::translate(M, pivot);   // Radians

    // M = M * rotate(angle, axis)
    M = glm::rotate(M, glm::radians(theta), axis);   // Radians

    // M = M * translate(pivot)
    M = glm::translate(M, -pivot);   // Radians

    for (int i = 1; i < lines; i++) {
        temppoint = temppoint * M;
        circle[i].x = temppoint.x;
        circle[i].y = temppoint.y;
        circle[i].z = temppoint.z;
    }
}

void
getmainCircle()
{
    mainCircle[0].x = majorRadius;
    mainCircle[0].y = height;
    mainCircle[0].z = 0.0f;

    glm::vec3 pivot(0, height, 0);

    getCircleAbout_Axis_At_Pivot(mainCircle, 36, mainAxis, pivot);
}

void
getsubCircle()
{
    subCircle[0].x = 0.0f;
    subCircle[0].y = height + minorRadius;
    subCircle[0].z = 0.0f;
    glm::vec3 sub_axis(0, 0, 1);
    glm::vec3 pivot(0, height, 0);

    getCircleAbout_Axis_At_Pivot(subCircle, 18, sub_axis, pivot);
}


// Cube
void drawmainCircle() {
    getmainCircle();

    glBegin(GL_POINTS);
    for (int i = 0; i < 36; i++) {
        glVertex3f(mainCircle[i].x, mainCircle[i].y, mainCircle[i].z);
    }
    glEnd();
}

void drawsubCircle()
{
    getsubCircle();
    glBegin(GL_POINTS);
    for (int i = 0; i < 18; i++) {
        glVertex3f(subCircle[i].x, subCircle[i].y, subCircle[i].z);
    }
    glEnd();
}



void getTorus() {
    getmainCircle();
    getsubCircle();
    float theta = 0.0f;

    for (int i = 0; i < 36; i++) {

        for (int j = 0; j < 18; j++) {
            //Using Homogenious coordinate
            glm::vec4 temppoint;

            temppoint.x = subCircle[j].x;
            temppoint.y = subCircle[j].y;
            temppoint.z = subCircle[j].z;
            temppoint.w = 1.0f;

            //Rotating, translation
            // Identity matrix
            glm::mat4   M(1.0);

            // M = M * translate(subCircle's point)
            M = glm::translate(M, mainCircle[i]);

            // M = M * rotate(angle, axis)
            M = glm::rotate(M, glm::radians(theta), mainAxis);   // Radians

            temppoint = M * temppoint;

            p[i][j].x = temppoint.x;
            p[i][j].y = temppoint.y;
            p[i][j].z = temppoint.z;


        }
        theta += 10.0f;
    }
}





// Cube
void drawTorus() {

    getTorus();
    for (int i = 0; i < 36; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 18; j++) {
            glVertex3f(p[i][j].x, p[i][j].y, p[i][j].z);
        }
        glEnd();

    }
}



void
rotationPivotGLM(float theta, glm::vec3 axis, glm::vec3* pivot, glm::vec3* point)
{

    // add homogenous term
    glm::vec4 _point((*point).x, (*point).y, (*point).z, 0);


    // Identity matrix
    glm::mat4   M(1.0);


    // M = M * translate(pivot)
    M = glm::translate(M, *pivot);

    // M = M * rotate(angle, axis)
    M = glm::rotate(M, glm::radians(theta), axis);   // Radians

    // M = M * translate(-pivot)
    M = glm::translate(M, -(*pivot));

    // Multiplying Transform Matrix * point
    _point = M * _point;

    // sub homogenous term
    (*point).x = _point.x;
    (*point).y = _point.y;
    (*point).z = _point.z;

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
    case 1: drawmainCircle();  break;
    case 2: drawsubCircle();  break;
    case 3: drawTorus();  break;

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

            // Play on/off
        case GLFW_KEY_SPACE:   pause = !pause; break;

            // Example selection
        case GLFW_KEY_1: selection = 1; break;
        case GLFW_KEY_2: selection = 2; break;
        case GLFW_KEY_3: selection = 3; break;
        case GLFW_KEY_4: selection = 4; break;
        case GLFW_KEY_5: selection = 5; break;
        case GLFW_KEY_6: selection = 6; break;
        case GLFW_KEY_7: selection = 7; break;
        }
    }
}