#include "fx/FxPanthersWig.h"

FxPanthersWig::FxPanthersWig()
{
	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto wig = make_shared<OpenGL3DModel>();
	wig->modelPath = "./assets/fx/panthers/panthersWig/NewWig4_004.obj";
	wig->texturesPaths.fill( { "./assets/fx/panthers/panthersWig/WigSurface_Color.png" } );
	wig->name = "wig";
	wig->modelScale = 1.7f;
	wig->modelShift = { 0, 0.18f, 0 };
	
	auto glasses = make_shared<OpenGL3DModel>();
	glasses->modelPath = "./assets/fx/panthers/panthersWig/PantherGlasses.obj";
	glasses->texturesPaths. fill( { "./assets/fx/panthers/panthersWig/GlassesSurface_Color.png" });
	glasses->name = "glasses";
	glasses->modelShift = { 0, 0, 0.28f };

	auto smile = make_shared<OpenGLEmojiModel>();
	smile->name = "smile";
	smile->emojisTextures.fill( { "./assets/fx/panthers/panthersWig/smile.png" } );
	smile->emojiRotation.fill(true);
	smile->emojisOffset.fill({ 0, 0, -0.02f });
	smile->emojisScale.fill({ 2, 2 });

	auto depthMask = make_shared<DepthMask>(-0.04, true, true);

	auto wigRenderParams = make_shared<ObjectRenderParams>();
	wigRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	wigRenderParams->cameraPos = { 0, 0, 0, 1 };
	wigRenderParams->ambientLight = { 2, 2, 2 };
	wigRenderParams->diffuseLight = { 0, 0, 0 };
	wigRenderParams->specularLight = { 0, 0, 0 };
	wigRenderParams->specularPower = 1;
	wigRenderParams->shader = shader3D;

	auto glassesRenderParams = make_shared<ObjectRenderParams>();
	glassesRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	glassesRenderParams->cameraPos = { 0, 0, 0, 1 };
	glassesRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	glassesRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	glassesRenderParams->specularLight = { 0.3f, 0.3f, 0.3f };
	glassesRenderParams->specularPower = 5;
	glassesRenderParams->shader = shader3D;

	wig->objectRenderParams.fill( { wigRenderParams } );
	glasses->objectRenderParams.fill( { glassesRenderParams } );
	smile->objectRenderParams.fill({ smile->createDefaultObjectRenderParams() });

	models.push_back(depthMask);
	models.push_back(wig);
	models.push_back(smile);
	models.push_back(glasses);
}

FxPanthersWig::~FxPanthersWig()
{
}