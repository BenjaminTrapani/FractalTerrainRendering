#ifndef SDLGRAPHICSPROGRAM
#define SDLGRAPHICSPROGRAM

 #include <SDL.h>

// The glad library helps setup OpenGL extensions.
#include <glad/glad.h>

#include <iostream>
#include <string>
#include <sstream>

#include "SimpleCamera.h"
#include "Terrain.h"
#include "Lights.h"

// This class sets up a full graphics program
class SDLGraphicsProgram{
public:

    // Constructor
    SDLGraphicsProgram(int w, int h);
    // Desctructor
    ~SDLGraphicsProgram();
    // Load .obj files into models
    void loadAssets();
    // Setup OpenGL
    bool initGL();
    // Per frame update
    void update();
    // Renders shapes to the screen
    void render();
    // loop that runs forever
    void loop();
    // Shader helpers
    unsigned int CreateShader(const std::string& vertexShaderSource,
                              const std::string& tessControlSource,
                              const std::string& tessEvalSource,
                              const std::string& fragmentShaderSource);
    unsigned int CompileShader(unsigned int type, const std::string& source);


    // Get Pointer to Window
    SDL_Window* getSDLWindow();
    // Shader loading utility programs
    void printProgramLog( GLuint program );
    void printShaderLog( GLuint shader );
    // Helper Function to Query OpenGL information
    void getOpenGLVersionInfo();

private:
    std::shared_ptr<FractalTerrain::Terrain> terrain;
    std::shared_ptr<FractalTerrain::Lights> lights;

    // Are we currently rendering a wireframe?
    bool isWireframe;

    // Camera instance
    SimpleCamera camera;

    // Should we move in the current frame and if so in what direction?
    glm::vec2 movementDir;
    bool shouldMove;
    float moveSpeed;

    // Screen dimension constants
    int screenHeight;
    int screenWidth;

    // The window we'll be rendering to
    SDL_Window* gWindow;
    // OpenGL context
    SDL_GLContext gContext;
    // Render flag
    // By default, render all of the shapes as soon as possible.
    bool gRenderShapes;
    // Graphics program
    GLuint gProgramID;
    // Vertex Array
    // Currently one allowed in this lab.
    GLuint VAOId;
    // shader
    // For now, we can have one shader.
    unsigned int shader;

    //Uniform locations
    GLint viewProjID;
    GLint terrainModelToWorldID;
    GLint cameraPosID;

    bool handleKey(SDL_Keycode keyCode);
    void handleMouseEvent(int x, int y);
    void handleMove(SDL_Keycode keyCode);
    void rotateSun(float angle, const glm::vec3& axis);
};

#endif
