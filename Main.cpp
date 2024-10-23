#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <Camera.h>
#include <glm/gtc/type_ptr.hpp>
#include <Transform.h>
#include <Primatives.h>
#include <thread>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#include <Types.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void cursorCallback(GLFWwindow* window,double x, double y);
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 430 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 proj;\n"
    "uniform mat4 view;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = proj * view * transform * vec4(aPos.xyz, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 430 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

const char* basicComputeShader = "\n"
    "#versiom 430 core  \n"
    "uniform \n"
    "\n"
    "\n";

const char *vertSelection ="";
const char *fragSelection ="";


#pragma region meshLoader

void bindMesh(std::map<int, GLuint>& vbos,
    tinygltf::Model& model, tinygltf::Mesh& mesh) {
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView& bufferView = model.bufferViews[i];
        if (bufferView.target == 0) {  // TODO impl drawarrays
            std::cout << "WARN: bufferView.target is zero" << std::endl;
            continue;  // Unsupported bufferView.
            /*
              From spec2.0 readme:
              https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
                       ... drawArrays function should be used with a count equal to
              the count            property of any of the accessors referenced by the
              attributes            property            (they are all equal for a given
              primitive).
            */
        }

        const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
        std::cout << "bufferview.target " << bufferView.target << std::endl;

        GLuint vbo;
        glGenBuffers(1, &vbo);
        vbos[i] = vbo;
        glBindBuffer(bufferView.target, vbo);

        std::cout << "buffer.data.size = " << buffer.data.size()
            << ", bufferview.byteOffset = " << bufferView.byteOffset
            << std::endl;

        glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        for (auto& attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            int vaa = -1;
            if (attrib.first.compare("POSITION") == 0) vaa = 0;
            if (attrib.first.compare("NORMAL") == 0) vaa = 1;
            if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
            if (vaa > -1) {
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, BUFFER_OFFSET(accessor.byteOffset));
            }
            else
                std::cout << "vaa missing: " << attrib.first << std::endl;
        }

        if (model.textures.size() > 0) {
            // fixme: Use material's baseColor
            tinygltf::Texture& tex = model.textures[0];

            if (tex.source > -1) {

                GLuint texid;
                glGenTextures(1, &texid);

                tinygltf::Image& image = model.images[tex.source];

                glBindTexture(GL_TEXTURE_2D, texid);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                GLenum format = GL_RGBA;

                if (image.component == 1) {
                    format = GL_RED;
                }
                else if (image.component == 2) {
                    format = GL_RG;
                }
                else if (image.component == 3) {
                    format = GL_RGB;
                }
                else {
                    // ???
                }

                GLenum type = GL_UNSIGNED_BYTE;
                if (image.bits == 8) {
                    // ok
                }
                else if (image.bits == 16) {
                    type = GL_UNSIGNED_SHORT;
                }
                else {
                    // ???
                }

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
                    format, type, &image.image.at(0));
            }
        }
    }
}

#pragma endregion

using std::cout;
Camera cam;
float deltaTime;
float lastFrameTime;

#include <random>
#pragma region

// Function to generate a random float in a given range
float RandomFloat(float min, float max) {
    // Use a random device to seed the generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// Function to generate a random glm::vec3
glm::vec3 RandomVec3(float minRange = -1.0f, float maxRange = 1.0f) {
    float x = RandomFloat(minRange, maxRange);
    float y = RandomFloat(minRange, maxRange);
    float z = RandomFloat(minRange, maxRange);
    return vec3(x, y, z);
}

#pragma endregion


void Test() 
{
    vector<vec3> randoms;
    
    for (int i = 0; i < 10; i++)
    {
        randoms.push_back(RandomVec3(-20, 20));
    }

    cout << "sup" << "\n";
}

int main()
{
    //tinygltf::Model model;
    void(*func)() = []() 
        {
            cout << "thing" << "\n";
        };
    std::thread thread = std::thread(func);
    thread.join();
    cam = Camera();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TempWindow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    


    
    //glfwcursor(GL_FRONT_AND_BACK, GL_LINE);
    glViewport(0,0,SCR_WIDTH, SCR_HEIGHT);
    
    cam.Position = {0.0f,0.0f,5.0f};
    glEnable(GL_DEPTH_TEST);

    mat4 transform = Transform::createModelMatrix(vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(10.0f,0.50f,10.0f));
    Cube cube;
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render
       
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4 matrix = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, .1f, 100.0f) ;
        
        glUseProgram(shaderProgram);
        GLuint loc =  glGetUniformLocation(shaderProgram,"view");
        glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(cam.GetViewMatrix()));
       
        loc =  glGetUniformLocation(shaderProgram,"proj");
        glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(matrix));

        loc =  glGetUniformLocation(shaderProgram,"transform");
        glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(transform));

        glDrawArrays(GL_TRIANGLES, 0, 3);
        cube.Render();
       
        glfwSwapBuffers(window);
        glfwPollEvents();
        deltaTime =  glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}


#pragma region CursorCallback
double LastX, LastY;
float deltaX,deltaY;
float halfWidth, halfHeight;
void cursorCallback(GLFWwindow* window,double x, double y)
{
    if((int)halfWidth != SCR_WIDTH*2  || (int)halfHeight != SCR_HEIGHT*2)
    {
        halfWidth = SCR_WIDTH/2;
        halfHeight = SCR_HEIGHT/2;
    }
    
    double centerPosX, centerPosY;
    centerPosY = (-1*y) + halfHeight;
    centerPosX = x - halfWidth;
    
    deltaX = centerPosX   / halfWidth; 
    deltaY = centerPosY / halfHeight; 
    
    cam.ProcessMouseMovement(centerPosX - LastX, centerPosY- LastY,true);
    LastX = centerPosX;
    LastY = centerPosY;
    //std::cout << "dX  " << deltaX << " dY  " << deltaY <<"\n" ;
}
#pragma endregion

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
            return;
        }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::LEFT,deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::RIGHT,deltaTime);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::FORWARD,deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::BACKWARD,deltaTime);
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::UPWARD,deltaTime);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.ProcessKeyboard(Camera_Movement::DOWNWARD,deltaTime);
    
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam.FastMove = true;
        else
        cam.FastMove = false;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    
    glViewport(0, 0, width, height);
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
    
}