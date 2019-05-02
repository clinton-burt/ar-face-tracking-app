#include "fx/FxFacePaint.h"
#include <iostream> 
#include "MeshMaker.h"

FxFacePaint::FxFacePaint()
{

	loadFromJSON("./assets/fx_config/filter-modules/face-paint-modules.json");


	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	facePaint->applySuit(filterModules[0].suits[0], 0, commonRenderParams, false);
	facePaint->applySuit(filterModules[1].suits[0], 1, commonRenderParams, false);
	facePaint->applySuit(filterModules[2].suits[0], 2, commonRenderParams, false);
	facePaint->applySuit(filterModules[3].suits[0], 3, commonRenderParams, false);
	facePaint->applySuit(filterModules[4].suits[0], 4, commonRenderParams, false);

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	models.push_back(facePaint);
}

FxFacePaint::~FxFacePaint()
{
}