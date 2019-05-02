#include <fx/FxRamHorns.h>


FxRamHorns::FxRamHorns()
{
	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/la_Rams/rams_horns_v1.obj";
	model->texturesPaths.fill({ "./assets/fx/la_Rams/tex/Horns_color.jpg" });
	model->name = "Horns";

	auto depthMask = make_shared<DepthMask>(0, true, true);

	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	skinRenderParams->specularPower = 5;
	skinRenderParams->shader = textureShader;

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 10;
	teethRenderParams->shader = textureShader;

	model->objectRenderParams.fill({ skinRenderParams, skinRenderParams, teethRenderParams });

	models.push_back(depthMask);
	models.push_back(model);

	/*for (int i = 0; i < 2; ++i)
	{
		model->objectRenderParams[i].lightPos = { 0.2f, 0.4f, 1, 0 };
		model->objectRenderParams[i].cameraPos = { 0, 0, 0, 1 };
		model->objectRenderParams[i].ambientLight = { 0.6f, 0.6f, 0.6f };
		model->objectRenderParams[i].diffuseLight = { 0.6f, 0.6f, 0.6f };
		model->objectRenderParams[i].specularLight = { 0.5f, 0.5f, 0.5f };
		model->objectRenderParams[i].specularPower = 5;
		model->objectRenderParams[i].shader = textureShader;
	}

	model->objectRenderParams[2].lightPos = { 0.2f, -0.2f, 1, 0 };
	model->objectRenderParams[2].cameraPos = { 0, 0, 0, 1 };
	model->objectRenderParams[2].ambientLight = { 0.6f, 0.6f, 0.6f };
	model->objectRenderParams[2].diffuseLight = { 0.2f, 0.2f, 0.2f };
	model->objectRenderParams[2].specularLight = { 0.5f, 0.5f, 0.5f };
	model->objectRenderParams[2].specularPower = 10;
	model->objectRenderParams[2].shader = textureShader;

	models.push_back(model);*/
}

FxRamHorns::~FxRamHorns()
{

}

void FxRamHorns::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	models[0]->draw(face, externalRenderParams); // depthMask

	glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	models[1]->draw(face, externalRenderParams); // helmet

	glPopAttrib();
}