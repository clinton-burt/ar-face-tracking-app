#include <fx/FxMarlinsHat.h>

FxMarlinsHat::FxMarlinsHat()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/backClippingPhongFragmentShader.txt");
	
	model = make_shared<OpenGL3DModel>();
	model->name = "hat";
	model->modelPath = "./assets/fx/marlins/marlinsHat/baseball_cap_with_logo.obj";
	model->texturesPaths.fill( {
		"./assets/fx/marlins/marlinsHat/baseball_cap_texture.jpg",
		"./assets/fx/marlins/marlinsHat/Miami_Marlins_2017.png"
	});
	model->modelScale = 1.27f;
	model->modelShift = { 0, 0.47f, 0.2f };

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	hatRenderParams->specularLight = { 0, 0, 0 };
	hatRenderParams->specularPower = 20;
	hatRenderParams->shader = shader;
	hatRenderParams->blend = true;

	auto logoRenderParams = make_shared<ObjectRenderParams>();
	logoRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	logoRenderParams->cameraPos = { 0, 0, 0, 1 };
	logoRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	logoRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	logoRenderParams->specularLight = { 0, 0, 0 };
	logoRenderParams->specularPower = 20;
	logoRenderParams->shader = shader;
	logoRenderParams->cullFace = false;
	logoRenderParams->blend = true;

	model->objectRenderParams.fill( { hatRenderParams, logoRenderParams } );

	auto &addUniforms = hatRenderParams->additionalUniforms;

	addUniforms["ZClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["YClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0)); // will be updated every transform

	models.push_back(model);
}

FxMarlinsHat::~FxMarlinsHat()
{
}

void FxMarlinsHat::load()
{
	FxCore::load();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = model->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(model->bmin[2] + (model->bmax[2] - model->bmin[2]) * 0.35f);
		addUniforms["YClip"] = TUniform1f(model->bmin[1] + (model->bmax[1] - model->bmin[1]) * 1);
	}
}

void FxMarlinsHat::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto ZRotation = model->renderParams.rotationMatrix.block<1, 3>(2, 0).data();

	model->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
		TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));
}