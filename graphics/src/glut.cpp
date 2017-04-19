// Hacky test

#include "Animat.hpp"
#include "AnimatWorld.hpp"
#include "Spring.hpp"
#include "GLAnimat.hpp"
#include "GLCompass.hpp"
#include "GLEnvironment.hpp"

// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <iostream>
#include <thread>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
double viewDistance = 0.1;

double angleZ = 0;

std::thread testThread;

int blocks = 8;
int popSize = 20;
simulator::AnimatWorld animatWorld(popSize,{blocks, 3.0, 5.0});
graphics::GLEnvironment glEnvironment(windowWidth, 
                                      windowHeight, 
                                      viewDistance, 
                                      angleZ,
                                      animatWorld);

// This is just an example using basic glut functionality.
// If you want specific Apple functionality, look up AGL
void init() // Called before main loop to set up the program
{
    glClearColor(209.0 / 255.0, 
                 220.0 / 255.0, 
                 235.0 / 255.0, 0.0);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnvironment.draw();
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
    } 
    // centre axis on/off
    else if (key == 'a') {
        glEnvironment.toggleAxisDisplay();
    } 
    // world rotation control
    else if (key == GLUT_KEY_RIGHT) {
        auto computed = angleZ;
        computed -= 5;
        angleZ = computed;
    } else if (key == GLUT_KEY_LEFT) {
        auto computed = angleZ;
        computed += 5;
        angleZ = computed;
    }
}

void passiveMouseFunc(int x, int y)
{
    glEnvironment.checkForAnimatHighlight(x, y);
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