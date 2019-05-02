#include "fx/FxFacePaintRavensNew.h"

FxFacePaintRavensNew::FxFacePaintRavensNew()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingFragmentShader.txt");

	auto facePaint = make_shared<OpenGLFacePaintModel>();
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
		//Eigen::Vector4f(164 / 255.f, 26 / 255.f, 47 / 255.f, 1),
		//Eigen::Vector4f(17 / 255.f, 26 / 255.f, 48 / 255.f, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 1),
		//"./assets/fx/yankees/ravens_facepaint_texture.png",
		"./assets/fx/canadians/newFacePaint/left.png",
		"./assets/fx/canadians/newFacePaint/right.png",
		"",
		0,
		"./assets/alphaMasks/crude_paint.png"
	});

	facePaint->transformVersion = TransformVersion::V2;

	models.push_back(facePaint);
}

FxFacePaintRavensNew::~FxFacePaintRavensNew()
{
}