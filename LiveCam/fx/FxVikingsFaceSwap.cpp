#include <fx/FxVikingsFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxVikingsFaceSwap::FxVikingsFaceSwap()
{
	loadFromJSON("./assets/fx/vikings/vikings-face-swap-v2.json");
}

FxVikingsFaceSwap::~FxVikingsFaceSwap()
{
}
