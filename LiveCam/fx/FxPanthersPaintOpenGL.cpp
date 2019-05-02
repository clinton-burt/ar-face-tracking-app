#include "fx/FxPanthersPaintOpenGL.h"

FxPanthersPaintOpenGL::FxPanthersPaintOpenGL()
{

	auto radialNormalsShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/special/panthersPaint_RadialNormalsVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto eyes = make_shared<OpenGL3DModel>();
	eyes->name = "eyes";
	eyes->modelPath = "./assets/fx/panthers/panthersPaint/FacePaint_Eyes.obj";
	eyes->texturesPaths.fill({
		"./assets/fx/panthers/panthersPaint/Left_EyeSurface_Color.png",
		"./assets/fx/panthers/panthersPaint/Right_EyeSurface_Color.png"
	});
	eyes->modelScale = 0.71f;
	eyes->modelShift = { 0, 0, 0.44f };


	/*
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		eyes->renderParams.faceXCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		eyes->renderParams.faceYCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		eyes->renderParams.pitchSmoother[i].ACTIVITY_THRESHOLD = 0;
		eyes->renderParams.yawSmoother[i].ACTIVITY_THRESHOLD = 0;
		eyes->renderParams.rollSmoother[i].ACTIVITY_THRESHOLD = 0;
		eyes->renderParams.faceWidthSmoother[i].ACTIVITY_THRESHOLD = 0;
	}*/

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto leftEyeRenderParams = make_shared<ObjectRenderParams>();
	leftEyeRenderParams->lightPos = { 0.25f, 0.25f, 1, 0 };
	leftEyeRenderParams->cameraPos = { 0, 0, 0, 1 };
	leftEyeRenderParams->ambientLight = { 0.1f, 0.1f, 0.1f };
	leftEyeRenderParams->diffuseLight = { 0.7f, 0.7f, 0.7f };
	leftEyeRenderParams->specularLight = { 1, 1, 1 };
	leftEyeRenderParams->specularPower = 20;
	leftEyeRenderParams->shader = radialNormalsShader;
	leftEyeRenderParams->additionalUniforms["RadialCenter"] = TUniform3f( 3.26f, 0, 10 );

	auto rightEyeRenderParams = make_shared<ObjectRenderParams>();
	rightEyeRenderParams->lightPos = { 0.25f, 0.25f, 1, 0 };
	rightEyeRenderParams->cameraPos = { 0, 0, 0, 1 };
	rightEyeRenderParams->ambientLight = { 0.1f, 0.1f, 0.1f };
	rightEyeRenderParams->diffuseLight = { 0.7f, 0.7f, 0.7f };
	rightEyeRenderParams->specularLight = { 1, 1, 1 };
	rightEyeRenderParams->specularPower = 20;
	rightEyeRenderParams->shader = radialNormalsShader;
	rightEyeRenderParams->additionalUniforms["RadialCenter"] = TUniform3f( -3.26f, 0, 10 );


	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;
	facePaintRenderParams->depthTest = false;

	eyes->objectRenderParams.fill({ leftEyeRenderParams, rightEyeRenderParams });
	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0.0f, 0.5f, 0.83f, 0.7),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0.1f, 0.1f, 0.1f, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		"",
		"./assets/fx/panthers/panthersPaint/stripes_black.png",
		"",
		5.f,
		""
	});

	models.push_back(facePaint);
	//models.push_back(eyes);
}

FxPanthersPaintOpenGL::~FxPanthersPaintOpenGL()
{
}