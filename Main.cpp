#include <GLFW/glfw3.h>
//#include <AABB.h>
#include <iostream>
#define Debug(x) std::cout<< x <<"\n"
#include <GPTThing.h>
int main()
{
    std::vector<Object> objects = {Object(),Object(),Object(),Object(),Object()};
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i].name = "object: "+std::to_string(i);
    }
    
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