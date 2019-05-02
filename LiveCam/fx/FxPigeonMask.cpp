#include <fx/FxPigeonMask.h>


FxPigeonMask::FxPigeonMask()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/pigeonMask/PigeonMask_004.obj";
	model->texturesPaths.fill( { "./assets/fx/pigeonMask/PigeonHead_UV.jpg" } );
	model->name = "PigeonMask";
	model->modelScale = 1.63f;
	model->modelShift = { 0, 0.351f, 0 };

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->shader = shader;
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->specularLight = { 0.0f, 0.0f, 0.0f };
	teethRenderParams->specularPower = 5;

	model->objectRenderParams.fill( { teethRenderParams } );

	models.push_back(model);
}

FxPigeonMask::~FxPigeonMask()
{

}