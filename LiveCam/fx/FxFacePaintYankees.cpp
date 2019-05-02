#include "fx/FxFacePaintYankees.h"

FxFacePaintYankees::FxFacePaintYankees()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.11f, 0.157f, 0.255f, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/yankees/New_York_Yankees_logo.png",
		"./assets/fx/yankees/yankees_new.png",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxFacePaintYankees::~FxFacePaintYankees()
{
}