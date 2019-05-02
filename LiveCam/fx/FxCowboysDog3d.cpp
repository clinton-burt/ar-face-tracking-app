#include "fx/FxCowboysDog3d.h"
const int TONGUE_CLOSED = 0;
const int TONGUE_OPEN = 1;
const int TONGUE_CLOSING = 2;

FxCowboysDog3d::FxCowboysDog3d()
{
	auto shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoFragmentShader.txt");

	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto depthMask = make_shared<DepthMask>(0, true, true);
	depthMask->modelShift = { 0, 0, -0.05f };
	depthMask->modelScale = 1.95f;

	auto dogEarModel = make_shared<OpenGL3DModel>();
	dogEarModel->modelPath = "./assets/fx/cowboys/dog/dog_ears/Cowboys_DogEars_v3.obj";
	dogEarModel->texturesPaths.fill({ "./assets/fx/cowboys/dog/dog_ears/tex/cowboysDog_ears.png" });
	dogEarModel->name = "ears";
	dogEarModel->modelShift = { 0, 0, 0.24f };
	dogEarModel->modelScale = 2.4f;

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "Cowboys Tongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/cowboys/dog/tongue/");

	tongue->animationOffset.fill({ 0, -0.95f, 0 });
	tongue->animationScale.fill({ 0.8f, 0.8f });
	tongue->animationOpened.fill(74);

	auto earsRenderParams = make_shared<ObjectRenderParams>();
	earsRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	earsRenderParams->cameraPos = { 0, 0, 0, 1 };
	earsRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	earsRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	earsRenderParams->specularLight = { 0, 0, 0 };
	earsRenderParams->specularPower = 1;
	earsRenderParams->shader = shader3D;
	
	auto tongueRenderParams = make_shared<ObjectRenderParams>();
	tongueRenderParams->shader = shader2D;
	tongueRenderParams->blend = true;

	dogEarModel->objectRenderParams.fill({ earsRenderParams });
	tongue->objectRenderParams.fill({ tongueRenderParams });

	models.push_back(depthMask);
	models.push_back(dogEarModel);
	models.push_back(tongue);
}

FxCowboysDog3d::~FxCowboysDog3d()
{

}