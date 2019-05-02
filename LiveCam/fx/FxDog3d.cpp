#include "fx/FxDog3d.h"
const int TONGUE_CLOSED = 0;
const int TONGUE_OPEN = 1;
const int TONGUE_CLOSING = 2;

FxDog3d::FxDog3d()
{

	auto shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoFragmentShader.txt");

	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto dogEarModel = make_shared<OpenGL3DModel>();
	dogEarModel->modelPath = "./assets/fx/dog/models/ears2.obj";
	dogEarModel->texturesPaths.fill( {"./assets/fx/dog/textures/DogEar_UVMapTextureV2.jpg" });
	dogEarModel->name = "ears";
	dogEarModel->modelShift = { 0, 0.7f, 0.18f };
	dogEarModel->modelScale = 1.33f;

	auto dogNoseModel = make_shared<LipsJoint3DModel>();
	dogNoseModel->name = "nose";
	dogNoseModel->modelPath = "./assets/fx/dog/models/nose2.obj";
	dogNoseModel->modelShift = { 0, 0.15f, 0 };
	dogNoseModel->modelScale = 0.5f;
	dogNoseModel->texturesPaths.fill({
		"./assets/fx/dog/textures/DogNoseB_UVMapTextureV2_small.jpg",
		"./assets/fx/dog/textures/Cube.1Surface_ColorV2_small.png"
	});

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/dog/animations/tongueSequence/");

 	tongue->animationOffset.fill({ 0, -0.95f, 0 });
 	tongue->animationScale.fill({ 0.8f, 0.8f });
	tongue->animationOpened.fill(46);


	auto earsRenderParams = make_shared<ObjectRenderParams>();
	earsRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	earsRenderParams->cameraPos = { 0, 0, 0, 1 };
	earsRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	earsRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	earsRenderParams->specularLight = { 0, 0, 0 };
	earsRenderParams->specularPower = 1;
	earsRenderParams->shader = shader3D;

	auto noseRenderParams = make_shared<ObjectRenderParams>();
	noseRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	noseRenderParams->cameraPos = { 0, 0, 0, 1 };
	noseRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	noseRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	noseRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	noseRenderParams->specularPower = 30;
	noseRenderParams->shader = shader3D;

	auto snoutRenderParams = make_shared<ObjectRenderParams>();
	snoutRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	snoutRenderParams->cameraPos = { 0, 0, 0, 1 };
	snoutRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	snoutRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	snoutRenderParams->specularLight = { 0, 0, 0 };
	snoutRenderParams->specularPower = 1;
	snoutRenderParams->shader = shader3D;

	auto tongueRenderParams = make_shared<ObjectRenderParams>();
	tongueRenderParams->shader = shader2D;
	tongueRenderParams->blend = true;

	dogEarModel->objectRenderParams.fill({ earsRenderParams });
	dogNoseModel->objectRenderParams.fill({ noseRenderParams, snoutRenderParams });
	tongue->objectRenderParams.fill({ tongueRenderParams });

	models.push_back(dogEarModel);
	models.push_back(dogNoseModel);
	models.push_back(tongue);
}

FxDog3d::~FxDog3d()
{

}