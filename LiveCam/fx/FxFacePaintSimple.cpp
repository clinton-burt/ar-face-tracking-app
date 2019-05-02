#include "fx/FxFacePaintSimple.h"
#include <iostream> 
#include "MeshMaker.h"

FxFacePaintSimple::FxFacePaintSimple()
{
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-chargers.json");
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-49ers.json");
	//loadFromJSON("./assets/fx/vikings/vikings-face-paint.json");
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-bills.json");
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-ny-giants.json");
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-bucs.json");
	//loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-cowboys.json");
	loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules-eagles.json");
	//loadFromJSON("./assets/fx/falcons/face-paint-modules-falcons.json");

	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	facePaint->applySuit(filterModules[0].suits[0], 0, commonRenderParams, false);
	facePaint->applySuit(filterModules[0].suits[0], 1, commonRenderParams, false);
	facePaint->applySuit(filterModules[0].suits[0], 2, commonRenderParams, false);
	facePaint->applySuit(filterModules[0].suits[0], 3, commonRenderParams, false);
	facePaint->applySuit(filterModules[0].suits[0], 4, commonRenderParams, false);

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	models.push_back(facePaint);
}

FxFacePaintSimple::~FxFacePaintSimple()
{
}