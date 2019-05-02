#include "fx/FxFacePaintRavens.h"

FxFacePaintRavens::FxFacePaintRavens()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");

	auto facePaint = make_shared<RavensFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	/*facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.11f, 0.157f, 0.255f, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/yankees/New_York_Yankees_logo.png",
		"./assets/fx/yankees/yankees_new.png",
		"",
		0
	});*/

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.65f, 0.098f, 0.18f, 0.7),
		Eigen::Vector4f(16.f/255.f, 24.f/255.f, 46.f/255.f, 0.7f),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/yankees/ravens_logo.png",
		"./assets/fx/yankees/ravens_texture.png"
	});

	facePaint->transformVersion = TransformVersion::V2;

	models.push_back(facePaint);
}

FxFacePaintRavens::~FxFacePaintRavens()
{
}

void FxFacePaintRavens::drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel)
{

}
