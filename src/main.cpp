// Support Code written by Michael D. Shah
// Last Updated: 1/21/17
// Please do not redistribute without asking permission.
#include "SDLGraphicsProgram.h"
#include <iostream>

// hack to get SDL link to work on windows
#undef main 

// The main entry piont into our program.
int main( int argc, char* args[] )
{
		std::cout << "Starting fractal terrain web app" << std::endl;
      // Create an instance of an object for a SDLGraphicsProgram
      SDLGraphicsProgram mySDLGraphicsProgram(1280,720);
      // Print out some useful information
      mySDLGraphicsProgram.getOpenGLVersionInfo();
      // Run our program forever
      mySDLGraphicsProgram.loop();
      // When our program ends, it will exit scope, the
      // destructor will then be called and clean up the program.

	return 0;
}
