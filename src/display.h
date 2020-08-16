#pragma once

#include <GLFW/glfw3.h>

#include <memory>

struct DestroyglfwWin
{
    void operator()(GLFWwindow* ptr)
    {
         glfwDestroyWindow(ptr);
    }
};

class Display
{
public:
    Display(){}
    ~Display(){}

    void createWindow();
    void updateWindow();

    bool isClosing() const;
    void close() const;

    static void window_size_callback(GLFWwindow* window, int width, int height);

private:
    std::unique_ptr<GLFWwindow, DestroyglfwWin> m_window;
};
