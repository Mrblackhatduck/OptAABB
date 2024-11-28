#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <Camera.h>
#include <glm/gtc/type_ptr.hpp>
#include <Transform.h>
#include <Primatives.h>
#include <Shader.h>
#include <thread>
#include <Texture.h>

//#include <bTree.h>

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

class Renderer {
    virtual void Render(vector<Drawable*>& Drawables) = 0;
};
class DeferedRenderer:public Renderer {
public:
    uint GBuffer;
    Texture Position;
    Texture Normal;
    Texture Albedo;
    Texture Depth;
    //uint gPos, gNormals, gColorSpec, gDepth;
    std::shared_ptr<Shader> firstPassShader;
    std::shared_ptr<Shader> finalPassShader;
    DeferedRenderer(const uint ScreenWidth, const uint ScreenHeight) :
        Position(ScreenWidth,ScreenHeight,TextureType::FRAMEBUFFER_ATTACHMENT),
        Normal(ScreenWidth,ScreenHeight, TextureType::FRAMEBUFFER_ATTACHMENT),
        Albedo(ScreenWidth,ScreenHeight, TextureType::FRAMEBUFFER_ATTACHMENT_ALBEDOSPEC),
        Depth(ScreenWidth, ScreenHeight, TextureType::DEPTH)
    {
        glGenFramebuffers(1,&GBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);

        glBindTexture(GL_TEXTURE_2D, Position);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Position,0);
        
        glBindTexture(GL_TEXTURE_2D, Normal);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Normal, 0);
        
        glBindTexture(GL_TEXTURE_2D, Albedo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Albedo, 0);
        
        glBindTexture(GL_TEXTURE_2D, Depth);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Depth, 0);


        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }
    void Render(vector<Drawable*>& Drawables) 
    override
    {
        
        firstPassShader->use();
        for (auto item : Drawables) 
        {
            firstPassShader->setMat4("model", item->Transform);
            item->Draw(firstPassShader.get());
        }
        
    }

};

uint timeStart, timeEnd;    
uint timerID[2];
inline void InitTimer() 
{
        glGenQueries(2, timerID);
        glQueryCounter(timerID[0], GL_TIMESTAMP); 
}
inline void StartTimer() 
{
    glQueryCounter(timerID[0], GL_TIMESTAMP);
}
inline float EndTimer() 
{
    glQueryCounter(timerID[1], GL_TIMESTAMP);
    int stopTimerAvailable = 0;
    while (!stopTimerAvailable) {
        glGetQueryObjectiv(timerID[1],GL_QUERY_RESULT_AVAILABLE,&stopTimerAvailable);
    }
    glGetQueryObjectuiv(timerID[0], GL_QUERY_RESULT, &timeStart);
    glGetQueryObjectuiv(timerID[1], GL_QUERY_RESULT, &timeEnd);

    //printf("Time spent on the GPU: %f ms\n", (timeStart - timeEnd) / 1000000.0);
    return (timeStart - timeEnd) / 1000000.0;
}
int main()
{
    //tinygltf::Model model;
    void(*func)() = []() 
        {

        };
            //Test();
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    
    glViewport(0,0,SCR_WIDTH, SCR_HEIGHT);
    
    cam.Position = {0.0f,0.0f,5.0f};
    glEnable(GL_DEPTH_TEST);

    mat4 transform = Transform::createModelMatrix(vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(1.0f,1.00f,1.0f));
    Cube cube;
    cube.Transform = transform;
    cube.Transform = glm::translate(cube.Transform, { 0.0f,2.0f,-3.0f });
    
    Cube rect;
    rect.Transform = transform;//Transform::createModelMatrix(vec3( 1.0f,1.0f,0.0f ), vec3( 0.0f,0.0f,0.0f ), vec3( 3.0f,0.5f,3.0f ));
    rect.Transform = glm::translate(rect.Transform, { 0.0f,-0.5f,-3.0f });
    rect.Transform = glm::scale(rect.Transform, { 7.0f,0.5f,7.0f });
    
    
    vector <Drawable*> drawables;
    drawables.push_back(&cube);
    drawables.push_back(&rect);
    
    //drawables.push_back(&p2);

    Shader basicShader("./Shaders/V_Basic.glsl", "./Shaders/F_Basic.glsl");
    DrawCall Basic(&basicShader);
    
    mat4 lightMat = mat4(1.0f);
    vec3 eye = { -2.0f, 10.0f, -1.0f };
    vec3 target = { 0,3,0 };
    vec3 up = { 0,1,0 };
    lightMat = glm::lookAt(eye, target, up);
    mat4 lightProj = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 700.0f);
    
    mat4 finalLightMat = lightProj * lightMat;
   
    Shader depthShader("./Shaders/V_Depth.glsl", "./Shaders/F_Depth.glsl");
    Shader ShadowShader("./Shaders/V_Shadow.glsl", "./Shaders/F_ShadowVolume.glsl");
    Shader DebugDepthShader("./Shaders/DEBUG/V_DShowDepth.glsl", "./Shaders/DEBUG/F_DShowDepth.glsl");
    std::shared_ptr<Shader> DeferedShader = std::make_shared<Shader>("./Shaders/Defered/V_Def.glsl", "./Shaders/Defered/F_Def.glsl");
    Shader ComputeTester("./Shaders/COMP_Test.glsl");
    std::shared_ptr<Shader> ComputeDefered = std::make_shared<Shader>("./Shaders/COMP_Defered.glsl");
    DrawCallDepth DepthCall(
        1024,1024,
        &finalLightMat,
        &ShadowShader,
        &depthShader,
        eye);
    DepthCall.screenHeight = SCR_HEIGHT;
    DepthCall.screenWidth = SCR_WIDTH;


    Texture compute_tex = Texture(SCR_WIDTH,SCR_HEIGHT,TextureType::BASIC);
    Quad screenQuad;
    DebugDepthCall debugDepthCall(&DebugDepthShader,DepthCall.depthTexture);
    //DebugDepthCall debugDepthCall(&DebugDepthShader,compute_tex);
    vector<Drawable*> screenMesh = { &screenQuad };
    

    Texture sample_image = Texture(string("./Skul.PNG"), TextureType::BASIC);
    
    float timer;
    float interval = 3;
    DeferedRenderer deferedRenderer = DeferedRenderer(SCR_WIDTH,SCR_HEIGHT);
    deferedRenderer.firstPassShader = DeferedShader;
    InitTimer();

    Texture volumetrics = Texture(SCR_WIDTH, SCR_HEIGHT, TextureType::BASIC);
    
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render
       
        
        
        mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, .1f, 100.0f) ;
        
        mat4 viewProj = Projection * cam.GetViewMatrix();
        Basic.shader->use();
        Basic.shader->setMat4("view", cam.GetViewMatrix());
        Basic.shader->setMat4("proj", Projection);
        
        DepthCall.shader->use();
        DepthCall.shader->setMat4("view", cam.GetViewMatrix());
        DepthCall.shader->setMat4("proj", Projection);
        DepthCall.shader->setVec3("lightPos", eye);
        DepthCall.shader->setVec3("viewPos", cam.Position);
        DepthCall.shader->setMat4("lightViewMatInv", glm::inverse(lightMat));
        
        DeferedShader->use();
        DeferedShader->setMat4("view",cam.GetViewMatrix());
        DeferedShader->setMat4("proj",Projection);
        DeferedShader->setVec4("BaseColor", {10.0f,10.0f,20.0f,1.0f});
        glBindFramebuffer(GL_FRAMEBUFFER, deferedRenderer.GBuffer);
        glClearColor(0.1f, 0.1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        deferedRenderer.Render(drawables);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        DepthCall.Draw(drawables);
        ////////////////  compute shader
       //glBindTexture(GL_TEXTURE_2D, volumetrics);
        debugDepthCall.shader->use();
       glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, volumetrics);
       debugDepthCall.shader->setInt("result", 0);
       
       glActiveTexture(GL_TEXTURE1);
       glBindTexture(GL_TEXTURE_2D, deferedRenderer.Albedo);
       debugDepthCall.shader->setInt("albedo", 1);

       glActiveTexture(GL_TEXTURE2);
       glBindTexture(GL_TEXTURE_2D, deferedRenderer.Position);
       debugDepthCall.shader->setInt("position", 2);

       glActiveTexture(GL_TEXTURE3);
       glBindTexture(GL_TEXTURE_2D, DepthCall.depthTexture);
       debugDepthCall.shader->setInt("shadowMap", 3);

       glActiveTexture(GL_TEXTURE4);
       glBindTexture(GL_TEXTURE_2D, deferedRenderer.Normal);
       debugDepthCall.shader->setInt("normal", 4);


       debugDepthCall.shader->setMat4("LightMatrix", finalLightMat);

       debugDepthCall.shader->setMat4("projectionMatrix",Projection);
       debugDepthCall.shader->setMat4("viewMatrix",cam.GetViewMatrix());
       debugDepthCall.shader->setVec3("camPos",cam.Position);
       //-----------glDispatchCompute(SCR_WIDTH, SCR_HEIGHT, 1);
       //-----------glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //Basic.Draw(drawables);
        
        
        glClearColor(0.1f, 0.1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        //debugDepthCall->use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, volumetrics);
        //debugDepthCall.shader->setInt("DepthImage", 0);

        debugDepthCall.Draw(screenMesh);
        //printf("%f millisocends \n", EndTimer());
     ///-----------------
        
        //lightMat =  glm::rotate(lightMat, .0001f, { 0.0f,1.0f,0.0f });
        //finalLightMat = lightProj * lightMat;

        glfwSwapBuffers(window);
        glfwPollEvents();

        deltaTime =  glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();
    }
    //glDeleteProgram(shaderProgram);
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
