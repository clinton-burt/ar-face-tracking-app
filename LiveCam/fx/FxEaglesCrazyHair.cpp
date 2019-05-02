#include <fx/FxEaglesCrazyHair.h>


FxEaglesCrazyHair::FxEaglesCrazyHair()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/eagles/crazy_hair/Eagles_CrazyHair_GolfCap_v003.obj";
	model->texturesPaths.fill( { 
		"./assets/fx/eagles/crazy_hair/tex/fabricTexture.png", 
		"./assets/fx/eagles/crazy_hair/tex/tennisCap_colorMap.png" ,
		"./assets/fx/eagles/crazy_hair/tex/vgh_ps_hair2.png" });
	model->name = "Eagles_CrazyHair_GolfCap";
	model->modelShift = { 0, 0.61f, 0.12f };
	model->modelScale = 1.65f;
	
	auto depthMask = make_shared<DepthMask>(-0.06, false, true);
	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	skinRenderParams->specularPower = 10;
	skinRenderParams->shader = textureShader;
	skinRenderParams->blend = true;

	auto teethRenderParams = make_shared<ObjectRenderParams>(); 
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 10;
	teethRenderParams->shader = textureShader;
	teethRenderParams->blend = true;
	teethRenderParams->depthMask = false;

	model->objectRenderParams.fill( { skinRenderParams, skinRenderParams, teethRenderParams });

	models.push_back(depthMask);
	models.push_back(model);
}

FxEaglesCrazyHair::~FxEaglesCrazyHair()
{

}