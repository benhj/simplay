// Hacky test

#include "model/Animat.hpp"
#include "simulator/AnimatWorld.hpp"
#include "simulator/Agent.hpp"

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

std::thread testThread;

int blocks = 10;
int popSize = 150;
simulator::AnimatWorld animatWorld(popSize,{blocks, 2.0, 3.8611});
graphics::GLEnvironment glEnvironment(windowWidth, 
                                      windowHeight, 
                                      animatWorld);

std::unique_ptr<graphics::Graphics> graphix;

std::vector<simulator::Agent> agents;

/// GLUT CALLBACKS. N.B. eventually this will change
/// once I've figured out a nicer graphics lib.
void display() { graphix->display(); }
void reshape(int w, int h) { graphix->reshape(w, h); }
void keyboardHandler(int key, int x, int y) { graphix->keyboardHandler(key, x, y); }
void passiveMouseFunc(int x, int y) { graphix->passiveMouseFunc(x, y); }

/// Hacky simulation loop. To be pulled into separate class.
void loop()
{
    for(int tick = 0; tick < 10000; ++tick) {
        
        for (int p = 0; p < popSize; ++p) {
            auto & agent = agents[p];

            // if physics broke, reinit position in world
            if(agent.update() == -1) {
                animatWorld.randomizePositionSingleAnimat(p, 10, 10);
            }
        }
        usleep(500);
    }
}


int main(int argc, char **argv)
{

    animatWorld.randomizePositions(10, 10);
    agents.reserve(popSize);

    for(int i = 0;i<popSize;++i){
        agents.emplace_back(animatWorld.animat(i));
    }

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

    testThread = std::thread(loop);

    // Starts the program.
    glutMainLoop();
    return 0;
}