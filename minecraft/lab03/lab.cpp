// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <queue>
    
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include noise
#include "common/chunk.h"
#include "common/fastNoiseLite.h"

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h>
#include <common/player.h>

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void run();

#define W_WIDTH 1024
#define W_HEIGHT 768

#define SHADOW_WIDTH 1024 
#define SHADOW_HEIGHT 768 


#define TITLE "Next Minecraft"

std::mutex g_pages_mutex;

// Global variables
GLFWwindow* window;
GLuint shaderProgram, shaderProgram2d;
GLuint MVPLocation;
GLuint textureSampler, depthTextureSampler;
GLuint texture;
GLuint movingTexture;
GLuint movingTextureSampler;
GLuint displacementTexture;
GLuint displacementTextureSampler;
GLuint timeUniform;

GLuint depthFrameBuffer;
GLuint lightVPLocation, projectionMatrixLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation, lightDirectionLocation, lightPowerLocation;
GLuint depthTexture, depthMapSampler;



#define threads 2
#define TOTALCHUNKS 2000

std::mutex allowAccessChunkArrayLoad[threads];
std::mutex allowAccessChunkArrayLoaded[threads];
std::mutex allowAccessChunkArrayMeshGenerated;

std::queue<Chunk*> chunksLoadThread[threads];
std::queue<Chunk*> chunksLoadedThread[threads];

std::vector<Chunk*> chunksMeshGeneratedThread;


bool endLoadThread[threads] = { false };
bool endGenMeshThread[threads] = { false };

std::thread* loadThread[threads];
std::thread* genThread[threads];


bool meshChanged;


// locations for depthProgram
GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;


GLuint suzanneVAO;
GLuint suzanneVerticiesVBO, suzanneUVVBO;

std::vector<vec3> suzanneVertices, suzanneNormals;
std::vector<vec2> suzanneUVs;

GLuint meshVAO;
GLuint meshVerticiesVBO_low, meshVerticiesVBO_high, meshUVVBO;
//std::vector<uint_fast64_t> meshVertices;
std::vector<int> meshVertices_data_low, meshVertices_data_high;
std::vector<int> verticiesInfo;

std::vector<Chunk*> arrayloadedChunks;
std::vector<Chunk*> meshloadedChunks;



//light
GLuint depthProgram, miniMapProgram;
Light* light;
Drawable* quad;

void addLighting(Light *light);
void setLighting();

void createContext() {
    // loading shader programs
    miniMapProgram = loadShaders("SimpleTexture.vertexshader", "SimpleTexture.fragmentshader");
    shaderProgram = loadShaders("texture.vertexshader", "texture.fragmentshader");
    shaderProgram2d = loadShaders("2dObjects.vertexshader", "2dObjects.fragmentshader");

    // load textures
    texture = loadBMP("textures.bmp");

    // uniform locations 
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    timeUniform = glGetUniformLocation(shaderProgram, "time");
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");
    quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");

    LaLocation = glGetUniformLocation(shaderProgram, "light.La");
    LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
    LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
    lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
    lightDirectionLocation = glGetUniformLocation(shaderProgram, "light.lightDirection");
    lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");

    shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "lightVP");
    depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
    lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");


    // create minimap obj
    vector<vec3> quadVertices = {
      vec3(0.5, 0.5, 0.0),
      vec3(1.0, 0.5, 0.0),
      vec3(1.0, 1.0, 0.0),
      vec3(1.0, 1.0, 0.0),
      vec3(0.5, 1.0, 0.0),
      vec3(0.5, 0.5, 0.0)
    };

    vector<vec2> quadUVs = {
      vec2(0.0, 0.0),
      vec2(1.0, 0.0),
      vec2(1.0, 1.0),
      vec2(1.0, 1.0),
      vec2(0.0, 1.0),
      vec2(0.0, 0.0)
    };

    quad = new Drawable(quadVertices, quadUVs);

    // light creation
    light = new Light(
        window,
        vec4{ 2, 2, 2, 1 },
        vec4{ 1, 1, 1, 1 },
        vec4{ 1, 1, 1, 1 },
        vec3{ 0, 200, 5 },
        vec3{ 0, -1, 0 },
        1.0f
    );

    
    // openGL flags
    glShadeModel(GL_FLAT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glEnable(GL_BLEND);  
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE, or GL_FILL



    



    

    // bind the block textures in the location 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    

}

void free() {
    glDeleteBuffers(1, &suzanneVerticiesVBO);
    glDeleteBuffers(1, &suzanneUVVBO);

    
    glDeleteBuffers(1, &meshVerticiesVBO_low);
    glDeleteBuffers(1, &meshVerticiesVBO_high);

    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &movingTexture);
    glDeleteTextures(1, &displacementTexture);
    glDeleteVertexArrays(1, &suzanneVAO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(depthProgram);
    glfwTerminate();
}



vector<Chunk*> arrayLoadedMeshes;
vector<Chunk*> meshGeneratedMeshes;
vector<Chunk*> meshApliedMeshes;
vector<Chunk*> arrayGeneratedMeshes;

void mainLoop() {
    Player* Steve = new Player(-1, 200, 0, window);

    int time = 0;
    float start = (float)glfwGetTime();

    bool update_buffers = false;
    
    float FPS=30;

    run();

    glGenVertexArrays(1, &meshVAO);
    
    glGenBuffers(1, &meshVerticiesVBO_low);
    glGenBuffers(1, &meshVerticiesVBO_high);
    glGenBuffers(1, &meshUVVBO);

    glBindVertexArray(0);

    setLighting(); // #############################===============================fix not working
    do {
        // sun rotation
        mat4 sunRot = glm::rotate(mat4(1.0f), 0.001f, vec3(1.0f, 0.0f, 0.0f));
        vec4 rotatedSun = sunRot * vec4(light->direction, 1.0f);
        light->direction = vec3(rotatedSun.x, rotatedSun.y, rotatedSun.z);

        addLighting(light);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // bind the block textures in the location 0

        glUseProgram(miniMapProgram);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(quadTextureSamplerLocation, 1);


        
        //quad->bind();
        //quad->draw();



        // draw cursor here
        Steve->drawCursor(shaderProgram2d);


        float brightness = (1 - light->direction.y)/2;
        //cout << brightness << endl;

        float r = 102.0 / 255.0 * ((1- brightness)*(-2* brightness* brightness+2.5* brightness-1) +1); // glm::sqrt(glm::sqrt(brightness));
        float g = 255.0 / 255.0 * ((1 - brightness) * (-2 * brightness * brightness + 2.5 * brightness - 1) + 1);
        float b = 255.0 / 255.0 * ((1 - brightness) * (-2 * brightness * brightness + 2.5 * brightness - 1) + 1);

        glClearColor(r, g, b, 1.0f);


        //Steve->draw();

        int camera_chunk_x = (int)(Steve->position.x / BLOCK_SIZE_X / 16);
        int camera_chunk_z = (int)(Steve->position.z / BLOCK_SIZE_Y / 16);

        if (verticiesInfo.size() != 0) {
            
            // select the location 0 for current use
                // link light to the uniform locations
            glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &light->lightVP()[0][0]); // shader program

            glUseProgram(shaderProgram);
            glUniform1i(textureSampler, 0);
            glUniform4f(LaLocation, light->La.r, light->La.g, light->La.b, light->La.a);
            glUniform4f(LdLocation, light->Ld.r, light->Ld.g, light->Ld.b, light->Ld.a);
            glUniform4f(LsLocation, light->Ls.r, light->Ls.g, light->Ls.b, light->Ls.a);
            glUniform3f(lightPositionLocation, light->lightPosition_worldspace.x, light->lightPosition_worldspace.y, light->lightPosition_worldspace.z);
            glUniform3f(lightDirectionLocation, light->direction.x, light->direction.y, light->direction.z);
            glUniform1f(lightPowerLocation, light->power);

            glBindVertexArray(meshVAO);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticiesInfo.size());
            glBindVertexArray(0);

        }


        // if i have an update in the mesh then 
        if (Steve->update_buffers) {
            Steve->update_buffers = false;
            update_buffers = true;
            // change arrays

        }

        if (meshChanged) {
            update_buffers = true;
            meshChanged = false;
            allowAccessChunkArrayMeshGenerated.lock();

            arrayGeneratedMeshes.clear();
            for (Chunk* tmp : chunksMeshGeneratedThread) {
                arrayGeneratedMeshes.push_back(tmp);
                //cout << tmp->index_x << " " << tmp->index_z << endl;
            }

            cout << arrayGeneratedMeshes.size() << endl;

            allowAccessChunkArrayMeshGenerated.unlock();
            
            if (arrayGeneratedMeshes.size() == TOTALCHUNKS) {

                for (int i = 0; i < threads; i++) {
                    endLoadThread[i] = true;
                    endGenMeshThread[i] = true;
                }

            }


        }
        

        if(update_buffers){
            update_buffers = false;
            meshVertices_data_low.clear();
            meshVertices_data_high.clear();
            verticiesInfo.clear();

            Chunk::getTotalMesh(arrayGeneratedMeshes, meshVertices_data_low, meshVertices_data_high, verticiesInfo);
            Chunk::getTotalMeshWater(arrayGeneratedMeshes, meshVertices_data_low, meshVertices_data_high, verticiesInfo);
            
            if (meshVertices_data_low.size() != 0) {
            // vertex VBO
            glBindVertexArray(meshVAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, meshVerticiesVBO_low);
 
            glBufferData(GL_ARRAY_BUFFER, meshVertices_data_low.size() * sizeof(int), &meshVertices_data_low[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);

            glBindBuffer(GL_ARRAY_BUFFER, meshVerticiesVBO_high);
            glBufferData(GL_ARRAY_BUFFER, meshVertices_data_high.size() * sizeof(int), &meshVertices_data_high[0], GL_STATIC_DRAW);
            glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);


            // uvs VBO
            
            glBindBuffer(GL_ARRAY_BUFFER, meshUVVBO);
            glBufferData(GL_ARRAY_BUFFER, verticiesInfo.size() * sizeof(int), &verticiesInfo[0], GL_STATIC_DRAW);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            
            glBindVertexArray(0);
            }
        
        }


        mat4 MVP, modelMatrix, viewMatrix, projectionMatrix;

        Steve->update(1/FPS);

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            // see from the ligth point of view
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &light->lightVP()[0][0]); 
        }
        else {
            // see from player 
            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &Steve->MVP[0][0]);
        }



        glUniform1f(timeUniform, (float)glfwGetTime());
            
        //cout << "Waiting for: " << 1 / FPS - glfwGetTime() + start << endl;
        while (glfwGetTime() - start<1/FPS);
        start = (float)glfwGetTime(); 


        glfwSwapBuffers(window);

        glfwPollEvents();
        
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}

void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
                            " If you have an Intel GPU, they are not 3.3 compatible." +
                            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Task 6.1
    // Enable blending for transparency
    // change alpha in fragment shader
    //*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //*/

    // Task 6.2
    // Cull triangles which normal is not towards the camera
    /*/
    glEnable(GL_CULL_FACE);
    //*/

    // Log
    logGLParameters();

    // Create camera
    
}

int main(void) {
    try {
        initialize();
        createContext();
        mainLoop();
        free();
    } catch (exception& ex) {
        cout << ex.what() << endl;
        auto a=getchar();
        free();
        return -1;
    }
    return 0;
}



































void setLighting() {


    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


    glGenFramebuffers(1, &depthFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

    glGenTextures(1, &depthTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glUniform1i(shadowViewProjectionLocation, 1);


    // Telling opengl the required information about the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// Task 4.5 Texture wrapping methods
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER

    
    // Step 1 : (Don't forget to comment out the respective lines above
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set color to set out of border 
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    ///

    // Finally, we have to always check that our frame buffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glfwTerminate();
        throw runtime_error("Frame buffer not initialized correctly");
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    

};


void addLighting(Light* light) {
    // fixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    //*
    //== depth program == 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
    glUseProgram(depthProgram);
    


    mat4 view_projection = light->lightVP();
    glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]); // depth program

    if (meshVertices_data_low.size() != 0) {
        // vertex VBO
        glBindVertexArray(meshVAO);

        glBindBuffer(GL_ARRAY_BUFFER, meshVerticiesVBO_low);

        glBufferData(GL_ARRAY_BUFFER, meshVertices_data_low.size() * sizeof(int), &meshVertices_data_low[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);

        glBindBuffer(GL_ARRAY_BUFFER, meshVerticiesVBO_high);
        glBufferData(GL_ARRAY_BUFFER, meshVertices_data_high.size() * sizeof(int), &meshVertices_data_high[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(int), NULL);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindVertexArray(meshVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticiesInfo.size());
        glBindVertexArray(0);

    }


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthMapSampler, 1);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}










// threading ... 
//*



void genArrayChunkThread(int id) {
    //vector<Chunk*> chunksOnLoad;
    while (!endLoadThread[id]) {
        do {
            if (endLoadThread[id]) return;
        } while (chunksLoadThread[id].size() == 0);
        
        // crit section
        allowAccessChunkArrayLoad[id].lock();
        
        Chunk* tmp = chunksLoadThread[id].front();
        //chunksOnLoad.push_back(tmp);
        chunksLoadThread[id].pop();

        allowAccessChunkArrayLoad[id].unlock();

        tmp->genArray();
        //cout << "Thread " << id << " - loaded chunk at: " << tmp->index_x << " " << tmp->index_z << endl;
        
        allowAccessChunkArrayLoaded[id].lock();
        
        chunksLoadedThread[id].push(tmp);
        //chunksOnLoad.pop_back();
        
        allowAccessChunkArrayLoaded[id].unlock();
        /*
        //*/
    }

};

//*/

void genMeshChunkThread(int id) {
    

    //vector<Chunk*> chunksOnMeshGen;
    while (!endGenMeshThread[id]) {
        do {
            if (endGenMeshThread[id]) return;
        }
        while (chunksLoadedThread[id].size() == 0);

        
        // crit section
        allowAccessChunkArrayLoaded[id].lock();

        Chunk* tmp = chunksLoadedThread[id].front();
        //chunksOnMeshGen.push_back(tmp);
        chunksLoadedThread[id].pop();
        
        allowAccessChunkArrayLoaded[id].unlock();


        
        tmp->genMesh();
        //cout << "Thread " << id << " - Gen Mesh chunk at: " << tmp->index_x << " " << tmp->index_z << endl;
        
        
        allowAccessChunkArrayMeshGenerated.lock();

        chunksMeshGeneratedThread.push_back(tmp);    
        meshChanged = true;
        //cout << tmp->index_x << " " << tmp->index_z << endl;
        //chunksOnMeshGen.pop_back();
        
        allowAccessChunkArrayMeshGenerated.unlock();

    };
};


//vector<Chunk*> loadedChunksThreads;

void run() {

auto start = glfwGetTime();


for (int i = 0; i < TOTALCHUNKS; i++) {
    int dx, dz;
    dims(i, &dx, &dz);

    Chunk* tmp = new Chunk(dx, dz);
    // loadedChunksThreads.push_back(tmp);
    //cout << "Thread "<< i % threads <<" - loaded chunk at: " << tmp->index_x << " " << tmp->index_z << endl;


    allowAccessChunkArrayLoad[i % threads].lock();
    chunksLoadThread[i % threads].push(tmp);
    allowAccessChunkArrayLoad[i % threads].unlock();
}

    for (int i = 0; i < threads; i++) {
        loadThread[i] = new std::thread(genArrayChunkThread, i);
    }


    
    /*
    for (int i = 0; i < threads; i++) {
      cout << chunksLoadedThread[i].size() << endl;
    }
    //*/

    for (int i = 0; i < threads; i++) {
        genThread[i] = new std::thread(genMeshChunkThread, i);
    }


    while (chunksMeshGeneratedThread.size() < TOTALCHUNKS);


cout << "Elapce time: " << glfwGetTime() - start << endl;

/*
for (int i = 0; i < threads; i++) {
    endLoadThread[i] = true;
    endGenMeshThread[i] = true;

}
/*
for (int i = 0; i < threads; i++) {
    loadThread[i]->join();
        genThread[i]->join();
}

//*/

};


// fix the access 