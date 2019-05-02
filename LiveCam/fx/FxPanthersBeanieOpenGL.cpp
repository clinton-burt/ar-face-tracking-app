#include "fx/FxPanthersBeanieOpenGL.h"

FxPanthersBeanieOpenGL::FxPanthersBeanieOpenGL()
{	
	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/backClippingSimpleVertexShader.txt", "./assets/shaders/fragment/backClippingPhongFragmentShader.txt");
	
	hat = make_shared<OpenGL3DModel>();
	hat->modelPath = "./assets/fx/panthers/panthersBeanie/Panthers_WinterHat.obj";
	hat->texturesPaths.fill( { "./assets/fx/panthers/panthersBeanie/WinterHat_UVTemplateV5.jpg" } );
	hat->name = "sirPurr";
	hat->modelScale = 1.35f;
	hat->modelShift = { 0, 0.85f, 0.15f };

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/panthers/panthersBeanie/tongue/NoMotionBlur/");
	tongue->animationOffset.fill({ 0, -1.6f, 0 });
	tongue->animationScale.fill({ 2, 2 });
	tongue->animationOpened.fill(50);
	//tongue->animationLength.fill(74);

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	hatRenderParams->specularLight = { 0, 0.0, 0.0 };
	hatRenderParams->specularPower = 1;
	hatRenderParams->shader = shader3D;
	hatRenderParams->blend = true;

	auto &addUniforms = hatRenderParams->additionalUniforms;

	addUniforms["ZClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["YClip"] = TUniform1f(0); // will be rewritten after loading the model
	addUniforms["ZRotation"] = TUniform3f(Eigen::Vector3f(0, 0, 0)); // will be updated every transform

	hat->objectRenderParams.fill({ hatRenderParams });
	tongue->objectRenderParams.fill({ tongue->createDefaultObjectRenderParams() });

	models.push_back(hat);
	//models.push_back(tongue);
}

FxPanthersBeanieOpenGL::~FxPanthersBeanieOpenGL()
{
}

void FxPanthersBeanieOpenGL::load()
{
	FxCore::load();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto &addUniforms = hat->objectRenderParams[i][0]->additionalUniforms;
		addUniforms["ZClip"] = TUniform1f(hat->bmin[2] + (hat->bmax[2] - hat->bmin[2]) * 0.4f);
		addUniforms["YClip"] = TUniform1f(hat->bmin[1] + (hat->bmax[1] - hat->bmin[1]) * 0.25f);
	}
}

void FxPanthersBeanieOpenGL::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	auto ZRotation = hat->renderParams.rotationMatrix.block<1, 3>(2, 0).data();

	hat->objectRenderParams[face.pointId][0]->additionalUniforms["ZRotation"] =
		TUniform3f(Eigen::Vector3f(ZRotation[0], ZRotation[1], ZRotation[2]));
}