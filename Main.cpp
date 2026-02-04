#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
