#include "fx/DepthMask.h"

const std::string DepthMask::TYPE_NAME = "DepthMask";

std::string DepthMask::getTypeName()
{
	return TYPE_NAME;
}

DepthMask::DepthMask() : DepthMask(0, true, true)
{
}

DepthMask::DepthMask(float zOffset, bool draw2Dmask, bool draw3Dmask, bool drawEars)
{
	this->zOffset = zOffset;
	this->draw2Dmask = draw2Dmask;
	this->draw3Dmask = draw3Dmask;

	shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/ortho3dMatchNoTextureVertexShader.txt", "./assets/shaders/fragment/transparentFragmentShader.txt");

	shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/transparentVertexShader.txt", "./assets/shaders/fragment/transparentFragmentShader.txt");

	name = "depthMask";

	canSwapSuit = false;

	modelPath = "./assets/fx/depthMask/head_no_ears.obj";
	//modelPath = "./assets/fx/depthMask/head_v2.obj";

	modelScale = 1.9f;
	modelShift = { 0, -0.33f, 0 };
}

DepthMask::~DepthMask() { }

void DepthMask::onInputFrameResized()
{
	OpenGLModel::onInputFrameResized();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		params2D.faceXCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params2D.faceYCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params2D.faceWidthSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params2D.pitchSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		params2D.yawSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		params2D.rollSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;

		params3D.faceXCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params3D.faceYCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params3D.faceWidthSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		params3D.pitchSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		params3D.yawSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		params3D.rollSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
	}
}

void DepthMask::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	canSwapSuit = modelRecord.get<bool>("canSwapSuit", false);

	this->zOffset = modelRecord.get<float>("zOffset", 0);
	this->draw2Dmask = modelRecord.get<bool>("draw2DMask", true);
	this->draw3Dmask = modelRecord.get<bool>("draw3DMask", true);

	shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/ortho3dMatchNoTextureVertexShader.txt", "./assets/shaders/fragment/transparentFragmentShader.txt");

	shader3D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/transparentVertexShader.txt", "./assets/shaders/fragment/transparentFragmentShader.txt");

	name = "depthMask";

	bool toStealEars = !modelRecord.get<bool>("drawEars", false);

	modelPath = modelRecord.get<std::string>("OBJ",
		toStealEars ? "./assets/fx/depthMask/head_no_ears.obj" : "./assets/fx/depthMask/head.obj");

	bool addLongNeck = modelRecord.get<bool>("drawLongNeck", false);
	if (addLongNeck) {
		modelPath = "./assets/fx/depthMask/head_long_neck.obj";
	}

	auto extraShiftVector = modelRecord.get_child_optional("modelShift");
	modelShift = extraShiftVector ? JSONVectorReader::readVector3f(extraShiftVector.get()) : Eigen::Vector3f(0, -0.33f, 0);

	modelScale = modelRecord.get<float>("modelScale", 1.9f);
}

void DepthMask::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately)
{
	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	if (!visible[faceIndex])
	{
		return;
	}

	auto extraShiftVector = suit.get_child_optional("modelShift");
	if (extraShiftVector)
	{
		modelShift = JSONVectorReader::readVector3f(extraShiftVector.get());
	}

	auto modelScaleRecord = suit.get_optional<float>("modelScale");
	if (modelScaleRecord)
	{
		modelScale = modelScaleRecord.get();
	}

	auto zOffset = suit.get_optional<float>("zOffset");
	if (zOffset)
	{
		this->zOffset = zOffset.get();
	}

	auto draw2Dmask = suit.get_optional<bool>("draw2DMask");
	if (draw2Dmask)
	{
		this->draw2Dmask = draw2Dmask.get();
	}

	auto draw3Dmask = suit.get_optional<bool>("draw3DMask");
	if (draw3Dmask)
	{
		this->draw3Dmask = draw3Dmask.get();
	}
}

const std::array<int, DepthMask::FACE_FAN_COUNT> DepthMask::face_fan =
{
	8, 9, 10, 11, 12, 13, 14, 15, 16, 26, 25, 24, 19, 18, 17, 0, 1, 2, 3, 4, 5, 6, 7
};

bool DepthMask::load()
{
	
	OpenGL3DModel::load();

	glGenBuffers(1, &mask2dVBO);

	return true;
}

void DepthMask::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	if (!draw2Dmask && !draw3Dmask) return;

	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;

	auto faceWidthRawSmooth = params3D.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
	depth = -distance;

	if (draw3Dmask)
	{
		double vAngle = externalRenderParams.vAngle;
		double f = 1 / tan(vAngle / 2);
		double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

		Matrix4f projection = Matrix4f::Zero();

		projection(0, 0) = f / aspect;
		projection(1, 1) = f;
		projection(2, 2) = (zFar + zNear) / (zNear - zFar);
		projection(3, 2) = -1;
		projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

		params3D.projection = projection;

		//Then modelview matrix

		auto xFaceCenterRawSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
		auto yFaceCenterRawSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

		auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
		auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
		auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

		double tx = xFaceCenterRawSmooth / face.fWidth;
		double ty = yFaceCenterRawSmooth / face.fHeight;

		double hAngleHalf = atan(tan(vAngle / 2) * aspect);

		Matrix4d modelMatrix;

		switch (transformVersion)
		{
		case TransformVersion::V2:
		{
			double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
			depth = -distance;

			double planeH = 2.f * distance * tan(vAngle / 2);
			double planeW = planeH * aspect;

			double xShift = (tx - 0.5) * planeW;
			double yShift = -(ty - 0.5) * planeH;

			Vector3d shift(xShift, yShift, -distance);
			modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

			Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(shift);
			rotateVector.normalize();
			double rotateAngle = atan((sqrt(xShift * xShift + yShift * yShift)) / distance);

			Affine3d rotation(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
																	  //x
			rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
			//y
			rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
			//z
			rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

			modelMatrix *= rotation.matrix(); // rotation

			Eigen::Matrix4d headAdjusting = Eigen::Matrix4d::Identity();

			shift = { modelShift[0] * STD_FACE_WIDTH,  modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
			headAdjusting *= Affine3d(Translation3d(Vector3d(shift[0], shift[1], shift[2] - STD_HEAD_LENGTH / 2))).matrix();
			// head center shift && custom head-relative shift

			double scale = faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
			headAdjusting *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with faceWidth & head-relative scale

			Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
			headAdjusting *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

			modelMatrix *= headAdjusting;

			params3D.additionalMatrices4[0] = headAdjusting.cast<float>();

			params3D.modelView = modelMatrix.cast<float>();
			params3D.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
			break;
		}



		case TransformVersion::V1:
		{
			float h = 30;

			double planeH = 2.f * h * tan(vAngle / 2);
			double planeW = planeH * aspect;

			double xShift = (tx - 0.5) * planeW;
			double yShift = -(ty - 0.5) * planeH;

			modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, 0))).matrix();
			Vector3d rotateVector = Vector3d(0.0, 0.0, -h).cross(Vector3d(xShift, yShift, -h));
			rotateVector.normalize();
			double rotateAngle = atan((sqrt(xShift*xShift + yShift*yShift)) / h);
			Affine3d rotation(AngleAxisd(rotateAngle, rotateVector));
			//x
			rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
			//y
			rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
			//z
			rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
			modelMatrix *= rotation.matrix();
			const double faceWidthToDepth = 1.0;
			double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
			double faceDepth = faceWidthToDepth * faceWidth;
			// depth
			modelMatrix *= Affine3d(Translation3d(0, 0, -faceDepth)).matrix();
			const double scaleK = 0.08;
			double scale = faceWidth * scaleK;
			// scale
			modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix();
			modelMatrix = Affine3d(Translation3d(0, 0, -h)).matrix() * modelMatrix;
			depth = -faceDepth * scale - h;
			params3D.modelView = modelMatrix.cast<float>();
			params3D.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
			break;
		}
		}

		//Then rotate yaw matrix
		params3D.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
	}

	if (draw2Dmask)
	{

		Matrix4f projection = Matrix4f::Zero();
		projection(0, 0) = 2 / width;
		projection(1, 1) = -2 / height;
		projection(2, 2) = (zFar + zNear) / (zNear - zFar);
		projection(3, 3) = 1;
		projection(0, 3) = -1;
		projection(1, 3) = 1;
		projection(2, 3) = 2 * zFar * zNear / (zNear - zFar);

		params2D.projection = projection;

		Matrix4d modelMatrix(Affine3d(Translation3d(Vector3d(0, 0, depth + zOffset))).matrix());

		params2D.modelView = modelMatrix.cast<float>();
		params2D.rotationMatrix = Matrix3f::Identity();
		params2D.yawMatrix = Matrix3f::Identity();

		ImagePoints points(face.pts);

		cv::Point2f chinPoint = points.at(9);
		cv::Point2f tmp = PointCalcUtil::crossPointTwoLines(chinPoint, points.at(28), points.at(21), points.at(24));
		Eigen::Vector2f topPoint = { tmp.x, tmp.y };

		Eigen::Vector2f verticalOffset = points.getEigenAt(28) - Eigen::Vector2f(chinPoint.x, chinPoint.y);
		verticalOffset *= 0.2;

		std::array<float, FACE_FAN_COUNT * 3> buffer;
		auto iter = buffer.begin();

		for (auto index : face_fan)
		{
			if (index < 17)
			{
				*iter++ = points.pts[index * 2];
				*iter++ = points.pts[index * 2 + 1];
			}
			else
			{
				*iter++ = points.pts[index * 2] + verticalOffset[0];
				*iter++ = points.pts[index * 2 + 1] + verticalOffset[1];
			}
			*iter++ = 0;
		}

		glBindBuffer(GL_ARRAY_BUFFER, mask2dVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FACE_FAN_COUNT * 3, buffer.data(), GL_STREAM_DRAW);
	}
}

void DepthMask::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	if (!draw2Dmask && !draw3Dmask) return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	if (draw2Dmask)
	{
		shader2D->begin();

		params2D.SetUniforms(*shader2D);

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader2D, mask2dVBO));

		glDrawArrays(GL_TRIANGLE_FAN, 0, FACE_FAN_COUNT);

		shader2D->end();
	}

	if (draw3Dmask)
	{
		shader3D->begin();

		params3D.SetUniforms(*shader3D);

		DrawObjectsModel o = objects[0];

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader3D, o.vb));

		glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);

		shader3D->end();
	}

	glPopAttrib();
}

void DepthMask::unload()
{
	OpenGL3DModel::unload();

	glDeleteBuffers(1, &mask2dVBO);
}