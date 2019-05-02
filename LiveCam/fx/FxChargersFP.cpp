#include "fx/FxChargersFP.h"

FxChargersFP::FxChargersFP()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });
	
	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(1.000000, 0.760784, 0.058824, 0.7),
		Eigen::Vector4f(0.000000, 0.164706, 0.360784, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		"",
		"",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxChargersFP::~FxChargersFP()
{
}