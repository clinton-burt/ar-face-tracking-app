#include <fx/FxKnightsHelmet.h>

FxKnightsHelmet::FxKnightsHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/cubeMapPhongFragmentShader.txt");
	auto maskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/cubeMapPhongFragmentShader.txt");

	helmet = make_shared<OpenGL3DModel>();
	helmet->modelPath = "./assets/fx/knight/knight_final.obj";
	helmet->texturesPaths.fill({"./assets/fx/knight/tex_structure_rust_1.jpg"});
	helmet->name = "helmet";
	helmet->modelShift = { 0, 0.13f, 0 };
	helmet->modelScale = 1.5f;

	auto helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->shader = helmetShader;
	helmetRenderParams->lightPos = { 1, 1, 1, 0 };
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 0.6122f, 0.5348f, 0.354f };
	helmetRenderParams->diffuseLight = { 0.6122f, 0.5348f, 0.354f };
	helmetRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	helmetRenderParams->specularPower = 20;
	helmetRenderParams->reflectionRatio = 0.25;
	helmetRenderParams->blend = true;

	auto maskRenderParams = make_shared<ObjectRenderParams>();
	maskRenderParams->shader = maskShader;
	maskRenderParams->lightPos = { 1, 1, 1, 0 };
	maskRenderParams->cameraPos = { 0, 0, 0, 1 };
	maskRenderParams->ambientLight = { 0.278f, 0.278f, 0.278f };
	maskRenderParams->diffuseLight = { 0.278f, 0.278f, 0.278f };
	maskRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	maskRenderParams->specularPower = 20;
	maskRenderParams->reflectionRatio = 0.25;

	helmet->objectRenderParams.fill({helmetRenderParams, maskRenderParams});

	auto &addUniforms = helmetRenderParams->additionalUniforms;

	addUniforms["ZClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["YClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0)); // will be updated every transform

	models.push_back(helmet);

	cubemapID = cubemapTexId;
}

FxKnightsHelmet::~FxKnightsHelmet()
{

}

void FxKnightsHelmet::load()
{
	FxCore::load();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = helmet->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(helmet->bmin[2] + (helmet->bmax[2] - helmet->bmin[2]) * 0.35f);
		addUniforms["YClip"] = TUniform1f(helmet->bmin[1] + (helmet->bmax[1] - helmet->bmin[1]) * 1);
	}
}

void FxKnightsHelmet::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto ZRotation = helmet->renderParams.rotationMatrix.block<1, 3>(2, 0).data();

	helmet->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
		TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));

}