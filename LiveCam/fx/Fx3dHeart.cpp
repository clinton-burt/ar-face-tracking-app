#include "fx/Fx3dHeart.h"

Fx3dHeart::Fx3dHeart()
{
	loadFromJSON("./assets/fx/reds/reds-heart-lips-modules.json");

	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");

	auto leftHeartModel = make_shared<OpenGL3DModel>();
	leftHeartModel->modelPath = "./assets/fx/reds/heartEyes/models/heart-left.obj";
	leftHeartModel->texturesPaths.fill( { "./assets/fx/reds/heartEyes/textures/heart_texture.png" } );
	leftHeartModel->name = "left_heart";
	leftHeartModel->visible.fill(false);
	leftHeartModel->modelShift = { -0.21f, 0, 0.43f };
	leftHeartModel->modelScale = 0.3f;

	auto rightHeartModel = make_shared<OpenGL3DModel>();
	rightHeartModel->modelPath = "./assets/fx/reds/heartEyes/models/heart-right.obj";
	rightHeartModel->texturesPaths.fill({ "./assets/fx/reds/heartEyes/textures/heart_texture.png" });
	rightHeartModel->name = "right_heart";
	rightHeartModel->visible.fill(false);
	rightHeartModel->modelShift = { 0.21f, 0, 0.43f };
	rightHeartModel->modelScale = 0.3f;

	auto lipsModel = make_shared<LipsJoint3DModel>();
	lipsModel->modelPath = "./assets/fx/reds/heartEyes/models/lips2.obj";
	lipsModel->texturesPaths.fill({ "./assets/fx/reds/heartEyes/textures/heart_texture.png" });
	lipsModel->name = "lips";
	lipsModel->modelShift = { 0, -0.08f, -0.03f };
	lipsModel->modelScale = 0.56f;

	auto lipsRenderParams = make_shared<ObjectRenderParams>();
	lipsRenderParams->shader = shader;
	lipsRenderParams->cameraPos = { 0, 0, 0,  1 };
	lipsRenderParams->lightPos = { 0, 0, 1,  0 };
	lipsRenderParams->materialColor = { 1, 0, 0, 1};
	lipsRenderParams->specularLight = { 1, 1, 1 };
	lipsRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	lipsRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	lipsRenderParams->specularPower = 10;

	auto heartsRenderParams = make_shared<ObjectRenderParams>();
	heartsRenderParams->shader = shader;
	heartsRenderParams->cameraPos = { 0, 0, 0,  1 };
	heartsRenderParams->lightPos = { 0, 0, 1,  0 };
	heartsRenderParams->materialColor = { 1, 0, 0, 1 };
	heartsRenderParams->specularLight = { 0, 0, 0 };
	heartsRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	heartsRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	heartsRenderParams->specularPower = 1;

	leftHeartModel->objectRenderParams.fill({ heartsRenderParams });
	rightHeartModel->objectRenderParams.fill({ heartsRenderParams });
	lipsModel->objectRenderParams.fill({ lipsRenderParams });

	models.push_back(leftHeartModel);
	models.push_back(rightHeartModel);
	models.push_back(lipsModel);
}

Fx3dHeart::~Fx3dHeart()
{
}