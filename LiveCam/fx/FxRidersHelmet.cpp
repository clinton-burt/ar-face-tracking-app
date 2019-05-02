#include <fx/FxRidersHelmet.h>

FxRidersHelmet::FxRidersHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");
	auto faceMaskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");

	auto depthMask = make_shared<DepthMask>(-0.1, true, true);

	auto helmet = make_shared<OpenGL3DModel>();
	helmet->modelPath = "./assets/fx/ridersHelmet/football_helmet_v01.obj";
	helmet->texturesPaths.fill( { "./assets/fx/ridersHelmet/helmet_v2.png" } );
	helmet->name = "ridersHelmet";
	helmet->modelScale = 1.6f;
	helmet->modelShift = { 0, 0.18f, 0 };

	auto helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->shader = helmetShader;
	helmetRenderParams->lightPos = { 0, 1, 0, 0 };
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->specularLight = { 1, 1, 1 };
	helmetRenderParams->specularPower = 20;
	helmetRenderParams->reflectionRatio = 0.25;
	helmetRenderParams->cullFace = false;

	auto maskRenderParams = make_shared<ObjectRenderParams>();
	maskRenderParams->shader = faceMaskShader;
	maskRenderParams->lightPos = { 0, 1, 0.5f, 0 };
	maskRenderParams->cameraPos = { 0, 0, 0, 1 };
	maskRenderParams->ambientLight = { 0, 0, 0 };
	maskRenderParams->diffuseLight = { 0, 0, 0 };
	maskRenderParams->specularLight = { 1, 1, 1 };
	maskRenderParams->specularPower = 10;
	maskRenderParams->materialColor = { 0, 0, 0, 1 };
	maskRenderParams->cullFace = false;

	helmet->objectRenderParams.fill( { maskRenderParams, helmetRenderParams  } );

	models.push_back(depthMask);
	models.push_back(helmet);

	cubemapTextures[0] = "./assets/cubemaps/stadium/posx.jpg";
	cubemapTextures[1] = "./assets/cubemaps/stadium/negx.jpg";
	cubemapTextures[2] = "./assets/cubemaps/stadium/posy.jpg";
	cubemapTextures[3] = "./assets/cubemaps/stadium/negy.jpg";
	cubemapTextures[4] = "./assets/cubemaps/stadium/posz.jpg";
	cubemapTextures[5] = "./assets/cubemaps/stadium/negz.jpg";
}

FxRidersHelmet::~FxRidersHelmet()
{

}