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
#include <unistd.h>

int windowWidth = 512;
int windowHeight = 512;
double viewDistance = 0.015;

std::thread testThread;

int blocks = 8;
int popSize = 1;
simulator::AnimatWorld animatWorld(1,{blocks, 0.3,0.5});

void setScene()
{
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((-windowWidth/2)*viewDistance, 
            (windowWidth/2)*viewDistance, 
            (-windowHeight/2 + (windowHeight/3))*viewDistance, 
            (windowHeight/2 + (windowHeight/3))*viewDistance, 
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
    setScene();
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setScene();
    glLoadIdentity();
    for (int p = 0; p < popSize; ++p) {
        graphics::GLAnimat(animatWorld.animat(p)).draw();
    }
    glutSwapBuffers();
}

// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h)
{
    setScene();
}

void loop()
{
    
    for(int tick = 0; tick < 5; ++tick) {
        for(int integ = 0; integ < 10; ++integ) {

            for (int p = 0; p < popSize; ++p) {

                for(int b = 0;b<blocks;++b) {
                    animatWorld.animat(p).applyBlockContraction(b, 0, 20);
                    animatWorld.animat(p).applyBlockContraction(b, 1, 20);
                }
                animatWorld.animat(p).applyWaterForces();    
                animatWorld.animat(p).update();
            }
            usleep(50000);
        }
    }
}

int main(int argc, char **argv)
{

    glutInit(&argc, argv); // Initializes glut

    // Sets up a double buffer with RGBA components and a depth component
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    // Sets the window size to 512*512 square pixels
    glutInitWindowSize(512, 512);

    // Sets the window position to the upper left
    glutInitWindowPosition(0, 0);

    // Creates a window using internal glut functionality
    glutCreateWindow("Animat test!");

    // passes reshape and display functions to the OpenGL machine for callback
    //glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    init();

    testThread = std::thread(loop);

    // Starts the program.
    glutMainLoop();
    return 0;
}