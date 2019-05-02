#include <fx/FxMiamiHeatSuperHeroesFaceSwap.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxMiamiHeatSuperHeroesFaceSwap::FxMiamiHeatSuperHeroesFaceSwap()
{
	loadFromJSON("./assets/fx/miamiHeat/miami-heat-super-heroes-face-swap-modules.json");
}

FxMiamiHeatSuperHeroesFaceSwap::~FxMiamiHeatSuperHeroesFaceSwap()
{
}
