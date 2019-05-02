#include <fx/FxRamsHelmet.h>
#include "utils/stb_image.h"

FxRamsHelmet::FxRamsHelmet()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/cubemapPhongFragmentShader.txt");
	auto faceMaskShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto maskChinShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");
	auto snapsShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/noTextureVertexShader.txt", "./assets/shaders/fragment/noTexturePhongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/la_rams/rams_helmet_v1.obj";
	model->texturesPaths.fill( {"./assets/fx/la_rams/tex/Helmet_V5_FaceMask_UVMap.jpg"});
	model->texturesPaths.fill( {"./assets/fx/la_rams/tex/RamsHelmet.png" });
	model->name = "pantherHelmet";
	//model->extraScale = 0.8f;

	auto depthMask = make_shared<DepthMask>(0, true, true);


	auto stripeRenderParams= make_shared<ObjectRenderParams>();
	stripeRenderParams->lightPos = { 0, 1, 0, 0 };
	stripeRenderParams->cameraPos = { 0, 0, 0, 1 };
	stripeRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	stripeRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	stripeRenderParams->specularLight = { 1, 1, 1 };
	stripeRenderParams->specularPower = 20;
	stripeRenderParams->reflectionRatio = 0.25;
	stripeRenderParams->shader = helmetShader;

	auto helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->lightPos = { 0, 1, 0, 0 };
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	helmetRenderParams->specularLight = { 1, 1, 1 };
	helmetRenderParams->specularPower = 20;
	helmetRenderParams->reflectionRatio = 0.25;
	helmetRenderParams->shader = helmetShader;

	auto maskRenderParams = make_shared<ObjectRenderParams>();
	maskRenderParams->lightPos = { 0, 0, 1, 0 };
	maskRenderParams->cameraPos = { 0, 0, 0, 1 };
	maskRenderParams->ambientLight = { 0, 0, 0 };
	maskRenderParams->diffuseLight = { 0, 0, 0 };
	maskRenderParams->specularLight = { 1, 1, 1 };
	maskRenderParams->specularPower = 1;
	maskRenderParams->materialColor = { 0, 0, 0, 1 };
	maskRenderParams->shader = faceMaskShader;

	auto chinRenderParams = make_shared<ObjectRenderParams>();
	chinRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	chinRenderParams->cameraPos = { 0, 0, 0, 1 };
	chinRenderParams->ambientLight = { 0.4f, 0.4f, 0.4f };
	chinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	chinRenderParams->specularLight = { 0, 0, 0 };
	chinRenderParams->specularPower = 1;
	chinRenderParams->materialColor = { 0, 0, 0, 1 };
	chinRenderParams->shader = maskChinShader;

	auto snapsRenderParams = make_shared<ObjectRenderParams>();
	snapsRenderParams->lightPos = { 0, 1, 0.25, 0 };
	snapsRenderParams->cameraPos = { 0, 0, 0, 1 };
	snapsRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	snapsRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	snapsRenderParams->specularLight = { 0, 0, 0 };
	snapsRenderParams->specularPower = 10;
	snapsRenderParams->materialColor = { 0.745f, 0.745f, 0.745f, 1 };
	snapsRenderParams->shader = snapsShader;

	models.push_back(depthMask);
	models.push_back(model);

	model->objectRenderParams.fill( { stripeRenderParams, helmetRenderParams, maskRenderParams, chinRenderParams, snapsRenderParams } );

	cubemap =
	{
		"./assets/cubemaps/stadium/posx.jpg",
		"./assets/cubemaps/stadium/negx.jpg",
		"./assets/cubemaps/stadium/posy.jpg",
		"./assets/cubemaps/stadium/negy.jpg",
		"./assets/cubemaps/stadium/posz.jpg",
		"./assets/cubemaps/stadium/negz.jpg"
	};
}

FxRamsHelmet::~FxRamsHelmet()
{

}

void FxRamsHelmet::load()
{
	FxCore::load();

	cubemapID = resourceManager.loadCubemap(cubemap);
}

void FxRamsHelmet::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	models[0]->draw(face, externalRenderParams); // depthMask

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	models[1]->draw(face, externalRenderParams); // helmet

	glPopAttrib();
}