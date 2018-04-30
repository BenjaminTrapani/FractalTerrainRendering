# Final Project - Computer Graphics

* Name and partners name(At most 1 partner for this project!)
  * Ben Trapani
* How many hours did it take you to complete this lab? ~40
* Did you collaborate or share ideas with any other students/TAs/Professors?
* Link to YouTube video: https://youtu.be/7M5dwhuP6yQ
* Did you use any external resources? 
  * Camera math reference: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
  * GLM library: https://github.com/g-truc/glm
  * SDL2 CMake script: https://github.com/brendan-w/collector/blob/master/cmake/FindSDL2.cmake
  * Tesselation example: http://ogldev.atspace.co.uk/www/tutorial30/tutorial30.html
  * Tesselation based on screen coordinates: http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
  * Terrain assets: http://nobiax.deviantart.com (see about.txt in assets directory)
  * Fog effect: http://in2gpu.com/2014/07/22/create-fog-shader/
  * Noise2D glsl implementation: https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl
  * (Optional) What was the most interesting part of the Final Assignment? How would you improve this assignment?
 
## Description

This project renders infinite terrain with heights and geometry generated on the fly via
perlin noise and tesselation. The phong reflection model is used for lighting. Pre-made
textures are used instead of a perlin noise function to generate textures on the fly, as this
was too slow.
  
### How to run your program
Building this project requires CMake to be installed.

#### Mac
The following steps have been tested to work on MacOSX with SDL2 installed in
the default location (where it gets placed after unpacking the DMG).
From a bash terminal in the project directory, run the following:
* Step 1: mkdir build
* Step 2: cd build
* Step 3: cmake ..
* Step 4: make
* Step 5: ./FractalTerrainRendering

#### Windows
* Download the SDL2 development library here: http://libsdl.org/download-2.0.php
* Place the unzipped folder at C:\local\ and rename it to SDL2 (After opening the new 'SDL2' dir in C:\local\, you should see 'libs' and 'include' directories)
* Open powershell or command prompt at the root of the cloned repo
* mkdir build
* cd build
* cmake .. -DCMAKE_GENERATOR_PLATFORM=x64
* start .\FractalTerrainRendering.sln
* Build as usual, and set the project called FractalTerrainRendering as the startup project to run

#### Controls:
* Arrow keys: move the camera
* Mouse: change camera direction
* w: toggle wireframe
* 1: Increase move speed
* 2: Decrease move speed
* q: quit
* s/x/z/c: rotate the sun
