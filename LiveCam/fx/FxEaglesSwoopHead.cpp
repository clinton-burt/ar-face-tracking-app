#include <fx/FxEaglesSwoopHead.h>


FxEaglesSwoopHead::FxEaglesSwoopHead()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/eagles/swoop_head/eagles_swoop_head_v3.obj";
	model->texturesPaths.fill( {  
		"./assets/fx/eagles/swoop_head/tex/SWOOP_Custom_1.png",
		"./assets/fx/eagles/swoop_head/tex/tongueTex.png",
		"./assets/fx/eagles/swoop_head/tex/EYES_Color_1.png" });

	model->name = "Eagles_SwoopMascot_v002";
	model->modelShift = { 0, -0.05f, 0.25f };
	model->modelScale = 2.02f;
	
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
	//model->extraScale = 0.14;
	//model->extraShift = { 0.0f,0.3f,0.0f };
	models.push_back(model);
}

FxEaglesSwoopHead::~FxEaglesSwoopHead()
{

}