#include "utils/Resolutions.h"

namespace Resolutions
{
	// prefered, requested camera frame resolution
	int INPUT_WIDTH = 1920;
	int INPUT_HEIGHT = 1080;

	// actual setted camera frame resolution (may be smaller because of constraints)
	int INPUT_ACTUAL_WIDTH;
	int INPUT_ACTUAL_HEIGHT;

	// OpenGL rendered frame resolution
	int OUTPUT_WIDTH = 1920;
	int OUTPUT_HEIGHT = 1080;
}