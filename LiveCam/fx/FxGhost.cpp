#include <fx/FxGhost.h>


FxGhost::FxGhost()
{
	ghostShader = shaderManagerWrapper.LoadFromFile(
		"./assets/shaders/vertex/simpleVertexShader.txt",
		"./assets/fx/halloween/ghost/ghost-fragmentShader.txt");

	ghost = make_shared<OpenGL3DModel>();
	ghost->modelPath = "./assets/fx/halloween/ghost/ghost_LP.OBJ";
	ghost->name = "Ghost";
	ghost->modelScale = 3.37;
	ghost->modelShift = { -0.07f, -0.26f, -0.13f };
	ghost->texturesPaths.fill({ "./assets/fx/halloween/ghost/texture/energy3.jpg" });

	auto ghostRenderParams = make_shared<ObjectRenderParams>();
	ghostRenderParams->shader = ghostShader;
	ghostRenderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	ghostRenderParams->cameraPos = { 0, 0, 0, 1 };
	ghostRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	ghostRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	ghostRenderParams->specularLight = { 0, 0, 0 };
	ghostRenderParams->specularPower = 1;
	ghostRenderParams->blend = true;

	ghost->objectRenderParams.fill( { ghostRenderParams } );

	auto depthMask = make_shared<DepthMask>(-0.1, true, true);

	models.push_back(depthMask);
	models.push_back(ghost);
	
	wavesOffset = 0;

	shineFactor = 0.5;
	shineIncreasing = true;
}

FxGhost::~FxGhost()
{

}

void FxGhost::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);


	shineFactor += shineIncreasing ? 0.02f : -0.02f;

	if (shineFactor > 2.5)
	{
		shineIncreasing = false;
	}

	if (shineFactor < 1)
	{
		shineIncreasing = true;
	}

	wavesOffset += 0.01;
	if (wavesOffset > 1) wavesOffset = 0;

	ghost->objectRenderParams[face.pointId][0]->additionalUniforms["wavesOffset"] = TUniform1f(wavesOffset);
	ghost->objectRenderParams[face.pointId][0]->additionalUniforms["shineFactor"] = TUniform1f(shineFactor);
}

void FxGhost::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::draw(face, externalRenderParams);
}