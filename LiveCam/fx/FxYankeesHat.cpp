#include <fx/FxYankeesHat.h>

FxYankeesHat::FxYankeesHat()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/special/yankeesHat_HideBackVertexShader.txt", "./assets/shaders/fragment/special/yankeesHat_HideBackFragmentShader.txt");

	auto depthMask = make_shared<DepthMask>(0, true, true);

	model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/yankees/yankeesHat/Yankees_Hat.obj";
	model->texturesPaths.fill( { "./assets/fx/yankees/yankeesHat/HatSurface_Color.png" } );
	model->name = "hat";
	model->modelScale = 2.23f;
	model->modelShift = { 0, 1.17f, -0.07f };

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->lightPos = { 1, 1, 1, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	hatRenderParams->specularLight = { 0.0, 0.0, 0.0 };
	hatRenderParams->specularPower = 10;
	hatRenderParams->shader = shader;
	hatRenderParams->blend = true;

 	auto &addUniforms = hatRenderParams->additionalUniforms;
 
 	addUniforms["angleK"] = TUniform1f(0); // will be rewritten after loading the model

	model->objectRenderParams.fill( { hatRenderParams } );

	models.push_back(model);
}

FxYankeesHat::~FxYankeesHat()
{
}

void FxYankeesHat::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto &addUniforms = model->objectRenderParams[face.pointId][0]->additionalUniforms;
	addUniforms["angleK"] = TUniform1f(1.f + abs(face.yaw) / 80.0);
}