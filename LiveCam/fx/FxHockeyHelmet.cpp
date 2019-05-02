#include <fx/FxHockeyHelmet.h>


FxHockeyHelmet::FxHockeyHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/hockey_helmet/hockey_helmet_filter_v07.obj";
	headModel->texturesPaths.fill( {"./assets/fx/hockey_helmet/hockey_helmetColor.png" });
	headModel->name = "helmet";
	headModel->modelShift = { 0, 0.27f, 0.1f };
	headModel->modelScale = 1.53f;

	auto helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->lightPos = { 0, 1, 0, 0 };
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->specularLight = { 1, 1, 1 };
	helmetRenderParams->specularPower = 20;
	helmetRenderParams->reflectionRatio = 0.25;
	helmetRenderParams->shader = helmetShader;

	headModel->objectRenderParams.fill( { helmetRenderParams });

	models.push_back(headModel);

	cubemapTextures =
	{
		"./assets/cubemaps/stadium/posx.jpg",
		"./assets/cubemaps/stadium/negx.jpg",
		"./assets/cubemaps/stadium/posy.jpg",
		"./assets/cubemaps/stadium/negy.jpg",
		"./assets/cubemaps/stadium/posz.jpg",
		"./assets/cubemaps/stadium/negz.jpg"
	};
}

FxHockeyHelmet::~FxHockeyHelmet()
{

}