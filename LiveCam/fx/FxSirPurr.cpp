#include "fx/FxSirPurr.h"

FxSirPurr::FxSirPurr()
{
	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/panthers/sirPurr/SirPurrFace.obj";
	model->texturesPaths.fill({ "./assets/fx/panthers/sirPurr/SirPur_Face_UVTemplate.jpg" });
	model->name = "sirPurr";
	model->modelScale = 1.03f;
	model->modelShift = { 0, 0, 0.42f };

	/*
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		model->renderParams.faceXCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceYCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.rollSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.pitchSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.yawSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceWidthSmoother[i].ACTIVITY_THRESHOLD = 0;
	}*/

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/panthers/sirPurr/tongue/NoMotionBlur/");
	
	tongue->animationOffset.fill({ 0, 0.13f, 0 });
	tongue->animationScale.fill({ 0.6f, 0.6f });
	
 	tongue->animationOffset.fill( { 0, -1.3f, 0 } );
 	tongue->animationScale.fill( { 1.2f, 1.2f } );
	
	tongue->animationOpened.fill(40);
	//tongue->animationLength.fill(66);

	auto modelRenderParams = make_shared<ObjectRenderParams>();
	modelRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	modelRenderParams->cameraPos = { 0, 0, 0, 1 };
	modelRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	modelRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	modelRenderParams->specularLight = { 0, 0.0, 0.0 };
	modelRenderParams->specularPower = 1;
	modelRenderParams->shader = shader3D;

	model->objectRenderParams.fill({ modelRenderParams });
	tongue->objectRenderParams.fill({ tongue->createDefaultObjectRenderParams() });

	models.push_back(model);
	//models.push_back(tongue);
}

FxSirPurr::~FxSirPurr()
{
}