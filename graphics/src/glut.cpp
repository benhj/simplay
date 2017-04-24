// Hacky test

#include "Animat.hpp"
#include "AnimatWorld.hpp"
#include "Spring.hpp"
#include "GLAnimat.hpp"
#include "GLCompass.hpp"
#include "GLEnvironment.hpp"
#include "HardcodedCPGController.hpp"
#include "CTRNNController.hpp"
#include "Agent.hpp"
#include "neat/Network.hpp"

// The OpenGL libraries, make sure to include the GLUT and OpenGL frameworks
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
std::atomic<double> viewDistance{0.2};

std::atomic<double> angleZ{0};

std::thread testThread;

int blocks = 10;
int popSize = 80;
simulator::AnimatWorld animatWorld(popSize,{blocks, 2.0, 3.8611});
graphics::GLEnvironment glEnvironment(windowWidth, 
                                      windowHeight, 
                                      viewDistance, 
                                      angleZ,
                                      animatWorld);

// Just for testing, this controller generates random
// motor output. Eventually, it will be replaced with
// evolved CTRNN controllers.
//std::vector<simulator::HardcodedCPGController> controllers;
std::vector<simulator::CTRNNController> controllers;
std::vector<neat::Network> neats;

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
    for(int tick = 0; tick < 10000; ++tick) {
        
        for (int p = 0; p < popSize; ++p) {
            auto & animat = animatWorld.animat(p);
            simulator::Agent(animat, controllers[p]).actuate();
        }
        usleep(5000);
    }
}

void keyboardHandler(int key, int x, int y)
{
    // zoom control
    if (key == '+') /* '+' */ {
        auto loaded = viewDistance.load();
        loaded -= 0.01;
        viewDistance = loaded;
    } else if (key == '-') /* '-' */ {
        auto loaded = viewDistance.load();
        loaded += 0.01;
        viewDistance = loaded;
    } 
    // centre axis on/off
    else if (key == 'a') {
        glEnvironment.toggleAxisDisplay();
    } 
    // world rotation control
    else if (key == GLUT_KEY_RIGHT) {
        glEnvironment.spinRight();    
    } else if (key == GLUT_KEY_LEFT) {
        glEnvironment.spinLeft(); 
    }
}

void passiveMouseFunc(int x, int y)
{
    glEnvironment.checkForAnimatHighlight(x, y);
}

int main(int argc, char **argv)
{

    animatWorld.randomizePositions(20, 20);
    neats.reserve(popSize);

    for(int i = 0;i<popSize;++i){
        neats.emplace_back(4, 1, 20, 0.2, 0.2, 0.2, 6.0);
        //controllers.push_back(simulator::HardcodedCPGController(4,66, blocks));
        controllers.push_back(simulator::CTRNNController(blocks, neats[i]));
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

    init();

    testThread = std::thread(loop);

    // Starts the program.
    glutMainLoop();
    return 0;
}