#include <iostream>
using namespace std;
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
const GLint WIDTH = 800, HEIGHT = 600;

void render() {
    glClearColor(0.1f, 0.8f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f); // 红色
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glEnd();
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // 使用兼容模式
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window_1 = glfwCreateWindow(WIDTH, HEIGHT, "Hello, friend! I'm an OpenGL window!", nullptr, nullptr);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window_1, &screenWidth, &screenHeight);

    if (nullptr == window_1)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window_1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialise GLEW" << endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, screenWidth, screenHeight);

    while (!glfwWindowShouldClose(window_1))
    {
        render();
        glfwPollEvents();
        glfwSwapBuffers(window_1);
    }
    glfwTerminate();
    return 0;
}
