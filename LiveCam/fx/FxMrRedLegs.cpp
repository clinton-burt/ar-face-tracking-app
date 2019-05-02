#include <fx/FxMrRedLegs.h>

FxMrRedLegs::FxMrRedLegs()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/advancedMaterialFragmentShader.txt");

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/reds/stash/models/Reds_Cap6.0.obj";
	headModel->texturesPaths.fill({ "./assets/fx/reds/stash/textures/Sphere.1Surface_Color.png" });
	headModel->name = "hat";
	headModel->modelScale = 1.43f;
	headModel->modelShift = { 0, 0.67f, 0.1f };

	auto hairModel = make_shared<LipsJoint3DModel>();
	hairModel->modelPath = "./assets/fx/reds/stash/models/Reds_Stache.obj";
	hairModel->texturesPaths.fill( { "./assets/fx/reds/stash/textures/StacheSurface_Color.png" } );
	hairModel->name = "stash";
	hairModel->modelScale = 0.91f;
	hairModel->modelShift = { 0, 0.06f, 0 };

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.5f, 0.5f, 0.5f };
	hatRenderParams->diffuseLight = { 0.5f, 0.5f, 0.5f };
	hatRenderParams->specularLight = { 0, 0, 0 };
	hatRenderParams->specularPower = 20;
	hatRenderParams->shader = shader;

	auto hairRenderParams = make_shared<ObjectRenderParams>();
	hairRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hairRenderParams->cameraPos = { 0, 0, 0, 1 };
	hairRenderParams->ambientLight = { 0.5f, 0.5f, 0.5f };
	hairRenderParams->diffuseLight = { 0.5f, 0.5f, 0.5f };
	hairRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	hairRenderParams->specularPower = 10;
	hairRenderParams->shader = shader;

	headModel->objectRenderParams.fill({ hatRenderParams });
	hairModel->objectRenderParams.fill({ hairRenderParams });

	models.push_back(hairModel);
	models.push_back(headModel);
}

FxMrRedLegs::~FxMrRedLegs()
{

}