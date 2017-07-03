// Hacky test

#include "model/Animat.hpp"
#include "model/AnimatWorld.hpp"
#include "simulator/Agent.hpp"
#include "simulator/Simulation.hpp"

#include "graphics/GLEnvironment.hpp"
#include "graphics/Graphics.hpp"

#include <GLUT/glut.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
int popSize = 20;

simulator::Simulation sim(popSize);

model::AnimatWorld & animatWorld = sim.animatWorld();
graphics::GLEnvironment glEnvironment(windowWidth, 
                                      windowHeight, 
                                      animatWorld);

std::unique_ptr<graphics::Graphics> graphix;

/// GLUT CALLBACKS. N.B. eventually this will change
/// once I've figured out a nicer graphics lib.
void display() { graphix->display(); }
void reshape(GLFWwindow *, int w, int h) { graphix->reshape(w, h); }
void keyboardHandler(GLFWwindow *, int key, int scancode, int action, int mods) 
{ 
    graphix->keyboardHandler(key, scancode, action, mods); 
}
void passiveMouseFunc(GLFWwindow *, double x, double y) { graphix->passiveMouseFunc(x, y); }
void clickHandler(GLFWwindow* , int button, int action, int mods) 
{
    graphix->clickHandler(button, action, mods);
}

int main(int argc, char **argv)
{

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Animat test!", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetWindowSizeCallback(window, reshape);
    glfwSetCursorPosCallback(window, passiveMouseFunc);
    glfwSetKeyCallback(window, keyboardHandler);
    glfwSetMouseButtonCallback(window, clickHandler);

    // GUI agnostics GL calls
    graphix.reset(new graphics::Graphics(glEnvironment));

    // Size correction for small monitor
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    reshape(window, windowWidth, windowHeight);

    // Start the main simulation loop
    sim.start();

    //while(true){}

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        display();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
