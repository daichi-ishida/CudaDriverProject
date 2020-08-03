#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include "display.h"
#include "constants.h"

void Display::window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}

void Display::createWindow()
{
    if (glfwInit() == GLFW_FALSE)
    {
        fprintf(stderr, "Initialization failed!\n");
    }

    m_window.reset(glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL));

    if (m_window == nullptr)
    {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window.get());

    glfwSetWindowSizeCallback(m_window.get(), window_size_callback);

    // load OpenGL 3.x/4.x
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) 
    {
        fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
    }

    printf("Load OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
}

void Display::updateWindow()
{
    glfwSwapBuffers(m_window.get());
    glfwPollEvents();
}


bool Display::isClosing() const
{
    return glfwWindowShouldClose(m_window.get()) || glfwGetKey(m_window.get(), GLFW_KEY_ESCAPE);
}

void Display::close() const
{
    glfwTerminate();
}