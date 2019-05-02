#include <fx/FxRidersWatermelon.h>


FxRidersWatermelon::FxRidersWatermelon()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/backClippingPhongFragmentShader.txt");

	model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/ridersWatermelon/watermelon_hat_v05.obj";
	model->texturesPaths.fill( { "./assets/fx/ridersWatermelon/melon_hat_v2.png" } );
	model->name = "ridersWatermelon";
	model->modelScale = 1.87f;
	model->modelShift = { 0, 0.91f, 0 };
	
	auto waterMelonRenderParams = make_shared<ObjectRenderParams>();
	waterMelonRenderParams->shader = shader;
	waterMelonRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	waterMelonRenderParams->cameraPos = { 0, 0, 0, 1 };
	waterMelonRenderParams->ambientLight = { 0.7f, 0.7f, 0.7f };
	waterMelonRenderParams->diffuseLight = { 0.7f, 0.7f, 0.7f };
	waterMelonRenderParams->specularLight = { 0.2f, 0.2f, 0.2f };
	waterMelonRenderParams->specularPower = 4;
	waterMelonRenderParams->reflectionRatio = 0.25;
	waterMelonRenderParams->blend = true;

	auto &addUniforms = waterMelonRenderParams->additionalUniforms;

	addUniforms["ZClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["YClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0)); // will be updated every transform

	model->objectRenderParams.fill( { waterMelonRenderParams } );

	models.push_back(model);
}

FxRidersWatermelon::~FxRidersWatermelon()
{

}

void FxRidersWatermelon::load()
{
	FxCore::load();


	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = model->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(model->bmin[2] + (model->bmax[2] - model->bmin[2]) * 0.55f);
		addUniforms["YClip"] = TUniform1f(model->bmin[1] + (model->bmax[1] - model->bmin[1]) * 1);
	}
}

void FxRidersWatermelon::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto ZRotation = model->renderParams.rotationMatrix.block<1, 3>(2, 0).data();

	model->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
	TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));
}