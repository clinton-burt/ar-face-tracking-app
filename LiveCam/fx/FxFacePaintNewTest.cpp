#include "fx/FxFacePaintNewTest.h"

FxFacePaintNewTest::FxFacePaintNewTest()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");

	auto facePaint = make_shared<OpenGLFacePaintModel2>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->faceParams.fill({
		"./assets/fx/facepaintsimple/Face_MainUVTexture_V1.jpg"
	});

	facePaint->transformVersion = TransformVersion::V2;

	models.push_back(facePaint);
}

FxFacePaintNewTest::~FxFacePaintNewTest()
{
}
