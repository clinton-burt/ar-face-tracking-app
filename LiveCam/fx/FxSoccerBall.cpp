#include <fx/FxSoccerBall.h>


FxSoccerBall::FxSoccerBall()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto model = make_shared<OpenGL3DModel>();
	model->modelPath = "./assets/fx/nyc_football_club/soccerBall/SunglassesAndBall.obj";
	model->texturesPaths.fill( { "./assets/fx/nyc_football_club/soccerBall/SunglassesAndBallSurface_Color.png" } );
	model->name = "soccerBallHead";
	model->modelScale = 2.17f;

	auto ballRenderParams = make_shared<ObjectRenderParams>();
	ballRenderParams->shader = shader;
	ballRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	ballRenderParams->cameraPos = { 0, 0, 0, 1 };
	ballRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	ballRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	ballRenderParams->specularLight = { 0, 0, 0 };
	ballRenderParams->specularPower = 1;

	model->objectRenderParams.fill( { ballRenderParams } );

	models.push_back(model);
}

FxSoccerBall::~FxSoccerBall()
{

}