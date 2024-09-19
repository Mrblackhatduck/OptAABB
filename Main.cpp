#include <GLFW/glfw3.h>
#include <AABB.h>
#include <iostream>
#define Debug(x) std::cout<< x <<"\n"

int main()
{
    Debug("Sheesh");
    if (!glfwInit())
    {
        Debug("ERROR Iniitalizing GLFW");
        return 0;
    }
    GLFWwindow* window = glfwCreateWindow(1080,720,"Temp",NULL,NULL);
    while (!glfwWindowShouldClose(window))
    {

        /* code */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    Debug("Bungo");
    return 0;
}