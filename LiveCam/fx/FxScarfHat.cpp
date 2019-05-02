#include <fx/FxScarfHat.h>


FxScarfHat::FxScarfHat()
{
	if (shaderManager == NULL)
		shaderManager = new cwc::glShaderManager;

	auto hatShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/backClippingPhongFragmentShader.txt");
	auto scarfShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto depthMask = make_shared<DepthMask>(0, true, false);
	depthMask->modelScale = 1.0f;
	depthMask->modelShift = { 0, 0, -0.5f };

	hat = make_shared<OpenGL3DModel>();
	hat->modelPath = "./assets/fx/nyc_football_club/scarfHat/WinterHat.obj";
	hat->texturesPaths.fill( { "./assets/fx/nyc_football_club/scarfHat/WinterHat_UVTemplateV4b.jpg" } );
	hat->name = "hat";
	hat->modelScale = 1.45f;
	hat->modelShift = { 0, 0.88f, 0.2f };

	auto scarf = make_shared<OpenGL3DModel>();
	scarf->modelPath = "./assets/fx/nyc_football_club/scarfHat/Scarf.obj";
	scarf->texturesPaths.fill( { "./assets/fx/nyc_football_club/scarfHat/ScarfSurface_Color.png" } );
	scarf->name = "scarf";
	scarf->modelScale = 1.7f;
	scarf->modelShift = { -0.17f, -2.1f, 0.17f };
	scarf->extraRotateMatrix(0, 0) = -0.819152f;
	scarf->extraRotateMatrix(0, 2) = -0.573576f;
	scarf->extraRotateMatrix(2, 0) = 0.573576f;;
	scarf->extraRotateMatrix(2, 2) = -0.819152f;

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->shader = hatShader;
	hatRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	hatRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->specularLight = { 0, 0, 0 };
	hatRenderParams->specularPower = 1;
	hatRenderParams->blend = true;

	auto &addUniforms = hatRenderParams->additionalUniforms;

	addUniforms["ZClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["YClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0)); // will be updated every transform

	auto scarfRenderParams = make_shared<ObjectRenderParams>();
	scarfRenderParams->shader = scarfShader;
	scarfRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	scarfRenderParams->cameraPos = { 0, 0, 0, 1 };
	scarfRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	scarfRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	scarfRenderParams->specularLight = { 0, 0, 0 };
	scarfRenderParams->specularPower = 1;

	hat->objectRenderParams.fill({ hatRenderParams });
	scarf->objectRenderParams.fill({ scarfRenderParams });

	models.push_back(depthMask);
	models.push_back(scarf);
	models.push_back(hat);
}

FxScarfHat::~FxScarfHat()
{

}

void FxScarfHat::load()
{
	FxCore::load();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = hat->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(hat->bmin[2] + (hat->bmax[2] - hat->bmin[2]) * 0.4f);
		addUniforms["YClip"] = TUniform1f(hat->bmin[1] + (hat->bmax[1] - hat->bmin[1]) * 0.25f);
	}
}

void FxScarfHat::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto ZRotation = hat->renderParams.rotationMatrix.block<1, 3>(2, 0).data();
	hat->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
		TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));
}

void FxScarfHat::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	models[0]->draw(face, externalRenderParams); // depthMask
	models[1]->draw(face, externalRenderParams); // scarf
	models[2]->draw(face, externalRenderParams); // hat

	glPopAttrib();
}