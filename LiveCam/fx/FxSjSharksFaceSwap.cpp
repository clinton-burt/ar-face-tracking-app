#include <fx/FxSjSharksFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxSjSharksFaceSwap::FxSjSharksFaceSwap()
{
	loadFromJSON("./assets/fx/sharks/sharks-players-face-swap-modules.json");
}

FxSjSharksFaceSwap::~FxSjSharksFaceSwap()
{
}
