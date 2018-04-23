# Final Project - Fractal Terrain Rendering

*TODO*: Please edit the following information in your assignment

* Name and partners name(At most 1 partner for this project!): Ben Trapani, Manuj Goyal
* How many hours did it take you to complete this lab? 8
* Did you collaborate or share ideas with any other students/TAs/Professors? None
* Did you use any external resources? 
  * Camera math reference: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
  * GLM library: https://github.com/g-truc/glm
  * SDL2 CMake script: https://github.com/brendan-w/collector/blob/master/cmake/FindSDL2.cmake
  * Tesselation example: http://ogldev.atspace.co.uk/www/tutorial30/tutorial30.html
  * Tesselation based on screen coordinates: http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
  * Terrain assets: http://nobiax.deviantart.com (see about.txt in assets directory)
  * Fog effect: http://in2gpu.com/2014/07/22/create-fog-shader/
  * Noise2D glsl implementation: https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl
* (Optional) What was the most interesting part of the assignment? How would you improve this assignment?
  Seeing the result of loading the complex bunny model was really cool. Adding
  functionality for moving around the resulting scene would have been helpful in diagnosing issues with
  the loaded models, as they were a bit hard to see in their default position.
## Description

You have learned that we can draw polygons to the screen by plotting them out one triangle at a time. We can even use mathematical functions to draw nice curves and geometric patterns (e.g. the sierpinski triangle).  Often times to create more complex shapes, 3D objects will be scanned from the real world by lasers or modeled in 3D software.  This data is then output in a 3D file format that can be read in and drawn to the screen..
  
## Task 1 - Render a model

For this assignment you are going to render a 3D model. 

The tasks for rendering in this assignment are the following:
- Make sure you can parse the .obj file format (i.e. read in the vertices and faces). This is your geometry data.
- Make sure you can store the geoemtry data.
- Make sure you know how to work with vertex buffers.
  - You will also be using an index buffer in order to be more efficient in loading the .obj file.
  - Here is a resource motivating index buffers: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/

## Task 2 - Interactive graphics

The tasks for interactivity in this assignment are the following:
- Pressing the 'w' key draws your object in wireframe mode.
- Pressing the 'q' key exits the application.
- Pressing the '1' key renders a cube (cube is shown by default)
- Pressing the '2' key renders a bunny

Additional interactivity:
Use arrow keys to move camera, and mouse to change look at

## OBJ Format

The models that you will be loading are in the .obj format. This format stores the vertices, normals, and faces of an object (and potentially other information as well). The easiest way to become familiar with the OBJ file format is to investigate a 3D Cube model. Here is a cube exported in the .obj file foramt from the free tool Blender3D.

![Alt text](/media/cube.png?raw=true "Cube in Blender3D")

### Parsing the file

(For this assignment, ignore the .mtl materials files)
```
mtllib cube.mtl
o Cube1
#8 vertices, 12 faces
v -1.00000000 -1.00000000 -1.00000000
v -1.00000000 -1.00000000 1.00000000
v -1.00000000 1.00000000 -1.00000000
v -1.00000000 1.00000000 1.00000000
v 1.00000000 -1.00000000 -1.00000000
v 1.00000000 -1.00000000 1.00000000
v 1.00000000 1.00000000 -1.00000000
v 1.00000000 1.00000000 1.00000000
vn -0.57735027 -0.57735027 -0.57735027
vn -0.57735027 -0.57735027 0.57735027
vn -0.57735027 0.57735027 -0.57735027
vn -0.57735027 0.57735027 0.57735027
vn 0.57735027 -0.57735027 -0.57735027
vn 0.57735027 -0.57735027 0.57735027
vn 0.57735027 0.57735027 -0.57735027
vn 0.57735027 0.57735027 0.57735027
g Cube1_default
usemtl default
s 1
f 1//1 5//5 2//2
f 2//2 3//3 1//1
f 2//2 5//5 6//6
f 2//2 8//8 4//4
f 3//3 5//5 1//1
f 3//3 8//8 7//7
f 4//4 3//3 2//2
f 4//4 8//8 3//3
f 5//5 8//8 6//6
f 6//6 8//8 2//2
f 7//7 5//5 3//3
f 7//7 8//8 5//5
```

For this file, the encoding is the following:
* Lines that start with *v* stands for a 'vertex'. A vertex has an x,y,z coordinate that follows
* Lines that start with *vn* stands for a 'vertex normal'. A vertex normal is a normalized directional vector(from 0,0,0 to its location)
* Lines that start with *f* stands for 'face'. A face can consist of any number of vertices. The first number is the index of the vertex we are using. The second number after the double slashes (//) is the vertex normal index.
* The other fields (that start with o, s, or the .mtl file) can be ignored for this assignment.

### Assignment strategy

My suggested strategy for this project is to:

* You may use compile scripts, code from lab, or other materials I have provided that may be helpful.
* Have a special C++ class for loading OBJ models
  * Utilize any data structure you like in the STL (e.g. vector)
  * You may assume all faces are triangles (though if you download any test .obj files from the web that may not be the case)
* Think about how you can load a line of text and then split it into individual tokens.
  * A resource loading files (Filo I/O) in C++ can be found here: http://www.cplusplus.com/doc/tutorial/files/
  * The reference on strings may be useful: http://www.cplusplus.com/reference/string/string/
    * Some ideas on string splitting: http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
* You do not need to use any complex shaders. In fact, I recommend using the most basic ones for this assignment.
* I recommend using this webpage for help with OpenGL: http://docs.gl/gl3/glPolygonMode

### How to run your program

*TODO*: You need to include directions on how to run your program here. 
(Pretend you are deploying this software to someone who has no idea what your code does and needs to be able to run it. You can assume your user has SDL2 setup however)

The following steps have been tested to correctly build and run GraphicsHW1 on Mac with SDL2 installed:
* Step 1: git clone https://github.com/Spring18CS4300/assignment1-render-a-model-bentrapani.git
* Step 2: cd assignment1-render-a-model-bentrapani
* Step 3: mkdir build-dir
* Step 4: cd build-dir
* Step 5: cmake ../
* Step 6: make
* Step 7: ./GraphicsHW1

### Deliverables

* You need to commit your code to this repository.
* You need to have a makefile, compile script, visual studio project, or Xcode project and directions on how to run your program. If your program does not compile and run, you get a zero!

### Rubric

* 10% - Do you have directions on how to run your program? 
  * (I should be able to download your repository and run very quickly!)
* 10% - Is your code well documented and written clearly (i.e. there should be comments and good use of C++ code)?
* 20% - Do the 'w', 'q', 1, and 2 keys work?
* 40% - Can you render the Cube?
* 20% - Can you render the bunny object?
  * (You should be able to toggle between each model)

## More Resources

Links on the OBJ Model Format
* https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html (I recommend reading this one for more information)
* https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html
* https://en.wikipedia.org/wiki/Wavefront_.obj_file
