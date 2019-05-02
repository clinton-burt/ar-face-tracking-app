#ifndef RESOLUTIONS_H
#define RESOLUTIONS_H

namespace Resolutions
{
	// prefered, requested camera frame resolution
	extern int INPUT_WIDTH;
	extern int INPUT_HEIGHT;

	// actual setted camera frame resolution (may be smaller because of constraints)
	extern int INPUT_ACTUAL_WIDTH; 
	extern int INPUT_ACTUAL_HEIGHT;

	// OpenGL rendered frame resolution
	extern int OUTPUT_WIDTH;
	extern int OUTPUT_HEIGHT;
}
#endif