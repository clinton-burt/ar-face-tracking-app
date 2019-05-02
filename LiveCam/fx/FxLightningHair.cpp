#include <fx/FxLightningHair.h>

#include <stdlib.h>

FxLightningHair::FxLightningHair()
{
	auto helmetShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");
	auto depthMask = make_shared<DepthMask>(0, true, true);

	helmet = make_shared<OpenGL3DModel>();
	
	helmet->modelPath = "./assets/fx/lightning/hair/logo_hair_2.obj";
	helmet->modelShift = { 0, 0.5f, 0.2f };
	helmet->modelScale = 4.f;

	helmet->texturesPaths.fill( {"./assets/fx/lightning/hair/hair_UVs_tetxure.png", "./assets/fx/lightning/hair/logo_texture/logo_Base_Color.png" });
	helmet->name = "helmet";

	helmetRenderParams = make_shared<ObjectRenderParams>();
	helmetRenderParams->cameraPos = { 0, 0, 0, 1 };
	helmetRenderParams->lightPos = { 1, 0, 0, 0 };
	helmetRenderParams->ambientLight = { 1, 1, 1 };
	helmetRenderParams->diffuseLight = { 0, 0, 0 };
	helmetRenderParams->specularLight = { 0, 0, 0 };
	helmetRenderParams->specularPower = 1;
	helmetRenderParams->blend = true;
	helmetRenderParams->alphaTest = true;
	helmetRenderParams->cullFace = false;
	helmetRenderParams->shader = helmetShader;

	auto logoRenderParams = make_shared<ObjectRenderParams>();
	logoRenderParams->cameraPos = { 0, 0, 0, 1 };
	logoRenderParams->lightPos = { 0.05f, 0, 1, 0 };
	logoRenderParams->ambientLight = { 0.5f, 0.5f, 0.5f };
	logoRenderParams->diffuseLight = { 0.5f, 0.5f, 0.5f };
	logoRenderParams->specularLight = { 0.0706f, 0.8589f, 0.9725f };
	logoRenderParams->specularPower = 20;
	logoRenderParams->shader = helmetShader;

	helmet->objectRenderParams.fill( { helmetRenderParams, logoRenderParams });

	models.push_back(depthMask);
	models.push_back(helmet);
	
	

	boltSprite.animationPath = "./assets/fx/lightning/hair/lightning/";
	boltSprite.animationIndex = 0;
	boltSprite.animationLength = 13;
	boltSprite.shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/spriteVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");
	
	srand(time(NULL));
	for (int i = 0; i < CHARGES_COUNT; ++i)
	{
		animationStart[i] = rand() % MAX_RELOADING_TIME + 1;
		animationIndex[i] = -1;
	}

	lipsAnimationIndex = 0;
}

FxLightningHair::~FxLightningHair()
{

}

void FxLightningHair::onInputFrameResized()
{
	FxCore::onInputFrameResized();

	for (auto &smoother : lipsDistanceSmoother)
	{
		smoother.SNAP_MULTIPLIER_SIMPLE = 1.0 / 60 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
	}
}

void FxLightningHair::load()
{
	FxCore::load();

	boltSprite.load();
}

void FxLightningHair::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);
	
	for (int i = 0; i < CHARGES_COUNT; ++i)
	{
		if (animationStart[i] > 0)
		{
			--animationStart[i];
		}
		else
		{
			if (animationIndex[i] == -1)
			{
				float angle = ((rand() % 220) - 110) / 180.f * M_PI;
				float shift = (0.4f + (rand() % 60) / 100.f) * helmet->bmax[0];

				chargesCoords[i].first = angle;
				chargesCoords[i].second = shift;
			}

			animationIndex[i] += 3; // skipping animation frames to speed up lightings

			if (animationIndex[i] >= boltSprite.animationLength)
			{
				animationIndex[i] = -1;
				animationStart[i] = rand() % MAX_RELOADING_TIME + 1;
			}
		}
	}

	++lipsAnimationIndex;
	if (lipsAnimationIndex >= boltSprite.animationLength)
	{
		lipsAnimationIndex = 0;
	}
}

void FxLightningHair::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::draw(face, externalRenderParams);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	//glEnable(GL_ALPHA_TEST);
	//glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	Matrix4f spriteTransform;

	float scale = (helmet->bmax[0] - helmet->bmin[0]) * 0.2f;
	Matrix4f scaling = Affine3f(Scaling(Vector3f(scale, scale, 0.))).matrix();

	Matrix4f translation = Affine3f(Translation3f(Vector3f(0., (helmet->bmax[1] - helmet->bmin[1]) * 0.29f, 0.))).matrix();

	for (int i = 0; i < CHARGES_COUNT; ++i)
	{
		float angle = chargesCoords[i].first;
		float shift = chargesCoords[i].second;

		spriteTransform = helmet->renderParams.modelView;
		spriteTransform *= translation;
		spriteTransform *= Affine3f(AngleAxisf(angle, Vector3f(0, 0, 1))).matrix();
		spriteTransform *= Affine3f(Translation3f(Vector3f(0, shift, 0))).matrix();
		spriteTransform *= scaling;

		boltSprite.animationIndex = animationIndex[i];
		boltSprite.draw(spriteTransform, helmet->renderParams.projection);
	}

	float lipToLip = lipsDistanceSmoother[face.pointId].responsiveAnalogReadSimple(
		PointCalcUtil::distanceBetween(face.pts[61 * 2], face.pts[61 * 2 + 1], face.pts[64 * 2], face.pts[64 * 2 + 1]));

	const float STD_FACE_WIDTH = 0.172f;
	const float STD_LIPS_DISTANCE = 260;

	float factor = lipToLip / (face.faceWidth / STD_FACE_WIDTH * STD_LIPS_DISTANCE);
	scale = (helmet->bmax[0] - helmet->bmin[0]) * 0.3f * (factor > 0.2f ? factor : 0);

	translation = Affine3f(Translation3f(Vector3f(0, (helmet->bmax[1] - helmet->bmin[1]) * 0.19f, (helmet->bmax[2] - helmet->bmin[2]) * 0.5f))).matrix();

	scaling = Affine3f(Scaling(Vector3f(scale, scale, 1))).matrix();

	Matrix4f translationRadial = Affine3f(Translation3f(Vector3f(scale / 2, 0, 0))).matrix();

	for (int i = 0; i < 16; ++i)
	{
		spriteTransform = helmet->renderParams.modelView;
		spriteTransform *= translation;
		spriteTransform *= Affine3f(AngleAxisf(i * (3.1416f * 2 / 16), Vector3f(0, 0, 1))).matrix();
		spriteTransform *= translationRadial;
		spriteTransform *= scaling;

		boltSprite.animationIndex = lipsAnimationIndex;
		//boltSprite.draw(spriteTransform, helmet->renderParams.projection);
	}

	glPopAttrib();
}

void FxLightningHair::unload()
{
	FxCore::unload();

	boltSprite.unload();
}