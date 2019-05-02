#include <fx/FxCowboysHelmet.h>
#include "utils/stb_image.h"

FxCowboysHelmet::FxCowboysHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");
	auto faceMaskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto maskChinShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");
	auto snapsShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->name = "Cowboys_3DHelmet";
	model->modelPath = "./assets/fx/cowboys/helmet/Cowboys_3DHelmet_v004.obj";
	model->texturesPaths.fill( {
		"./assets/fx/cowboys/helmet/tex/Cowboys_FootballHelmet_ColorMap_v002.png"
	});
	model->modelShift = { 0, 0.12f, 0 };
	model->modelScale = 1.7f;

	auto depthMask = make_shared<DepthMask>(-0.06, true, true);

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

	model->objectRenderParams.fill( {
		helmetRenderParams,
		helmetRenderParams,
		maskRenderParams,
		chinRenderParams,
		snapsRenderParams
	} );

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

FxCowboysHelmet::~FxCowboysHelmet()
{

}