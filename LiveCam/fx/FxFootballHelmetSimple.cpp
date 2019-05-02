#include <fx/FxFootballHelmetSimple.h>

FxFootballHelmetSimple::FxFootballHelmetSimple()
{
	loadFromJSON("./assets/fx_config/filter-modules/football-helmet-module-eagles.json");

	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");
	auto faceMaskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/helmet/helmet_7.obj";
	model->texturesPaths.fill({
		"./assets/fx/nfl_teams/textures/browns/Helmet_V5_HelmetStripe_UVMap.jpg",
		"./assets/fx/nfl_teams/textures/browns/Helmet_V5_FaceMask_UVMap.jpg",
		"./assets/fx/nfl_teams/textures/browns/Helmet_V5_Helmet_UVMap.jpg"
	});
	model->name = "helmet";
	model->extraShift = { 0, 0, 10 };
	model->extraScale = 0.24f;

	auto depthMask = make_shared<DepthMask>(3, true, true);

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
	maskRenderParams->lightPos = { 0, 1, 1, 0 };
	maskRenderParams->cameraPos = { 0, 0, 0, 1 };
	maskRenderParams->ambientLight = { 0, 0, 0 };
	maskRenderParams->diffuseLight = { 0, 0, 0 };
	maskRenderParams->specularLight = { 1, 1, 1 };
	maskRenderParams->specularPower = 2;
	maskRenderParams->shader = faceMaskShader;
	maskRenderParams->cullFace = false;

	model->objectRenderParams.fill({ helmetRenderParams, maskRenderParams, helmetRenderParams });

	models.push_back(depthMask);
	models.push_back(model);

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

FxFootballHelmetSimple::~FxFootballHelmetSimple()
{

}