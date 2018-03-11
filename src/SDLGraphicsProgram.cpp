#include "SDLGraphicsProgram.h"
#include "Utilities.h"

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h) : screenWidth(w), screenHeight(h),
                                                       isWireframe(false),
                                                       camera(45, (float)w / (float)h),
                                                       shouldMove(false) {
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
        gWindow = SDL_CreateWindow("Lab",
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
    OBJFileReader_t reader("cube.obj");
    auto cubeData = reader.loadData();
    reader.setObjPath("monkey.obj");
    auto bunnyData = reader.loadData();
    models[0] = std::make_shared<SimpleModel<OBJFileReader_t>>(cubeData);
    models[1] = std::make_shared<SimpleModel<OBJFileReader_t>>(bunnyData);
    activeModel = models[0];

    terrainPatch = std::make_shared<TerrainPatch>(16, 16);
    terrainPatch->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
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

    screenSizeID = glGetUniformLocation(shader, "screenSize");
    if (screenSizeID < 0) {
        std::cerr << "Unable to find screenSize uniform" << std::endl;
        return false;
    }

    terrainModelToWorldID = glGetUniformLocation(shader, "modelToWorld");
    if (terrainModelToWorldID < 0) {
        std::cerr << "Unable to find modelToWorld uniform" << std::endl;
        return false;
    }

    return true;
}


// Update OpenGL
void SDLGraphicsProgram::update() {
    if (shouldMove)
        camera.updateTranslation(movementDir);
}


// Render
// The render function gets called once per loop
void SDLGraphicsProgram::render() {
    // Disable depth test and enable face culling.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    //Clear color buffer and Depth Buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Use our shader
    glUseProgram(shader);
    glEnableVertexAttribArray(0);

    // Don't copy the matrix and vector data here
    const glm::mat4& viewProj = camera.getTransform();
    const glm::mat4& terrainTransform = terrainPatch->getModelToWorldTransform();

    // Transfer data to gpu uniforms
    glUniformMatrix4fv(viewProjID, 1, GL_FALSE, &viewProj[0][0]);
    glUniformMatrix4fv(terrainModelToWorldID, 1, GL_FALSE, &terrainTransform[0][0]);
    glUniform2f(screenSizeID, screenWidth, screenHeight);

    //activeModel->bindVertexBuffer();
    terrainPatch->bindVertexBuffer();
    glVertexAttribPointer(0,  // Attribute 0, which will match layout in shader
                          3,            // size
                          GL_FLOAT, // Type
                          GL_FALSE, // Is the data normalized
                          0,                // Stride
                          (void *) 0
    );
    //activeModel->bindIndexBuffer();
    //glDrawElements(GL_TRIANGLES, activeModel->getIndexBufferCount(), GL_UNSIGNED_INT, nullptr);
    terrainPatch->bindIndexBuffer();
    glDrawElements(GL_PATCHES, terrainPatch->getIndexBufferCount(), GL_UNSIGNED_INT, nullptr);

    // Remove our program
    glDisableVertexAttribArray(0);
    glUseProgram(NULL);
}

void SDLGraphicsProgram::handleMove(SDL_Keycode keyCode) {
    // TODO make moveSpeed not depend on framerate
    const float moveSpeed = 0.01f;
    shouldMove = true;
    switch(keyCode) {
        case SDLK_UP:{
            movementDir = glm::vec2(0.0f, moveSpeed);
            break;
        }
        case SDLK_DOWN:{
            movementDir = glm::vec2(0.0f, -moveSpeed);
            break;
        }
        case SDLK_RIGHT:{
            movementDir = glm::vec2(moveSpeed, 0.0f);
            break;
        }
        case SDLK_LEFT:{
            movementDir = glm::vec2(-moveSpeed, 0.0f);
            break;
        }
        default: {
            shouldMove = false;
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
        // Show cube
        case SDLK_1:{
            activeModel = models[0];
            break;
        }
        // Show bunny
        case SDLK_2:{
            activeModel = models[1];
            break;
        }
        default:
            break;
    }
    return quit;
}

//Loops forever!
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
