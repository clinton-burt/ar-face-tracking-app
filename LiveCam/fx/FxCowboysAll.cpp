#include "fx/FxCowboysAll.h"

FxCowboysAll::FxCowboysAll()
{
	loadFromJSON("./assets/fx/cowboys/cowboys_all_filters_modules.json");

	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto depthMask = make_shared<DepthMask>(0, true, true);
	depthMask->canSwapSuit = true;

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/cowboys/rowdy/Cowboys_RowdyMascot_v003.obj";
	model->texturesPaths.fill({ "./assets/fx/cowboys/rowdy/tex/hat.1Surface_Color_v3.png" });
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
	
	model->objectRenderParams.fill({ skinRenderParams });

	//------------------------------------------------------------------------
	auto shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto dogEarModel = make_shared<OpenGL3DModel>();
	dogEarModel->modelPath = "./assets/fx/cowboys/dog/dog_ears/Cowboys_DogEars_v3.obj";
	dogEarModel->texturesPaths.fill({ "./assets/fx/cowboys/dog/dog_ears/tex/cowboysDog_ears.png" });
	dogEarModel->name = "ears";
	dogEarModel->modelShift = { 0, 0.25f, 0 };
	dogEarModel->modelScale = 2.41f;

	auto tongue = make_shared<OpenGLAnimatedEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/cowboys/dog/tongue/");
	tongue->animationScale.fill({ 0.5f, 0.5f });
	tongue->animationOpened.fill(74);
	//tongue->animationLength.fill(90);

	auto earsRenderParams = make_shared<ObjectRenderParams>();
	earsRenderParams->lightPos = { 0.2f, 0.0f, 1, 0 };
	earsRenderParams->cameraPos = { 0, 0, 0, 1 };
	earsRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	earsRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	earsRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	earsRenderParams->specularPower = 50;
	earsRenderParams->shader = shader3D;
	earsRenderParams->depthTest = true;
	earsRenderParams->blend = true;
	
	dogEarModel->objectRenderParams.fill({ earsRenderParams });
	tongue->objectRenderParams.fill({ tongue->createDefaultObjectRenderParams() });

	//--------------------------------------------------------------------------------------
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");
	auto faceMaskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto maskChinShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");
	auto snapsShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");

	auto helmet = make_shared<OpenGL3DModel>();
	helmet->name = "Cowboys_3DHelmet";
	helmet->modelPath = "./assets/fx/cowboys/helmet/Cowboys_3DHelmet_v004.obj";
	helmet->texturesPaths.fill({
		"./assets/fx/cowboys/helmet/tex/Cowboys_FootballHelmet_ColorMap_v002.png"
	});
	helmet->modelShift = { 0, 0.15f, 0 };
	helmet->modelScale = 1.7f;

	auto helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->lightPos = { 0, 1, 0, 0 };
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->specularLight = { 1, 1, 1 };
	helmetRenderParams->specularPower = 20;
	helmetRenderParams->reflectionRatio = 0.25;
	helmetRenderParams->shader = helmetShader;
	helmetRenderParams->cullFace = false;

	auto maskRenderParams = make_shared<ObjectRenderParams>();
	maskRenderParams->lightPos = { 0, 0, 1, 0 };
	maskRenderParams->cameraPos = { 0, 0, 0, 1 };
	maskRenderParams->ambientLight = { 0, 0, 0 };
	maskRenderParams->diffuseLight = { 0, 0, 0 };
	maskRenderParams->specularLight = { 1, 1, 1 };
	maskRenderParams->specularPower = 1;
	maskRenderParams->materialColor = { 0, 0, 0, 1 };
	maskRenderParams->shader = faceMaskShader;
	maskRenderParams->cullFace = false;

	auto chinRenderParams = make_shared<ObjectRenderParams>();
	chinRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	chinRenderParams->cameraPos = { 0, 0, 0, 1 };
	chinRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	chinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	chinRenderParams->specularLight = { 0, 0, 0 };
	chinRenderParams->specularPower = 1;
	chinRenderParams->materialColor = { 0, 0, 0, 1 };
	chinRenderParams->shader = maskChinShader;
	chinRenderParams->cullFace = false;

	auto snapsRenderParams = make_shared<ObjectRenderParams>();
	snapsRenderParams->lightPos = { 0, 1, 0.25, 0 };
	snapsRenderParams->cameraPos = { 0, 0, 0, 1 };
	snapsRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	snapsRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	snapsRenderParams->specularLight = { 0, 0, 0 };
	snapsRenderParams->specularPower = 10;
	snapsRenderParams->materialColor = { 0.745f, 0.745f, 0.745f, 1 };
	snapsRenderParams->shader = snapsShader;
	snapsRenderParams->cullFace = false;

	helmet->objectRenderParams.fill({
		helmetRenderParams,
		helmetRenderParams,
		maskRenderParams,
		chinRenderParams,
		snapsRenderParams
	});

	cubemapTextures =
	{
		"./assets/cubemaps/stadium/posx.jpg",
		"./assets/cubemaps/stadium/negx.jpg",
		"./assets/cubemaps/stadium/posy.jpg",
		"./assets/cubemaps/stadium/negy.jpg",
		"./assets/cubemaps/stadium/posz.jpg",
		"./assets/cubemaps/stadium/negz.jpg"
	};

	//------------------------------------------------------------------------------------------
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");
	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";
	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;
	facePaint->objectRenderParams.fill({ facePaintRenderParams });
	
	models.push_back(depthMask);

	models.push_back(model); //1
	models.push_back(dogEarModel); //2
	models.push_back(tongue); //3
	models.push_back(helmet); //4
	models.push_back(facePaint); //5

	for (int i = 0; i < 5; ++i)
	{
		applyModule(&filterModules[0], i, false);
	}
}

FxCowboysAll::~FxCowboysAll()
{
}