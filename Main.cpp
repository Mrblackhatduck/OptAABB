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

#include<Tree.h>
#include <tiny_obj_loader.h>


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
        glDrawElements(GL_TRIANGLES, NumOfIndices, GL_UNSIGNED_INT, 0);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*Positions.size(), &Positions[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*Normals.size(), &Normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*TextureCoords.size(), &TextureCoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW);

        

        

       

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


Model loadmodel(string& filepath,bool triangulate = true) 
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    string errors;
    string warnings;
    uint checksections;

    int seperator = filepath.find_last_of("/") + 1;
    string path = filepath.substr(0,seperator);
    string name = filepath.substr(seperator, filepath.size() - seperator );
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, filepath.c_str(),
        NULL, triangulate);
   Model _model;
   
   
   if (warnings.size() > 0)
       std::cout << warnings << "\n";
   
   if (errors.size() > 0) {

       std::cout << errors << "\n";
       return _model;
   }
   //model.meshes[0].
   vector<vec3> _positions;
   vector<vec3> _normals;
   vector<vec2> _texcoords;
   vector<uint> _indices;
   int currentvertexindex = 0;
   int currentindex = 0;
    
   std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
   std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
   std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
       << std::endl;
   uint sizePositions = attrib.vertices.size()/3;
   uint sizeNormals = attrib.normals.size()/3;
   uint sizeTexCoords = attrib.texcoords.size()/2;
   float x, y, z;
   for (size_t i = 0; i < sizePositions; i++)
   {
       x = attrib.vertices[i * 3 + 0];
       y = attrib.vertices[i * 3 + 1];
       z = attrib.vertices[i * 3 + 2];
       _positions.push_back({ x,y,z });
   }

   for (size_t i = 0; i < sizeNormals; i++)
   {
       x = attrib.normals[i * 3 + 0];
       y = attrib.normals[i * 3 + 1];
       z = attrib.normals[i * 3 + 2];
       _normals.push_back({ x,y,z });
   }

   for (size_t i = 0; i < sizeTexCoords; i++)
   {
       x = attrib.texcoords[i * 2 + 0];
       y = attrib.texcoords[i * 2 + 1];
       _texcoords.push_back({ x,y });
   }

   for (size_t i = 0; i < shapes.size(); i++)
   {
       for (size_t j = 0; j < shapes[i].mesh.indices.size(); j++)
       {
           _indices.push_back(shapes[i].mesh.indices[j].vertex_index);
       } 

   }
   _model.InitModel(_positions, _normals, _texcoords, _indices);
   _model.NumOfIndices = _indices.size();
   return _model;
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
void TestTREE() {

    Tree tree;
    tree.InsertNode(AABB(vec3(0, 0, 0), vec3(1, 1, 1)));
    tree.InsertNode(AABB(vec3(2, 2, 2), vec3(3, 3, 3)));
    tree.InsertNode(AABB(vec3(-1, -1, -1), vec3(0, 0, 0)));

    AABB queryArea(vec3(-0.5f, -0.5f, -0.5f), vec3(2.5f, 2.5f, 2.5f));
    std::vector<int> results;
    //tree.query(queryArea, results);

    std::cout << "Objects in query area:\n";
    for (int index : results) {
        std::cout << "Object " << index << "\n";
    }


}

int main()
{
    //tinygltf::Model model;
    void(*func)() = []() 
        {
            TestTREE();
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
    rect.Transform = glm::scale(rect.Transform, { 40.0f,0.5f,40.0f });
    string modelpath = string("./Res/Models/Shop.obj");
    Model model = loadmodel(modelpath);
    model.Transform = mat4(1.0f);
    model.Transform = glm::translate(rect.Transform, { 0.0f,0.f,0.0f });
    model.Transform = glm::scale(rect.Transform, { 1.0f,1.0f,1.0f });
    model.Transform = Transform::createModelMatrix({0.0f,0.0f,-3.0f}, vec3(0.0f), 2.5f* vec3(1.0f));
    
    
    
    vector <Drawable*> drawables;
    drawables.push_back(&cube);
    drawables.push_back(&rect);
    drawables.push_back(&model);
    
    //drawables.push_back(&p2);

    Shader basicShader("./Shaders/V_Basic.glsl", "./Shaders/F_Basic.glsl");
    DrawCall Basic(&basicShader);
    
    mat4 lightMat = mat4(1.0f);
    vec3 eye = { -2.0f, 9.0f, 0.0f };
    vec3 target = { 0,-10,-10 };
    vec3 up = { 0,1,0 };
    lightMat = glm::lookAt(eye, target, up);
        //glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    mat4 lightProj = glm::ortho(-7.5f, 7.5f, -7.5f, 7.5f, .15f, 100.5f);
    
    mat4 finalLightMat = lightProj * lightMat;
   
    Shader depthShader("./Shaders/V_Depth.glsl", "./Shaders/F_Depth.glsl");
    Shader ShadowShader("./Shaders/V_Shadow.glsl", "./Shaders/F_ShadowVolume.glsl");
    Shader DebugDepthShader("./Shaders/DEBUG/V_DShowDepth.glsl", "./Shaders/DEBUG/F_DShowDepth.glsl");
    std::shared_ptr<Shader> DeferedShader = std::make_shared<Shader>("./Shaders/Defered/V_Def.glsl", "./Shaders/Defered/F_Def.glsl");
    //Shader ComputeTester("./Shaders/COMP_Test.glsl");
    
    //std::shared_ptr<Shader> ComputeVolumetric = std::make_shared<Shader>("./shaders/COMP_Volumetric.glsl");
    std::shared_ptr<Shader> ComputeVolumetric = std::make_shared<Shader>("./shaders/COMP_Volumetric.glsl");
    std::shared_ptr<Shader> ComputeGaussianBlur = std::make_shared<Shader>("./shaders/COMP_BLUR.glsl");
    Texture VolumetricTexture = Texture(SCR_WIDTH, SCR_HEIGHT, TextureType::BASIC);
    Texture BlurOutput = Texture(SCR_WIDTH, SCR_HEIGHT, TextureType::BASIC);

    //std::shared_ptr<Shader> ComputeDefered = std::make_shared<Shader>("./Shaders/COMP_Defered.glsl");
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

        //***
        ////////////////  compute shader
      
        ComputeVolumetric->use();
        glActiveTexture(GL_TEXTURE0);
        glBindImageTexture(0, VolumetricTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
       
        ComputeVolumetric->setVec3("camPosition", cam.Position);
        ComputeVolumetric->setVec3("lightPosition", eye);
        ComputeVolumetric->setMat4("projectionMat", Projection);
        ComputeVolumetric->setMat4("viewMatrix", cam.GetViewMatrix());
        ComputeVolumetric->setMat4("lightMatrix", finalLightMat);
        
       
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, deferedRenderer.Position);
        ComputeVolumetric->setInt("inPosition", 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, deferedRenderer.Normal);
        ComputeVolumetric->setInt("inNormal", 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, DepthCall.depthTexture);
        ComputeVolumetric->setInt("inDepth",3);
       
        glDispatchCompute(SCR_WIDTH/8, SCR_HEIGHT/8, 1);
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        
        ///-------------------
        //blur dispatch
        ComputeGaussianBlur->use();
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, VolumetricTexture);
        //glBindImageTexture(0, VolumetricTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        ComputeGaussianBlur->setInt("inputImage", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindImageTexture(1, BlurOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        ComputeGaussianBlur->setInt("outpuImage", 1);

        glDispatchCompute((SCR_WIDTH) / 16, (SCR_HEIGHT) / 16, 1);
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        //*-----



        debugDepthCall.shader->use();
        glActiveTexture(GL_TEXTURE0);
        //glBindImageTexture(0, BlurOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindTexture(GL_TEXTURE_2D, BlurOutput);
        debugDepthCall.shader->setInt("VolumetericInput",0);

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
