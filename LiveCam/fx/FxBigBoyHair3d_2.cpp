#include <fx/FxBigBoyHair3d_2.h>

extern GLuint cubemapTexId;

FxBigBoyHair3d_2::FxBigBoyHair3d_2()
{
	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/extendedVertexShader.txt", "./assets/shaders/fragment/special/bigboyHair_FragmentShader.txt");

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/bigboy/models/BigBoy_Hair.obj";
	headModel->texturesPaths.fill( { "./assets/fx/bigboy/images/BBoy_HairUV.jpg" });
	headModel->name = "hair";
	headModel->modelShift = { 0, 0.56f, 0 };
	headModel->modelScale = 1.37f;

	auto renderParams = make_shared<ObjectRenderParams>();

	renderParams->shader = shader;
	renderParams->cameraPos = { 0, 0, 0, 1 };
	renderParams->lightPos = { 0.2f, 1, 0.4f, 0 };
	renderParams->ambientLight = { 0.3f, 0.3f, 0.3f };
	renderParams->normalMap = "./assets/fx/bigboy/images/BBoy_HairUV_nml.2.jpg";

	headModel->objectRenderParams.fill( { renderParams });

	models.push_back(headModel);

	cubemapID = cubemapTexId;
}

FxBigBoyHair3d_2::~FxBigBoyHair3d_2()
{

}