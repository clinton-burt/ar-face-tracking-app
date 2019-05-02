#include <fx/FxRedsFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxRedsFaceSwap::FxRedsFaceSwap()
{
	//loadFromJSON("./assets/fx/miamiHeat/miami-heat-entertainers-face-swap-modules.json");
	loadFromJSON("./assets/fx/reds/reds_face_swap_modules.json");
}

FxRedsFaceSwap::~FxRedsFaceSwap()
{
}
