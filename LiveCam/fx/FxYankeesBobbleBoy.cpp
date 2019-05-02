#include <fx/FxYankeesBobbleBoy.h>


FxYankeesBobbleBoy::FxYankeesBobbleBoy()
{
	animationCounter.fill(0);
	animationDirection.fill(false);

	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/yankees/bobble_boy/NYY_Bobblehead_v04.obj";
	model->texturesPaths.fill({ "./assets/fx/yankees/bobble_boy/tex/NYY-04_Filter-Material_Bobblehead-v02-02.png" });
	model->name = "BOBBLE	";
	model->modelScale = 1.72;

	auto skinRenderParams = make_shared<ObjectRenderParams>();
	skinRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	skinRenderParams->cameraPos = { 0, 0, 0, 1 };
	skinRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	skinRenderParams->specularLight = { 0.15f, 0.15f, 0.15f };
	skinRenderParams->specularPower = 5;
	skinRenderParams->reflectionRatio = 0;
	skinRenderParams->shader = textureShader;
	skinRenderParams->blend = true;
	skinRenderParams->depthTest = true;
	skinRenderParams->cullFace = false;

	auto teethRenderParams = make_shared<ObjectRenderParams>();
	teethRenderParams->lightPos = { 0.2f, -0.2f, 1, 0 };
	teethRenderParams->cameraPos = { 0, 0, 0, 1 };
	teethRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	teethRenderParams->diffuseLight = { 0.2f, 0.2f, 0.2f };
	teethRenderParams->specularLight = { 0.5f, 0.5f, 0.5f };
	teethRenderParams->specularPower = 15;
	teethRenderParams->shader = textureShader;
	teethRenderParams->blend = true;
	teethRenderParams->depthTest = true;
	teethRenderParams->cullFace = false;

	model->objectRenderParams.fill({ skinRenderParams });

	models.push_back(model);
}

FxYankeesBobbleBoy::~FxYankeesBobbleBoy()
{

}

void FxYankeesBobbleBoy::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	animateNextStep(face.pointId);

	models[0]->renderParams.modelView *= Affine3f(Translation3f(0, 0.4*(animationCounter[face.pointId]), 0)).matrix();
}

void FxYankeesBobbleBoy::animateNextStep(size_t i)
{
	if (animationDirection[i]) {

		animationCounter[i]++;

		if (animationCounter[i] == maxMovementBoy)
		{
			animationDirection[i] = false;
		}
	}
	else {

		animationCounter[i]--;

		if (animationCounter[i] == (-maxMovementBoy))
		{
			animationDirection[i] = true;
		}
	}
}