// Hacky test

#include "model/Animat.hpp"
#include "model/AnimatWorld.hpp"
#include "simulator/Agent.hpp"
#include "simulator/Simulation.hpp"

#include "graphics/GLEnvironment.hpp"
#include "graphics/Graphics.hpp"

#include <GLUT/glut.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
int popSize = 150;

simulator::Simulation sim(popSize);

model::AnimatWorld & animatWorld = sim.animatWorld();
graphics::GLEnvironment glEnvironment(windowWidth, 
                                      windowHeight, 
                                      animatWorld);

std::unique_ptr<graphics::Graphics> graphix;

/// GLUT CALLBACKS. N.B. eventually this will change
/// once I've figured out a nicer graphics lib.
void display() { graphix->display(); }
void reshape(int w, int h) { graphix->reshape(w, h); }
void keyboardHandler(int key, int x, int y) { graphix->keyboardHandler(key, x, y); }
void passiveMouseFunc(int x, int y) { graphix->passiveMouseFunc(x, y); }

int main(int argc, char **argv)
{

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

    // GUI agnostics GL calls
    graphix.reset(new graphics::Graphics(glEnvironment));

    // Start the main simulation loop
    sim.start();

    // Starts the program.
    glutMainLoop();
    return 0;
}