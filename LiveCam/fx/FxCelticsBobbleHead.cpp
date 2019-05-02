#include <fx/FxCelticsBobbleHead.h>

FxCelticsBobbleHead::FxCelticsBobbleHead()
{
	animationCounter.fill(0);
	animationDirection.fill(false);

	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/advancedMaterialFragmentShader.txt");

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/bostonCeltics/bobblehead/celtics.obj";
	headModel->texturesPaths.fill({"./assets/fx/bostonCeltics/bobblehead/celtics_defaultMat_BaseColor.png"});
	headModel->name = "bobblehead";
	headModel->modelScale = 2.0f;
	headModel->modelShift = { 0, 0, 0.15f };

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->normalMap = "./assets/fx/bostonCeltics/bobblehead/celtics_defaultMat_Normal.png";
	hatRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.5f, 0.5f, 0.5f };
	hatRenderParams->diffuseLight = { 0.5f, 0.5f, 0.5f };
	hatRenderParams->specularLight = { 0.15f, 0.15f, 0.15f };
	hatRenderParams->specularPower = 5;
	hatRenderParams->shader = shader;
	
	headModel->objectRenderParams.fill({ hatRenderParams });

	models.push_back(headModel);
}

FxCelticsBobbleHead::~FxCelticsBobbleHead()
{

}

void FxCelticsBobbleHead::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	animateNextStep(face.pointId);

	models[0]->renderParams.modelView = Affine3f(Translation3f(0, 0.0035*(animationCounter[face.pointId]), 0)).matrix() * models[0]->renderParams.modelView;
}

void FxCelticsBobbleHead::animateNextStep(size_t i)
{
	if (animationDirection[i]) {

		animationCounter[i]++;

		if (animationCounter[i] == 8)
		{
			animationDirection[i] = false;
		}
	}
	else {

		animationCounter[i]--;

		if (animationCounter[i] == (-8))
		{
			animationDirection[i] = true;
		}
	}
}