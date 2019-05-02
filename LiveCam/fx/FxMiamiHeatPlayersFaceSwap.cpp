#include <fx/FxMiamiHeatPlayersFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxMiamiHeatPlayersFaceSwap::FxMiamiHeatPlayersFaceSwap()
{
	loadFromJSON("./assets/fx/miamiHeat/miami-heat-players-face-swap-modules.json");
}

FxMiamiHeatPlayersFaceSwap::~FxMiamiHeatPlayersFaceSwap()
{
}
