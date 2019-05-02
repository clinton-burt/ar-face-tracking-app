#include <fx/FxPhilliesPhanatic.h>

FxPhilliesPhanatic::FxPhilliesPhanatic()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/phillies/phanatic_v1.obj";
	model->texturesPaths.fill({ 
		"./assets/fx/phillies/tex/PlushTexture_green.png",
		"./assets/fx/phillies/tex/PhilliesHat_ColorMap.png" ,
		"./assets/fx/phillies/tex/nose_ring.png",
		"./assets/fx/phillies/tex/vgh_ps_hair3.png",
		"./assets/fx/phillies/tex/eyes.png"
	});
	model->name = "Phanatic";
	model->modelScale = 1.8f;
	model->modelShift = { 0, 0, 0.31f };

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/phillies/animation/tongue_sequence/");
	tongue->animationOffset.fill({ 0, 0, 0.12f });
	//tongue->animationScale.fill({ 0.7f, 0.7f });
	tongue->animationOpened.fill(65);
	//tongue->animationLength.fill(90);

	/*auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "phanaticTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/phillies/animation/tongue_sequence/");
	tongue->animationOffset.fill({ 0, .5f, 0 });
	//tongue->animationScale.fill({ 0.6f, 0.6f });
	tongue->animationOpened.fill(65);
	tongue->animationLength.fill(91);*/

	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	skinRenderParams->specularPower = 10; 
	skinRenderParams->blend = true;
	skinRenderParams->shader = textureShader;

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 10;
	teethRenderParams->blend = true;
	teethRenderParams->shader = textureShader;

	model->objectRenderParams.fill({ skinRenderParams, skinRenderParams, teethRenderParams });
	tongue->objectRenderParams.fill({ tongue->createDefaultObjectRenderParams() });

	models.push_back(model);
	models.push_back(tongue);
}

FxPhilliesPhanatic::~FxPhilliesPhanatic()
{
}