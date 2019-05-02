#include <fx/FxHipsterBeard.h>

#include <opencv\cv.h>

FxHipsterBeard::FxHipsterBeard()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/normalMapHalfSpacePhongFragmentShader.txt");

	auto model = make_shared<LipsJoint3DModel>();
	model->modelPath = "./assets/fx/jets/hipsterBeard/HipsterBeard_v001.obj";
	model->texturesPaths.fill( { "./assets/fx/jets/hipsterBeard/tex/hipsterBeard_smoothed.png" } );
	model->name = "hipsterBeard";
	model->modelShift = { 0, -0.4f, 0 };
	model->modelScale = 1.5f;

	auto beardRenderParams = make_shared<ObjectRenderParams>();
	beardRenderParams->shader = shader;
	beardRenderParams->lightPos = { 0, 0, 1, 0 };
	beardRenderParams->cameraPos = { 0, 0, 0, 1 };
	beardRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	beardRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	beardRenderParams->specularLight = { 0.2f, 0.2f, 0.2f };
	beardRenderParams->specularPower = 10;
	beardRenderParams->blend = true;
	beardRenderParams->depthTest = false;
	beardRenderParams->normalMap = "./assets/fx/jets/hipsterBeard/tex/normals.png";

	model->objectRenderParams.fill( { beardRenderParams } );

	/*for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		model->renderParams.faceXCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceYCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceWidthSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.yawSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.pitchSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.rollSmoother[i].ACTIVITY_THRESHOLD = 0;
	}*/

	models.push_back(model);
}

FxHipsterBeard::~FxHipsterBeard()
{

}