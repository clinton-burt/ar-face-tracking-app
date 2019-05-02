#include <fx/FxEmojiFace3d.h>
#include <iostream> 
#include <tracker/FaceTracker.h>


FxEmojiFace3d::FxEmojiFace3d()
{
	auto regularShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/special/emojiFace3d_ColorFragmentShader.txt"); // load (and compile, link) from file
	auto picShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/special/emojiFace3d_TextureFragmentShader.txt"); // load (and compile, link) from file

	//std::string capModelFile = "./assets/fx/emojiFace3d/models/model.obj";
	//std::string textureFile = "./assets/fx/emojiFace3d/textures/logoGC.png";

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/emojiFace3d/models/model.obj";
	model->name = "model";
	model->modelScale = 2.13f;

	auto smileRenderParams = make_shared<ObjectRenderParams>();
	smileRenderParams->shader = regularShader;
	smileRenderParams->lightPos = { 0, 0, 100, 0 };
	smileRenderParams->cameraPos = { 0, 1.f, 1.f, 1 };
	smileRenderParams->materialColor = { 1, 1, 0, 1 };
	smileRenderParams->specularLight = { 1, 1, 1 };
	smileRenderParams->ambientLight = { 0.05f, 0.05f, 0.05f };

	auto glassesRenderParams = make_shared<ObjectRenderParams>();
	glassesRenderParams->shader = regularShader;
	glassesRenderParams->lightPos = { 0, 0, 100, 0 };
	glassesRenderParams->cameraPos = { 0, 1.f, 1.f, 1 };
	glassesRenderParams->materialColor = { 0, 0, 0, 1 };
	glassesRenderParams->specularLight = { 1, 1, 1 };
	glassesRenderParams->ambientLight = { 0.05f, 0.05f, 0.05f };

	model->objectRenderParams.fill( { smileRenderParams, glassesRenderParams });

	models.push_back(model);

	cubemapID = cubemapTexId;
}

FxEmojiFace3d::~FxEmojiFace3d()
{

}





