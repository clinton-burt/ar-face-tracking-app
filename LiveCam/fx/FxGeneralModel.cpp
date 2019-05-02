#include <fx/FxGeneralModel.h>


FxGeneralModel::FxGeneralModel()
{
	loadFromJSON("./assets/fx_config/models/falcons/falcons-party-hat.json");

	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = filterModules[0].suits[0].get<std::string>("modelPath", "");
	//model->modelPath = "./assets/fx/AngelCrab/AngelCrab_Model_v001.obj";
	//model->texturesPaths.fill( { "./assets/fx/AngelCrab/tex/AngelCrab_Color_1.png" });
	std::vector<std::string> texturesPaths;
	auto modelTextures = filterModules[0].suits[0].get_child_optional("modelTextures");
	if (modelTextures)
	{
		for (auto &texture : modelTextures.get())
		{
			texturesPaths.push_back(texture.second.get_value<std::string>());
		}
	}

	model->texturesPaths.fill({
		texturesPaths[0]
	});

	model->name = "FxGeneralModel";
	
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

	model->objectRenderParams.fill( { skinRenderParams, skinRenderParams, teethRenderParams });

	models.push_back(model);
}

FxGeneralModel::~FxGeneralModel()
{

}