#include <fx/FxCrowsFootball.h>


FxCrowsFootball::FxCrowsFootball()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/crows/afl_ball/AFL_Ball_Model_3.obj";
	model->texturesPaths.fill({ "./assets/fx/crows/afl_ball/AFL_Ball_Texture.jpg" });
	model->name = "AngelCrab";
	model->modelShift = { 0, 0.35f, 0.18f };
	model->modelScale = 1.95f;

	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.15f, 0.15f, 0.15f };
	skinRenderParams->specularPower = 5;
	skinRenderParams->shader = textureShader;
	skinRenderParams->blend = true;
	skinRenderParams->depthTest = true;
	skinRenderParams->cullFace = false;

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 15;
	teethRenderParams->shader = textureShader;
	teethRenderParams->blend = true;
	teethRenderParams->depthTest = true;
	teethRenderParams->cullFace = false;

	model->objectRenderParams.fill({ skinRenderParams });

	models.push_back(model);
}

FxCrowsFootball::~FxCrowsFootball()
{

}

void FxCrowsFootball::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	for (auto model : models)
	{
		if (model->visible[face.pointId])
		{
			model->transform(face, externalRenderParams);
		}
	}

}