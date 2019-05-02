#include <fx/FxLightningBee.h>

#include <stdlib.h>

FxLightningBee::FxLightningBee()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	helmet = make_shared<OpenGL3DModel>();
	
	helmet->modelPath = "./assets/fx/lightning/bee/bee.obj";
	helmet->modelShift = { 0, 0.3f, 0.3f };
	helmet->modelScale = 2.2f;
	//helmet->modelScale = 4.0f;

	helmet->texturesPaths.fill({
		"./assets/fx/lightning/bee/hairs_diffuse.jpg",
		"./assets/fx/lightning/bee/HeadSurface_Color.png"
	});
	helmet->name = "helmet";

	animationIndex = 0;

	helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->lightPos = { 1, 1, 1, 0 };
	helmetRenderParams->ambientLight = { 0.5f, 0.5f, 0.5f };
	helmetRenderParams->diffuseLight = { 0.5f, 0.5f, 0.5f };
	helmetRenderParams->specularLight = { 0, 0, 0 };
	helmetRenderParams->specularPower = 1;
	helmetRenderParams->shader = helmetShader;

	helmet->objectRenderParams.fill( { helmetRenderParams });

	//helmet->transformVersion = TransformVersion::V1;

	models.push_back(helmet);

	boltSprite.animationPath = "./assets/fx/lightning/bee/lightning/";
	boltSprite.animationIndex = 0;
	//boltSprite.animationLength = 13;
	boltSprite.shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/spriteVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");
}

FxLightningBee::~FxLightningBee()
{

}

void FxLightningBee::load()
{
	FxCore::load();

	boltSprite.load();
}

void FxLightningBee::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);
	
	animationIndex += 1;

	if (animationIndex == boltSprite.animationLength)
	{
		animationIndex = 0;
	}
}

void FxLightningBee::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::draw(face, externalRenderParams);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	//glEnable(GL_ALPHA_TEST);
	//glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	Matrix4f transform;

	float shiftY = (helmet->bmax[1] - helmet->bmin[1]) * 0.75f;
	float shiftZ = (helmet->bmax[2] - helmet->bmin[2]) * 0.15f;
	transform = Affine3f(Translation3f(Vector3f(0, shiftY, shiftZ))).matrix();

	float scale = (helmet->bmax[0] - helmet->bmin[0]) * 1.1f;
	transform *= Affine3f(Scaling(Vector3f(scale, scale, 1))).matrix();

	Matrix4f result = helmet->renderParams.modelView * transform;

	boltSprite.animationIndex = animationIndex;
	boltSprite.draw(result, helmet->renderParams.projection);

	Matrix4f mirror = Matrix4f::Identity();
	mirror(0, 0) = -1;

	transform *= mirror;

	result = helmet->renderParams.modelView * transform;

	boltSprite.animationIndex = animationIndex;
	boltSprite.draw(result, helmet->renderParams.projection);

	glPopAttrib();
}

void FxLightningBee::unload()
{
	FxCore::unload();

	boltSprite.unload();
}