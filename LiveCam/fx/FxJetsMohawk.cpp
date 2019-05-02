#include <fx/FxJetsMohawk.h>

FxJetsMohawk::FxJetsMohawk()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/normalMapHalfSpacePhongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/jets/jetsMohawk/Jets_mohawk_NoHeadBand.obj";
	model->texturesPaths.fill( { "./assets/fx/jets/jetsMohawk/tex/vgh_ps_hair2_smoothed.png" } );
	model->name = "jetsMohawk";
	model->modelShift = { 0, 0.65f, -0.12f };
	model->modelScale = 1;
	
	auto depthMask = make_shared<DepthMask>(0, false, true);
	depthMask->modelScale = 2.05f;

	auto mohawkRenderParams = make_shared<ObjectRenderParams>();
	mohawkRenderParams->shader = shader;
	mohawkRenderParams->lightPos = { 0, 0, 1, 0 };
	mohawkRenderParams->cameraPos = { 0, 0, 0, 1 };
	mohawkRenderParams->ambientLight = { 1, 1, 1 };
	mohawkRenderParams->diffuseLight = { 0, 0, 0 };
	mohawkRenderParams->specularLight = { 0.3f, 0.3f, 0.3f };
	mohawkRenderParams->specularPower = 10;
	mohawkRenderParams->blend = true;
	mohawkRenderParams->depthMask = false;
	mohawkRenderParams->normalMap = "./assets/fx/jets/jetsMohawk/tex/normals.jpg";

	model->objectRenderParams.fill( { mohawkRenderParams } );

	/*
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		model->renderParams.faceXCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceYCenterSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.faceWidthSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.yawSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.pitchSmoother[i].ACTIVITY_THRESHOLD = 0;
		model->renderParams.rollSmoother[i].ACTIVITY_THRESHOLD = 0;
	}*/

	models.push_back(depthMask);
	models.push_back(model);
}

FxJetsMohawk::~FxJetsMohawk()
{

}