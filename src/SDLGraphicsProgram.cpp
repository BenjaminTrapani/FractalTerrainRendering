#include "SDLGraphicsProgram.h"
#include "Utilities.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <string_view>
// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h) : screenWidth(w), screenHeight(h),
                                                       isWireframe(false),
                                                       camera(45, (float)w / (float)h),
                                                       shouldMove(false),
                                                       moveSpeed(0.1f) {
    // Place camera a little in front of cube
    camera.updateTranslation(glm::vec2(0.0f, -1.0f));

    // Initialization flag
    bool success = true;
    // String to hold any errors that occur.
    std::stringstream errorStream;
    // The window we'll be rendering to
    gWindow = NULL;
    // Render flag
    // By default, render all of the shapes as soon as possible.
    gRenderShapes = true;

    // Graphics program
    gProgramID = 0;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        success = false;
    } else {
        //Use OpenGL 4.1 core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        // We want to request a double buffer for smooth updating.
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        //Create window
        gWindow = SDL_CreateWindow("Fractal Terrain",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   screenWidth,
                                   screenHeight,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        // Check if Window did not create.
        if (gWindow == NULL) {
            errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }

        //Create an OpenGL Graphics Context
        gContext = SDL_GL_CreateContext(gWindow);
        if (gContext == NULL) {
            errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }

        // Initialize GLAD Library
        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            errorStream << "Failed to iniitalize GLAD\n";
            success = false;
        }

        //Initialize OpenGL
        if (!initGL()) {
            errorStream << "Unable to initialize OpenGL!\n";
            success = false;
        }

        loadAssets();

        // Set initial mouse location to be screen center
        SDL_WarpMouseInWindow(getSDLWindow(), screenWidth / 2, screenHeight / 2);
    }

    // If initialization did not work, then print out a list of errors in the constructor.
    if (!success) {
        errorStream << "Failed to initialize!\n";
        std::string errors = errorStream.str();
        std::cout << errors << "\n";
    } else {
        std::cout << "No SDL, GLAD, or OpenGL, errors Detected\n\n";
    }

}

// Proper shutdown and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram() {
    //Deallocate program
    glDeleteProgram(gProgramID);
    //Destroy window
    SDL_DestroyWindow(gWindow);
    // Point gWindow to NULL to ensure it points to nothing.
    gWindow = NULL;
    glDeleteVertexArrays(1, &VAOId);
    // Delete our shader program
    glDeleteProgram(shader);

    //Quit SDL subsystems
    SDL_Quit();
}

void SDLGraphicsProgram::loadAssets() {
    terrain = std::make_shared<FractalTerrain::Terrain>(16, 16, 19, 56.0f, shader);
    lights = std::make_shared<FractalTerrain::Lights>(shader);
    const glm::vec3 ones(1.0, 1.0, 1.0);
    lights->addLight(glm::vec3(0.0, 1.0, 0.0), ones, ones, ones);
}

// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::initGL() {
    // Setup shaders
    const std::string vertexShader = FractalTerrain::Utilities::slurpFile("vertex.glsl");
    const std::string tessControlShader = FractalTerrain::Utilities::slurpFile("TessControl.glsl");
    const std::string tessEvalShader = FractalTerrain::Utilities::slurpFile("TessEval.glsl");
    const std::string fragmentShader = FractalTerrain::Utilities::slurpFile("fragment.glsl");

    // VertexArrays
    glGenVertexArrays(1, &VAOId);
    glBindVertexArray(VAOId);

    glPatchParameteri(GL_PATCH_VERTICES, 3);
    shader = CreateShader(vertexShader, tessControlShader, tessEvalShader, fragmentShader);

    // Get viewProj uniform id and if we fail to find it, return false
    viewProjID = glGetUniformLocation(shader, "viewProj");
    if (viewProjID < 0) {
        std::cerr << "Unable to find viewProj uniform" << std::endl;
        return false;
    }

    terrainModelToWorldID = glGetUniformLocation(shader, "modelToWorld");
    if (terrainModelToWorldID < 0) {
        std::cerr << "Unable to find modelToWorld uniform" << std::endl;
        return false;
    }

    cameraPosID = glGetUniformLocation(shader, "cameraPos");
    if (cameraPosID < 0) {
        std::cerr << "Unable to find cameraPos uniform" << std::endl;
        return false;
    }

    return true;
}


// Update OpenGL
void SDLGraphicsProgram::update() {
    if (shouldMove)
        camera.updateTranslation(movementDir);
    terrain->updatePatchPositions(camera.getPosition());
}


// Render
// The render function gets called once per loop
void SDLGraphicsProgram::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(94.f/255.0f, 100.f/255.0f, 100.f/255.0f, 1.f);

    //Clear color buffer and Depth Buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(shader);
    glEnableVertexAttribArray(0);

    // Don't copy the matrix and vector data here
    const glm::mat4& viewProj = camera.getTransform();
    const glm::vec3& cameraPos = camera.getPosition();

    // Transfer data to gpu uniforms
    glUniformMatrix4fv(viewProjID, 1, GL_FALSE, &viewProj[0][0]);
    glUniform3fv(cameraPosID, 1, &cameraPos[0]);

    glVertexAttribPointer(0,  // Attribute 0, which will match layout in shader
                          3,            // size
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          0,                // Stride
                          (void *) 0
    );

    lights->bind();

    terrain->bindTextures();
    terrain->bindVertexBuffer();
    terrain->bindIndexBuffer();
    auto patchesToRender = terrain->getTerrainPatches();
    const GLsizei indexBufferCount = terrain->getIndexBufferCount();
    for (auto terrainPatch : patchesToRender) {
        const glm::mat4 &terrainTransform = terrainPatch->getModelToWorldTransform();
        glUniformMatrix4fv(terrainModelToWorldID, 1, GL_FALSE, &terrainTransform[0][0]);
        glDrawElements(GL_PATCHES, indexBufferCount, GL_UNSIGNED_INT, nullptr);
    }
    // Remove our program
    glDisableVertexAttribArray(0);
    glUseProgram(NULL);
}

void SDLGraphicsProgram::rotateSun(const float angle, const glm::vec3& axis) {
  auto& sun = lights->getSun();
  const glm::vec3& prevSunLocation = sun.getPosition();
  const glm::vec3 rotatedLoc = glm::rotate(prevSunLocation, angle, axis);
  //std::cout << "rotated sun vec: " << rotatedLoc.x << ", " << rotatedLoc.y << ", " << rotatedLoc.z << std::endl;
  sun.setPosition(rotatedLoc);
}

void SDLGraphicsProgram::handleMove(SDL_Keycode keyCode) {
    // TODO make moveSpeed not depend on framerate
    const float rotateSpeed = 0.1f;
    shouldMove = false;
    switch(keyCode) {
        case SDLK_UP:{
            movementDir = glm::vec2(0.0f, moveSpeed);
            shouldMove = true;
            break;
        }
        case SDLK_DOWN:{
            movementDir = glm::vec2(0.0f, -moveSpeed);
            shouldMove = true;
            break;
        }
        case SDLK_RIGHT:{
            movementDir = glm::vec2(moveSpeed, 0.0f);
            shouldMove = true;
            break;
        }
        case SDLK_LEFT:{
            movementDir = glm::vec2(-moveSpeed, 0.0f);
            shouldMove = true;
            break;
        }
        case SDLK_s:{
          rotateSun(rotateSpeed, glm::vec3(1.0, 0.0, 0.0));
          break;
        }
        case SDLK_x:{
          rotateSun(-rotateSpeed, glm::vec3(1.0, 0.0, 0.0));
          break;
        }
        case SDLK_z:{
          rotateSun(-rotateSpeed, glm::vec3(0.0, 0.0, 1.0));
          break;
        }
        case SDLK_c:{
          rotateSun(rotateSpeed, glm::vec3(0.0, 0.0, 1.0));
          break;
        }
        default: {
            break;
        }
    }
}

bool SDLGraphicsProgram::handleKey(const SDL_Keycode keyCode) {
    bool quit = false;
    switch(keyCode) {
        //Toggle wireframe
        case SDLK_w:{
            if (isWireframe){
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            isWireframe = !isWireframe;
            break;
        }
        //quit app
        case SDLK_q:{
            quit = true;
            break;
        }
        // Increase move speed
        case SDLK_1:{
            moveSpeed += 0.05f;
            break;
        }
        // Decrease move speed
        case SDLK_2:{
            moveSpeed -= 0.05f;
            break;
        }
        default:
            break;
    }
    return quit;
}

void SDLGraphicsProgram::loop() {
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // While application is running
    while (!quit) {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            switch(e.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                {
                    shouldMove = false;
                    quit = handleKey(e.key.keysym.sym);
                    break;
                }
                case SDL_KEYDOWN:
                {
                    handleMove(e.key.keysym.sym);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    handleMouseEvent(e.motion.x, e.motion.y);
                    break;
                }
                default:
                    break;
            }
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        update();
        // Render using OpenGL
        render();
        //Update screen of our specified window
        SDL_GL_SwapWindow(getSDLWindow());
    }

    //Disable text input
    SDL_StopTextInput();
}

void SDLGraphicsProgram::handleMouseEvent(int x, int y) {
    const float offsetX = (float)x - (float) screenWidth / 2.0f;
    const float offsetY = (float)y - (float) screenHeight / 2.0f;
    // TODO make these not depend on framerate
    camera.updateRotation(offsetX * -0.01f, offsetY * -0.01f);
    SDL_WarpMouseInWindow(getSDLWindow(), screenWidth / 2, screenHeight / 2);
}

unsigned int SDLGraphicsProgram::CreateShader(const std::string &vertexShaderSource,
                                              const std::string &tessControlSource,
                                              const std::string &tessEvalSource,
                                              const std::string &fragmentShaderSource) {

    // Create a new program
    unsigned int program = glCreateProgram();
    // Compile our shaders
    unsigned int myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int tcShader = CompileShader(GL_TESS_CONTROL_SHADER, tessControlSource);
    unsigned int teShader = CompileShader(GL_TESS_EVALUATION_SHADER, tessEvalSource);
    unsigned int myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    // Link our program
    glAttachShader(program, myVertexShader);
    glAttachShader(program, tcShader);
    glAttachShader(program, teShader);
    glAttachShader(program, myFragmentShader);
    // Link our programs together
    glLinkProgram(program);
    glValidateProgram(program);
    int programStatus = GL_FALSE;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &programStatus);
    if (programStatus != GL_TRUE) {
        GLsizei logLen;
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, sizeof(infoLog), &logLen, infoLog);
        std::string_view logView(infoLog, (size_t)logLen);
        std::stringstream errorStream;
        errorStream << "Failed to link program:\n" << logView;
        throw std::invalid_argument(errorStream.str());
    }
    // Once the shaders have been linked in, we can delete them.
    glDetachShader(program, myVertexShader);
    glDetachShader(program, tcShader);
    glDetachShader(program, teShader);
    glDetachShader(program, myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(tcShader);
    glDeleteShader(teShader);
    glDeleteShader(myFragmentShader);

    return program;
}


unsigned int SDLGraphicsProgram::CompileShader(unsigned int type, const std::string &source) {
    // Compile our shaders
    unsigned int id = glCreateShader(type);

    const char *src = source.c_str();
    // The source of our shader
    glShaderSource(id, 1, &src, nullptr);
    // Now compile our shader
    glCompileShader(id);

    // Retrieve the result of our compilation
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *errorMessages = new char[length]; // Could also use alloca here.
        glGetShaderInfoLog(id, length, &length, errorMessages);
        std::cout << "ERROR: compilation failed!\n" << errorMessages << "\n";
        // Reclaim our memory
        delete[] errorMessages;
        // Delete our broken shader
        glDeleteShader(id);
        return 0;
    }

    return id;
}


// Get Pointer to Window
SDL_Window *SDLGraphicsProgram::getSDLWindow() {
    return gWindow;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::getOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}
