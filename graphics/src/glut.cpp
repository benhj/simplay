// Hacky test

#include "Animat.hpp"
#include "AnimatWorld.hpp"
#include "Spring.hpp"
#include "GLAnimat.hpp"

// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
double viewDistance = 0.1;
std::atomic<bool> displayAxis(true);

std::thread testThread;

int blocks = 8;
int popSize = 20;
simulator::AnimatWorld animatWorld(popSize,{blocks, 3.0, 5.0});
std::vector<graphics::GLAnimat> glAnimats;

void worldToScreen(double const wx, double const wy,
                   double & sx, double & sy)
{
    double sz;
    GLdouble projection[16];
    GLdouble modelview[16];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    gluProject(wx, wy, 0, modelview, projection, viewport, &sx, &sy, &sz);
}

void setScene()
{
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((-windowWidth/2)*viewDistance, 
            (windowWidth/2)*viewDistance, 
            (-windowHeight/2)*viewDistance, 
            (windowHeight/2)*viewDistance, 
            -10000, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*
    glRotatef(angleX,1.0,0.0,0.0);
    glRotatef(angleY,0.0,1.0,0.0);
    glRotatef(angleZ,0.0,0.0,1.0);
    */
}

// This is just an example using basic glut functionality.
// If you want specific Apple functionality, look up AGL

void init() // Called before main loop to set up the program
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glAnimats.reserve(popSize);
    for (int p = 0; p < popSize; ++p) {
        glAnimats.emplace_back(animatWorld.animat(p));
    }
    setScene();
}

void drawAxis()
{
    glBegin(GL_LINES);
    glVertex3f(0, -(windowHeight/2), 0);
    glVertex3f(0, (windowHeight/2), 0);
    glVertex3f(-(windowWidth/2), 0, 0);
    glVertex3f((windowWidth/2), 0, 0);
    glEnd();
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setScene();
    glLoadIdentity();
    if(displayAxis) { drawAxis(); }
    for (int p = 0; p < popSize; ++p) {
        glAnimats[p].draw();
    }
    glutSwapBuffers();
}

// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
}

void loop()
{
    
    for(int tick = 0; tick < 5; ++tick) {
        for(int integ = 0; integ < 10; ++integ) {

            for (int p = 0; p < popSize; ++p) {

                auto & animat = animatWorld.animat(p);

                for(int b = 0;b<blocks;++b) {
                    animat.applyBlockContraction(b, 0, 20);
                    animat.applyBlockContraction(b, 1, 20);
                }
                animat.applyWaterForces();    
                animat.update();
            }
            usleep(50000);
        }
    }
}

void keyboardHandler(int key, int x, int y)
{
    // zoom control
    if (key == '+') /* '+' */ {
        viewDistance -= 0.01;
    } else if (key == '-') /* '-' */ {
        viewDistance += 0.01;
    } else if (key == 'a') {
        displayAxis = !displayAxis;
    }
}

void passiveMouseFunc(int x, int y)
{
    // Convert animat central points to world points
    // and then figure out if world mouse if within
    // range of world animat
    for (int p = 0; p < popSize; ++p) {
        auto & animat = animatWorld.animat(p);
        auto centralPoint = animat.getCentralPoint();
        auto & pos = centralPoint.first;
        auto cx = pos.m_vec[0];
        auto cy = pos.m_vec[1];
        double sx, sy;
        worldToScreen(cx, -cy, sx, sy);
        auto absDiffX = std::abs(sx - x);
        auto absDiffY = std::abs(sy - y);
        if (absDiffX < 40 && absDiffY < 40) {
            glAnimats[p].highlight();
        } else {
            glAnimats[p].dehighlight();
        }
    }
}

int main(int argc, char **argv)
{

    animatWorld.randomizePositions(20, 20);

    glutInit(&argc, argv); // Initializes glut

    // Sets up a double buffer with RGBA components and a depth component
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    // Sets the window size to 512*512 square pixels
    glutInitWindowSize(windowWidth, windowHeight);

    // Sets the window position to the upper left
    glutInitWindowPosition(0, 0);

    // Creates a window using internal glut functionality
    glutCreateWindow("Animat test!");

    // passes reshape and display functions to the OpenGL machine for callback
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutPassiveMotionFunc(passiveMouseFunc);
    glutSpecialFunc(keyboardHandler);

    init();

    testThread = std::thread(loop);

    // Starts the program.
    glutMainLoop();
    return 0;
}