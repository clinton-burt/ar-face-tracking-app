#include "fx/FxCowboysFacePaint.h"

FxCowboysFacePaint::FxCowboysFacePaint()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.000000, 0.164706, 0.360784, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0.686275, 0.713726, 0.737255, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/cowboys/face/star.png",
		"./assets/fx/cowboys/face/star.png",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxCowboysFacePaint::~FxCowboysFacePaint()
{
}