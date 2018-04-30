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
The following steps have been tested to work on MacOSX with SDL2 installed in
the default location. From a bash terminal in the project directory, run the following:
* Step 1: mkdir build
* Step 2: cd build
* Step 3: cmake ..
* Step 4: make (or open the Visual Studio solution and build if on Windows)
* Step 5: ./FractalTerrainRendering

## Controls:
* Arrow keys: move the camera
* Mouse: change camera direction
* w: toggle wireframe
* 1: Increase move speed
* 2: Decrease move speed
* q: quit
* s/x/z/c: rotate the sun
### Deliverables

* You need to commit your code to this repository.
* You need to have a makefile, compile script, visual studio project, or Xcode project and directions on how to run your program. If your program does not compile and run, you get a zero!

### Rubric

* (33.3%) Project Complection
  * Was the proposed project completed?
  * Does the project compile and run
  * Is it polished without any bugs (No weird visual artifacts)
* (33.3%) Technical
  * Was the implementaiton of the project challenging?
    * Even if you followed some tutoral, it should not be trivial, and have some personal touches to it.
  * Did you have to organize/process a sufficient amount of data.
  * Was it clear you consulted some outside resources that go above and beyond the scope of this class?
* (33.4%) Creativity
  * How visually appealing is the scene?
    * Note: There should be some 'wow' factor.
  * How original is the project
    * Again, did you enhance a tutorial and do something unique or just go by the book?
