#include <fx/FxEaglesSwoopBeak.h>


FxEaglesSwoopBeak::FxEaglesSwoopBeak()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<LipsJoint3DModel>();
	model->modelPath = "./assets/fx/eagles/swoop_beak/Eagles_Beak_v002.obj";
	model->texturesPaths.fill( {"./assets/fx/eagles/swoop_beak/tex/SWOOP_Custom_1.png", "./assets/fx/eagles/swoop_head/tex/tongueTex.png" });
	model->name = "Eagles_Beak_v002";
	
	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	skinRenderParams->specularPower = 5;
	skinRenderParams->shader = textureShader;

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 10;
	teethRenderParams->shader = textureShader;

	model->objectRenderParams.fill( { skinRenderParams, skinRenderParams, teethRenderParams });
	//model->extraScale = 1;
	//model->extraShift = { 0, 0, 0 };

	models.push_back(model);
}

FxEaglesSwoopBeak::~FxEaglesSwoopBeak()
{

}