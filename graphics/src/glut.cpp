// Hacky test

#include "Animat.hpp"
#include "Spring.hpp"

// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <iostream>
#include <thread>
#include <unistd.h>

std::thread testThread;

int blocks = 8;
model::Animat animat(blocks+1,0.3,0.5);

// This is just an example using basic glut functionality.
// If you want specific Apple functionality, look up AGL

void init() // Called before main loop to set up the program
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

// Called at the start of the program, after a glutPostRedisplay() and during idle
// to display a frame
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    auto physicsEngine = animat.getPhysicsEngine();

    for(int b = 0; b < blocks; ++b) {
        auto block = animat.getBlock(b);
        auto layer1 = block.getLayerOne();
        auto layer2 = block.getLayerTwo();
        auto layer1Left = layer1.getPositionLeft(physicsEngine);
        auto layer1Right = layer1.getPositionRight(physicsEngine);
        auto layer2Left = layer2.getPositionLeft(physicsEngine);
        auto layer2Right = layer2.getPositionRight(physicsEngine);

        glBegin(GL_LINES);
            glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
            glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
            glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], -10.0);
            glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], -10.0);

            glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
            glVertex3f(layer2Left.m_vec[0], layer2Left.m_vec[1], -10.0);
            glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
            glVertex3f(layer2Right.m_vec[0], layer2Right.m_vec[1], -10.0);
        glEnd();
    }

    // Draw antennae
    auto leftAnt = animat.getLeftAntennaePoint();
    auto rightAnt = animat.getRightAntennaePoint();
    auto layer1 = animat.getBlock(0).getLayerOne();
    auto layer1Left = layer1.getPositionLeft(physicsEngine);
    auto layer1Right = layer1.getPositionRight(physicsEngine);
    glBegin(GL_LINES);
        glVertex3f(layer1Left.m_vec[0], layer1Left.m_vec[1], -10.0);
        glVertex3f(leftAnt.m_vec[0], leftAnt.m_vec[1], -10.0);
        glVertex3f(layer1Right.m_vec[0], layer1Right.m_vec[1], -10.0);
        glVertex3f(rightAnt.m_vec[0], rightAnt.m_vec[1], -10.0);
    glEnd();

    glutSwapBuffers();
}

// Called every time a window is resized to resize the projection matrix
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.2, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void loop()
{
    
    for(int tick = 0; tick < 5; ++tick) {
        for(int integ = 0; integ < 10; ++integ) {
            for(int b = 0;b<blocks;++b) {
                animat.applyBlockContraction(b, 0, 20);
                animat.applyBlockContraction(b, 1, 20);
            }
            animat.applyWaterForces();    
            animat.update();
            usleep(500000);
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
    glutCreateWindow("Hello!");

    // passes reshape and display functions to the OpenGL machine for callback
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    init();

    testThread = std::thread(loop);

    // Starts the program.
    glutMainLoop();
    return 0;
}