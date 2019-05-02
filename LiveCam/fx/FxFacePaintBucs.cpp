#include "fx/FxFacePaintBucs.h"

FxFacePaintBucs::FxFacePaintBucs()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.84, 0.66, 0.56, 0.6),
		Eigen::Vector4f(0.854902, 0.129412, 0.152941, 0.6),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/bucs/facepaint/left_side_bucs.png",
		"",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxFacePaintBucs::~FxFacePaintBucs()
{
}