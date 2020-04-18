// Hacky test

#include "model/Animat.hpp"
#include "model/AnimatWorld.hpp"
#include "simulator/Agent.hpp"
#include "simulator/Simulation.hpp"

#include "graphics/GLEnvironment.hpp"
#include "graphics/Graphics.hpp"
#include "graphics/GLButton.hpp"
#include "graphics/GLVerticalSlider.hpp"
#include "graphics/GLCircularDial.hpp"
#include "graphics/PauseOverlay.hpp"
#include "graphics/ThreadRunner.hpp"
#include "graphics/RetinaScalar.hpp"
#include "glfreetype/TextRenderer.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <unistd.h>

int windowWidth = 800;
int windowHeight = 800;
int popSize = 20;

// For running operations asynchronously
graphics::detail::ThreadRunner threadRunner;



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

    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(windowWidth, windowHeight, "Animat test!", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Size correction for small monitor
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Call retinaScalar once with window do derive correct size
    (void)graphics::detail::retinaScalar(window);

    simulator::Simulation sim(popSize);

    model::AnimatWorld & animatWorld = sim.animatWorld();
    graphics::GLEnvironment glEnvironment(windowWidth, 
                                          windowHeight, 
                                          animatWorld,
                                          threadRunner);

    // GUI agnostics GL calls
    graphix.reset(new graphics::Graphics(windowWidth, 
                                         windowHeight, 
                                         glEnvironment,
                                         threadRunner));



    // Callbacks
    glfwSetWindowSizeCallback(window, reshape);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetCursorPosCallback(window, passiveMouseFunc);
    glfwSetKeyCallback(window, keyboardHandler);
    glfwSetMouseButtonCallback(window, clickHandler);

    // Add some GUI Elements
    auto button = std::make_shared<graphics::GLButton>(window,
                                                       20, 20,
                                                       80, 50,
                                                       threadRunner);

    button->installHandler([&](bool const state) {
        if (state) {
            sim.pause();
        } else {
            sim.resume();
        }
    });

    button->setOverlay(graphics::pauseOverlay());

    auto slider = std::make_shared<graphics::GLVerticalSlider>(window,
                                                               25, // offset from right
                                                               15, // bar width
                                                               threadRunner);
    slider->installHandler([&](double const value) {

        auto comp = 1.0 - (value + 0.1);
        if(comp > 0 && comp < 1.0) {
            glEnvironment.setViewDistance(comp);
        }
    });

    // Add a dial to control the speed of the simulation
    // with a starting level set to maximum speed. Note,
    // simulation speed is proportional to processor speed.
    auto dial = std::make_shared<graphics::GLCircularDial>(window, 160, 45,
                                                           25.0, /* radius */
                                                           100, /* start level */
                                                           threadRunner);
    dial->installHandler([&](int const value) {
        if(value >= 0 && value <= 100) {
            auto actual = 100 - value;
            double ratio = static_cast<double>(actual) / 100.0;
            auto const toSet = static_cast<int>(ratio * 50000.0);
            sim.setSleepDuration(toSet);
        }
    });

    graphix->addGUIElement(std::move(button));
    graphix->addGUIElement(std::move(slider));
    graphix->addGUIElement(std::move(dial));

    float sx = windowWidth / 2.0;
    float sy = windowHeight / 2.0;
    sx *= graphics::detail::retinaScalar();
    sy *= graphics::detail::retinaScalar();
    std::cout << sx << std::endl;

    // Start the main simulation loop
    sim.start();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        display();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
