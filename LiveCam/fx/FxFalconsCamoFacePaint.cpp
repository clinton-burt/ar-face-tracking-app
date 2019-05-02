#include <fx/FxFalconsCamoFacePaint.h>

FxFalconsCamoFacePaint::FxFalconsCamoFacePaint()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingFragmentShader.txt");

	auto facePaint = make_shared<OpenGLFacePaintModel>();
	facePaint->name = "paint 2D model";

	auto facePaintRenderParams = make_shared<ObjectRenderParams>();
	facePaintRenderParams->shader = facepaintShader;

	facePaint->objectRenderParams.fill({ facePaintRenderParams });

	facePaint->halfFaceParams.fill({
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(0, 0, 0, 0),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(1, 1, 1, 1),
		Eigen::Vector4f(0, 0, 0, 0),
		"./assets/fx/falcons/facepaint/CamoFacePaintV3b.png",
		"./assets/fx/falcons/facepaint/CamoFacePaintV3b.png",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxFalconsCamoFacePaint::~FxFalconsCamoFacePaint()
{
}