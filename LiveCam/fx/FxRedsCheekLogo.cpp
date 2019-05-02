#include <fx/FxRedsCheekLogo.h>

FxRedsCheekLogo::FxRedsCheekLogo()
{
	auto facepaintShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureEdgeSmoothingVertexShader.txt", "./assets/shaders/fragment/orthoColorTextureEdgeSmoothingOldFragmentShader.txt");

	auto facePaint = make_shared<old::OpenGLFacePaintModel>();
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
		"./assets/fx/reds/images/reds_face_paint_right.png",
		"./assets/fx/reds/images/reds_face_paint_left.png",
		"",
		0,
		""
	});

	models.push_back(facePaint);
}

FxRedsCheekLogo::~FxRedsCheekLogo()
{
}