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

class Mesh {
    #define ALBEDO_INDX_1 0
    #define ALBEDO_INDX_2 1
    #define NRML_INDX 2
    #define SPEC_INDX 3
public:
    int BaseVertex;
    int VerteciesCount;

    int BaseIndex;
    int IndeciesCount;

    uint Textures[4];
};
#pragma region meshLoader



class Model :public Drawable
{
    uint ID, EBO;
public:
    void Draw(Shader* shader)
        override
    {
        shader->setMat4("model", this->Transform);
        glBindVertexArray(ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, meshes[0].IndeciesCount, GL_UNSIGNED_INT, 0);
       /* for(const auto& mesh:meshes)
            glDrawElementsBaseVertex(GL_TRIANGLES,mesh.BaseIndex,GL_UNSIGNED_INT,0,mesh.IndeciesCount);*/
        //glDrawElementsBaseVertex(GL_TRIANGLES, NumOfIndices, GL_UNSIGNED_INT, 0);

    }

    vector<Mesh> meshes;
    std::unordered_map<string,uint> textures;
    uint NumOfIndices;
    Model():
        NumOfIndices(0),
        ID(0),
        EBO(0),
        meshes(vector<Mesh>())
        
    {
        this->Transform = (mat4(1.0f));
    }
    Model(vector<vec3>& Positions, vector<vec3>& Normals, vector<vec2>& TextureCoords, vector<uint>& indices) :
        NumOfIndices(indices.size()),
        meshes(vector<Mesh>())
    {
        uint vboPos, vboNorm, vboTexCoord;
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        glGenBuffers(1, &vboPos);
        glGenBuffers(1, &vboNorm);
        glGenBuffers(1, &vboTexCoord);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, vboPos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), &Normals[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TextureCoords), &TextureCoords[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
    }
    void InitModel(vector<vec3>& Positions, vector<vec3>& Normals, vector<vec2>& TextureCoords, vector<uint>& indices) 
    {
        uint vboPos, vboNorm, vboTexCoord;
        glGenVertexArrays(1, &ID);
        glBindVertexArray(ID);
        glGenBuffers(1, &vboPos);
        glGenBuffers(1, &vboNorm);
        glGenBuffers(1, &vboTexCoord);
        glGenBuffers(1, &EBO);

        glBindBuffer(GL_ARRAY_BUFFER, vboPos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), &Normals[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TextureCoords), &TextureCoords[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
    }
   /* ~Model() 
    {
        glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER,&EBO);
        glDeleteVertexArrays(1, &ID);
    }*/
    void Delete() 
    {
        glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &EBO);
        glDeleteVertexArrays(1, &ID);
    }
};

//
//Model LoadModel(string& filePath) 
//{
//    tinygltf::Model model;
//    string errors;
//    string warnings;
//    uint checkSections;
//   Model _model;
//   bool ret = loader.LoadASCIIFromFile(&model,&errors,&warnings,filePath);
//   
//   if (warnings.size() > 0)
//       std::cout << warnings << "\n";
//   
//   if (errors.size() > 0) {
//
//       std::cout << errors << "\n";
//       return _model;
//   }
//   //model.meshes[0].
//   vector<vec3> _positions;
//   vector<vec3> _normals;
//   vector<vec2> _texCoords;
//   vector<uint> _indices;
//   int currentVertexIndex = 0;
//   int currentIndex = 0;
//   for(tinygltf::Mesh& msh : model.meshes ) 
//   {
//       Mesh mesh;
//       mesh.BaseVertex = currentVertexIndex;
//       mesh.BaseIndex = currentIndex;
//       
//       for (int i = 0; i < msh.primitives.size(); i++) 
//       {
//           const auto& positions = msh.primitives[i].attributes["POSITION"];
//           const auto& normals = msh.primitives[i].attributes["NORMAL"];
//           const auto& textureCoords = msh.primitives[i].attributes["TEXCOORD_0"];
//           const auto& indices = msh.primitives[i].indices;
//
//
//           // read positions
//           const auto& accessor = model.accessors[positions];
//           const auto& bufferView = model.bufferViews[accessor.bufferView];
//           const auto& buffer = model.buffers[bufferView.buffer];
//
//           
//           const float* vertexData = reinterpret_cast<const float*>(buffer.data.data() + accessor.byteOffset);
//
//           // Access individual vertex positions
//           for (size_t i = 0; i < accessor.count; ++i) {
//               float x = vertexData[i * 3 + 0];
//               float y = vertexData[i * 3 + 1];
//               float z = vertexData[i * 3 + 2];
//               // ... use x, y, z
//               _positions.push_back({ x,y,z });
//           }
//           currentVertexIndex += accessor.count;
//
//           const auto& accessor_normal = model.accessors[normals];
//           const auto& bufferView_normals = model.bufferViews[accessor_normal.bufferView];
//           const auto& buffer_normals = model.buffers[bufferView_normals.buffer];
//
//           const float* vertexNormals = reinterpret_cast<const float*>(buffer_normals.data.data() + accessor_normal.byteOffset);
//           for (size_t i = 0; i < accessor.count; i++) 
//           {
//               float x = vertexNormals[i * 3 + 0];
//               float y = vertexNormals[i * 3 + 1];
//               float z = vertexNormals[i * 3 + 2];
//               _normals.push_back({ x,y,z });
//           }
//
//           const auto& accessor_texCord = model.accessors[textureCoords];
//           const auto& bufferViewTexCoor = model.bufferViews[accessor_texCord.bufferView];
//           const auto& bufferTexCoord = model.buffers[bufferViewTexCoor.buffer];
//
//           const float* TexCoords = reinterpret_cast<const float*>(bufferTexCoord.data.data() + accessor_texCord.byteOffset);
//       
//           for (size_t i = 0; i < accessor_texCord.count; i++)
//           {
//               float x = TexCoords[i * 2 + 0];
//               float y = TexCoords[i * 2 + 1];
//               _texCoords.push_back({ x,y });
//           }
//
//           const auto& indicesAccessor = model.accessors[indices];
//           const auto& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
//           const auto& indicesBuffer = model.buffers[indicesBufferView.buffer];
//
//           const uint16_t* indicesData = reinterpret_cast<const uint16_t*>(indicesBuffer.data.data() + indicesAccessor.byteOffset);
//
//           for (size_t i = 0; i < indicesAccessor.count; i++) 
//           {
//               _indices.push_back(indicesData[i]);
//           }
//           currentIndex += indicesAccessor.count;
//           _model.NumOfIndices += indicesAccessor.count;
//       }
//       mesh.VerteciesCount = currentVertexIndex - mesh.BaseVertex;
//       mesh.IndeciesCount = currentIndex - mesh.BaseIndex;
//       _model.meshes.push_back(mesh);
//   }
//   _model.InitModel(_positions, _normals, _texCoords, _indices);
//   return _model;
//}


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

    //mat4 transform = Transform::createModelMatrix(vec3(0.0f,0.0f,0.0f),vec3(0.0f,0.0f,0.0f),vec3(1.0f,1.00f,1.0f));
    Cube cube;
    cube.Transform = mat4(1.0f);
    cube.Transform = glm::translate(cube.Transform, { 0.0f,2.0f,-3.0f });
    
    Cube rect;
    rect.Transform = mat4(1.0f);
    rect.Transform = glm::translate(rect.Transform, { 0.0f,-0.5f,-3.0f });
    rect.Transform = glm::scale(rect.Transform, { 7.0f,0.5f,7.0f });
    string modelpath = string("./Res/Models/Cube.gltf");
   /* Model model = ProcessMeshes(modelpath);
    model.Transform = mat4(1.0f);
    model.Transform = glm::translate(rect.Transform, { 0.0f,3.5f,3.0f });
    model.Transform = glm::scale(rect.Transform, { 1.0f,0.5f,1.0f });*/
    
    //rect.Transform = transform;//Transform::createModelMatrix(vec3( 1.0f,1.0f,0.0f ), vec3( 0.0f,0.0f,0.0f ), vec3( 3.0f,0.5f,3.0f ));
    //rect.Transform = glm::translate(rect.Transform, { 0.0f,-0.5f,-3.0f });
    //rect.Transform = glm::scale(rect.Transform, { 7.0f,0.5f,7.0f });
    
    
    vector <Drawable*> drawables;
    drawables.push_back(&cube);
    drawables.push_back(&rect);
    //drawables.push_back(&model);
    
    //drawables.push_back(&p2);

    Shader basicShader("./Shaders/V_Basic.glsl", "./Shaders/F_Basic.glsl");
    DrawCall Basic(&basicShader);
    
    mat4 lightMat = mat4(1.0f);
    vec3 eye = { -2.0f, 30.0f, -5.0f };
    vec3 target = { 0,3,0 };
    vec3 up = { 0,1,0 };
    lightMat = glm::lookAt(eye, target, up);
        //glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.5f, 30.5f);
    
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
       glBindTexture(GL_TEXTURE_2D, deferedRenderer.Normal);
       debugDepthCall.shader->setInt("normal", 3);
       
       glActiveTexture(GL_TEXTURE4);
       glBindTexture(GL_TEXTURE_2D, DepthCall.depthTexture);
       debugDepthCall.shader->setInt("shadowMap", 4);

      


       debugDepthCall.shader->setMat4("LightMatrix", finalLightMat);

       debugDepthCall.shader->setMat4("projectionMatrix",Projection);
       debugDepthCall.shader->setMat4("viewMatrix",cam.GetViewMatrix());
       debugDepthCall.shader->setVec3("camPos",cam.Position);
       debugDepthCall.shader->setVec3("LightForward",target-eye);
       debugDepthCall.shader->setVec3("LightPos",eye);

       //-----------glDispatchCompute(SCR_WIDTH, SCR_HEIGHT, 1);
       //-----------glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //Basic.Draw(drawables);
       glCullFace(GL_BACK);
        
        glClearColor(0.1f, 0.1f, .1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        
        //debugDepthCall->use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, volumetrics);
        //debugDepthCall.shader->setInt("DepthImage", 0);

        debugDepthCall.Draw(screenMesh);
        //printf("%f millisocends \n", EndTimer());
     ///-----------------
        
        cube.Transform =  glm::rotate(cube.Transform, .0005f, { 0.0f,1.0f,0.0f });
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
