#include <fx/FxFireHead.h>

FxFireHead::FxFireHead()
{
	//useFaceLightParams = true;
	//useFrameLightParams = true;
	//initialModulesRandomize = false;

	//auto atlasAnimationShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoAtlasFragmentShader.txt");

	//fire = make_shared<AtlasAnimatedEmojiModel>();
	//fire->name = "fire head";
	//fire->animationPaths.fill("./assets/fx/miamiHeat/fire-hairs/fire_4096x4096.png");
	//fire->animationOffset.fill({ 0, -1.5, -0.5 });
	//fire->animationScale.fill({ 6, 4 });
	//fire->animationBackward.fill(false);
	//fire->animationSpeed.fill(3);
	//fire->animationRolling.fill(false);

	//auto animationRenderParams = make_shared<ObjectRenderParams>();
	//animationRenderParams->blend = true;
	//animationRenderParams->alphaTest = true;
	//animationRenderParams->shader = atlasAnimationShader;

	//fire->objectRenderParams.fill({ animationRenderParams });

	//auto facePaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingFragmentShader.txt");

	//auto facePaint = make_shared<OpenGLFacePaintModel>();
	//facePaint->name = "facePaint";
	//facePaint->headTextureMap = true;

	//facePaint->halfFaceParams.fill({
	//	{ 0, 0, 0, 0 },
	//	{ 0, 0, 0, 0 },
	//	{ 0, 0, 0, 0 },
	//	{ 0, 0, 0, 0 },
	//	{ 0, 0, 0, 0 },
	//	{ 0, 0, 0, 0 },
	//	"./assets/fx/miamiHeat/facepaint/Face_diffuseV3.jpg",
	//	"./assets/fx/miamiHeat/facepaint/Face_diffuseV3.jpg",
	//	"",
	//	0,
	//	"./assets/alphaMasks/crude_paint_for_headTexMap.png",
	//	1
	//});

	//auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	//facePaintRenderParams->shader = facePaintShader;

	//facePaint->objectRenderParams.fill({ facePaintRenderParams });

	//models.push_back(fire);
	//models.push_back(facePaint);

	loadFromJSON("./assets/fx/miamiHeat/miami_heat_facepaint_with_fire.json");

	fire = (AtlasAnimatedEmojiModel*)models[2].get();
}

FxFireHead::~FxFireHead()
{
}

void FxFireHead::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	float angle = fire->renderParams.rollSmoother[face.pointId].smoothValue / 180 * M_PI - M_PI_2;
	float sine = sinf(angle);
	float cosine = cosf(angle);

	fire->animationOffset[face.pointId] = { -1.5f * cosine, 2 * sine, -0.6f };
	fire->animationScale[face.pointId][0] = 7 * (1 + 0.4f * abs(cosine));
}