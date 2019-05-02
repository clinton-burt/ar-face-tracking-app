#include "fx/FxGumboMascotTongue.h"


FxGumboMascotTongue::FxGumboMascotTongue()
{
	//shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/normalMapHalfSpacePhongFragmentShader.txt");
//	shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/invertingNormalsPhongFragmentShader.txt");
	shader3Dtongue = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/special/newOrleansPelicans_dogMoveTongueVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto dogHeadModel = make_shared<OpenGL3DModel>();
	dogHeadModel->modelPath = "./assets/fx/new_orleans_pelicans/gumboMascotTongue/models/Gumbo_mascot.obj";
	dogHeadModel->texturesPaths.fill({ "./assets/fx/new_orleans_pelicans/gumboMascotTongue/texture/ears_LP_dog_mascot_Base_Color.png" });
	dogHeadModel->name = "head";
	dogHeadModel->modelScale = 3.0f;
	dogHeadModel->modelShift = { 0, 0, 0 };

	dogHeadModel->extraRotateMatrix(0, 0) = -1;
	dogHeadModel->extraRotateMatrix(0, 1) = 0;
	dogHeadModel->extraRotateMatrix(0, 2) = 0;
	dogHeadModel->extraRotateMatrix(1, 0) = 0;
	dogHeadModel->extraRotateMatrix(1, 1) = 1;
	dogHeadModel->extraRotateMatrix(1, 2) = 0;
	dogHeadModel->extraRotateMatrix(2, 0) = -0;
	dogHeadModel->extraRotateMatrix(2, 1) = 0;
	dogHeadModel->extraRotateMatrix(2, 2) = -1;
	
	dogTongueModel = make_shared<OpenGL3DModel>();
	dogTongueModel->modelPath = "./assets/fx/new_orleans_pelicans/gumboMascotTongue/models/Gumbo_mascot_tongue.obj";
	dogTongueModel->texturesPaths.fill({ "./assets/fx/new_orleans_pelicans/gumboMascotTongue/texture/ears_LP_dog_mascot_Base_Color.png" });
	dogTongueModel->name = "tongue";
	dogTongueModel->modelScale = 1.0f;
	dogTongueModel->modelShift = { -0.45f, -1.95f, 1.25f };
	
	dogTongueModel->extraRotateMatrix(0, 0) = -1;
	dogTongueModel->extraRotateMatrix(0, 1) = 0;
	dogTongueModel->extraRotateMatrix(0, 2) = 0;
	dogTongueModel->extraRotateMatrix(1, 0) = 0;
	dogTongueModel->extraRotateMatrix(1, 1) = 1;
	dogTongueModel->extraRotateMatrix(1, 2) = 0;
	dogTongueModel->extraRotateMatrix(2, 0) = -0;
	dogTongueModel->extraRotateMatrix(2, 1) = 0;
	dogTongueModel->extraRotateMatrix(2, 2) = -1;

	auto headRenderParams = make_shared<ObjectRenderParams>();
	headRenderParams->lightPos = { 0.0f, 1, 1.0f, 0 };
	headRenderParams->cameraPos = { 0, 0, 0, 1 };
	headRenderParams->ambientLight = { 0.5f,  0.5f, 0.5f };
	headRenderParams->diffuseLight = { 0.5f,  0.5f, 0.5f };
	headRenderParams->specularLight = { 0.1f, 0.1f, 0.1f };
	headRenderParams->specularPower = 1;
	headRenderParams->shader = shader3D;
	headRenderParams->normalMap = "./assets/fx/new_orleans_pelicans/gumboMascotTongue/texture/ears_LP_dog_mascot_Normal_DirectX.png";

	/* before
	auto headRenderParams = make_shared<ObjectRenderParams>();
	headRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	headRenderParams->cameraPos = { 0, 0, 0, 1 };
	headRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	headRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	headRenderParams->specularLight = { 0, 0, 0 };
	headRenderParams->specularPower = 1;
	headRenderParams->shader = shader3D;
	headRenderParams->normalMap = "./assets/fx/new_orleans_pelicans/gumboMascotTongue/textures/ears_LP_dog_mascot_Normal_DirectX.png";
	*/

	auto tongueRenderParams = make_shared<ObjectRenderParams>();
	tongueRenderParams->lightPos = { 0.2f, 0.4f, 1, 0 };
	tongueRenderParams->cameraPos = { 0, 0, 0, 1 };
	tongueRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	tongueRenderParams->diffuseLight = { 0.4f, 0.4f, 0.4f };
	tongueRenderParams->specularLight = { 0, 0, 0 };
	tongueRenderParams->specularPower = 1;
	tongueRenderParams->shader = shader3Dtongue;

	dogHeadModel->objectRenderParams.fill({ headRenderParams });
	dogTongueModel->objectRenderParams.fill({ tongueRenderParams });
	models.push_back(dogHeadModel);
	models.push_back(dogTongueModel);

	timeStart = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
}

FxGumboMascotTongue::~FxGumboMascotTongue()
{

}

void FxGumboMascotTongue::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{

	FxCore::transform(face, externalRenderParams);

	timeEnd = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
	timeInterval = boost::lexical_cast<long long int>((timeEnd - timeStart).count());
	countFrame = timeInterval / dt;
	timeCount = countFrame % 15;	
	dogTongueModel->objectRenderParams[face.pointId][0]->additionalUniforms["NewPosTangMatrix"]
		= TUniform3f(Vector3f(tongueX[timeCount] * 0.3, tongueY[timeCount] * 0.3, tongueZ[timeCount] * 0.05));
}

void FxGumboMascotTongue::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::draw(face, externalRenderParams);
}