#include <fx/FxMiamiHeatPastPlayersFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxMiamiHeatPastPlayersFaceSwap::FxMiamiHeatPastPlayersFaceSwap()
{
	loadFromJSON("./assets/fx/miamiHeat/miami-heat-past-players-face-swap-modules.json");
}

FxMiamiHeatPastPlayersFaceSwap::~FxMiamiHeatPastPlayersFaceSwap()
{
}
