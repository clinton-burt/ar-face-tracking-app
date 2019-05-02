#include <fx/FxBravesTomahawkChop.h>

FxBravesTomahawkChop::FxBravesTomahawkChop()
{
	animationCounter.fill(0);
	animationDirection.fill(false);

	auto shader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/atlanta_braves/animated_tomahawk/BravesTomaHawk_V9.obj";
	//headModel->texturesPaths.fill({"./assets/fx/atlanta_braves/animated_tomahawk/BravesLogoUV2.png" });
	headModel->texturesPaths.fill({ "./assets/fx/atlanta_braves/animated_tomahawk/balance_cube.png", "./assets/fx/atlanta_braves/animated_tomahawk/BravesLogoUV2.png" });
	headModel->name = "animated_tomahawk";
	headModel->modelScale = 1.0f;
	headModel->modelShift = { 0, 0.3f, 0.15f };

	auto hatRenderParams = make_shared<ObjectRenderParams>();
	hatRenderParams->lightPos = { 0.2f, 0.4f, 1.0f, 0 };
	hatRenderParams->cameraPos = { 0, 0, 0, 1 };
	hatRenderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	hatRenderParams->specularLight = { 0.15f, 0.15f, 0.15f };
	hatRenderParams->cullFace = false;
	hatRenderParams->alphaTest = true;
	hatRenderParams->specularPower = 5;
	hatRenderParams->shader = shader;
	
	headModel->objectRenderParams.fill({ hatRenderParams });

	models.push_back(headModel);

	//for (auto &smoother : lipsDistanceSmoother)
	//{
	//	smoother.SNAP_MULTIPLIER_SIMPLE = 1.0 / 60 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
	//}
}

FxBravesTomahawkChop::~FxBravesTomahawkChop()
{

}

/*
void FxBravesTomahawkChop::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);
	
	//animateNextStep(face.pointId);

	//models[0]->renderParams.yawMatrix = Affine3d(AngleAxisd(0.6 * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
	//models[0]->renderParams.modelView = Affine3d(AngleAxisd(0.6 * M_PI / 180.0, Vector3d(0, 1, 0))).matrix() * models[0]->renderParams.modelView;

	Matrix4f modelMatrix = models[0]->renderParams.modelView;

	//models[0]->renderParams.modelView = Affine3f(Translation3f(0, 0.0035*(animationCounter[face.pointId]), 0)).matrix() * models[0]->renderParams.modelView;
	Affine3d rotation(AngleAxisd(21 * M_PI_4, Vector3d(1, 0, 0)));
	//models[0]->renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();

	//modelMatrix *= rotation.matrix().block<3, 3>(0, 0).cast<float>();
	models[0]->renderParams.modelView = modelMatrix;
	//models[0]->renderParams.yawMatrix = Affine3d(AngleAxisd(99 * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
	
}
*/

void FxBravesTomahawkChop::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	animateNextStep(face.pointId);

	auto lipToLip = lipsDistanceSmoother[face.pointId].responsiveAnalogReadSimple(
		PointCalcUtil::distanceBetween(face.pts[61 * 2], face.pts[61 * 2 + 1], face.pts[64 * 2], face.pts[64 * 2 + 1]));

	const double STD_FACE_WIDTH = 0.172;
	const double STD_LIPS_DISTANCE = 60;

	auto factor = 0.1 * (animationCounter[face.pointId]); //lipToLip / (face.faceWidth / STD_FACE_WIDTH * STD_LIPS_DISTANCE);

	double h = 30.0;

	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double hAngleHalf = atan(tan(vAngle / 2) * aspect);

	double planeH = 2.f *h * tan(vAngle / 2);
	double planeW = planeH * aspect;

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	for (int i = 0; i < 1; ++i)
	{
		models[i]->renderParams.projection = projection;

		auto xFaceCenterRawSmooth = models[i]->renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
		auto yFaceCenterRawSmooth = models[i]->renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

		auto rollSmooth = models[i]->renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
		auto pitchSmooth = models[i]->renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
		auto yawSmooth = models[i]->renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

		auto faceWidthRawSmooth = models[i]->renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

		double tx = xFaceCenterRawSmooth / face.fWidth;
		double ty = yFaceCenterRawSmooth / face.fHeight;

		double xShift = ((tx - 0.5) * planeW);
		double yShift = (0 - (ty - 0.5) * planeH);

		Matrix4d modelMatrix;

		modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, 0))).matrix();

		Vector3d rotateVector = Vector3d(0.0, 0.0, -h).cross(Vector3d(xShift, yShift, -h));
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift*xShift + yShift*yShift)) / h);

		//double additionalRotation = factor * M_PI_4 * (i == 0 ? -1 : 1);
		double additionalRotation = factor * M_PI_4;

		Affine3d rotation(AngleAxisd(additionalRotation, Vector3d(0, 0, 1)));

		//rotation *= AngleAxisd(additionalRotation, Vector3d(1, 0, 0));

		rotation *= AngleAxisd(rotateAngle, rotateVector);
	

		//rotation *= AngleAxisd(rotateAngle, rotateVector);

		//x
		//rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));

		//y
		//rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));

		//z
		//rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		modelMatrix *= rotation.matrix();

		const double faceWidthToDepth = 1.0;
		double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
		double faceDepth = faceWidthToDepth * faceWidth;

		// depth
		modelMatrix *= Affine3d(Translation3d(0, 0, -faceDepth)).matrix();

		const double scaleK = 0.012;
		double scale = faceWidth * scaleK * ((OpenGL3DModel*)models[i].get())->modelScale;

		// scale
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix();

		modelMatrix = Affine3d(Translation3d(0, 0, -h)).matrix() * modelMatrix;

		models[i]->depth = -faceDepth * scale - h;

		models[i]->renderParams.modelView = modelMatrix.cast<float>();
		models[i]->renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();

		//Then rotate yaw matrix
		//models[i]->renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
	}
}
void FxBravesTomahawkChop::animateNextStep(size_t i)
{
	if (animationDirection[i]) {

		animationCounter[i]++;

		if (animationCounter[i] == 0)
		{
			animationDirection[i] = false;
		}
	}
	else {

		animationCounter[i]--;

		if (animationCounter[i] == (-20))
		{
			animationDirection[i] = true;
		}
	}
}