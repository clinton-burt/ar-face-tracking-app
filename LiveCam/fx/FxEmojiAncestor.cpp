#include "fx/FxEmojiAncestor.h"

using namespace Eigen;

const std::string OpenGLEmojiModel::TYPE_NAME = "EmojiModel";

std::string OpenGLEmojiModel::getTypeName()
{
	return TYPE_NAME;
}

OpenGLEmojiModel::OpenGLEmojiModel()
{
	emojisOffset.fill({ 0, 0, 0 });
	emojisScale.fill({ 1, 1 });
	emojisPivot.fill({ 0.f, 0.f });

	symmetricState.fill(0);
	emojiRotation.fill(false);
	emojiRolling.fill(true);
	match3D.fill(false);
}

void OpenGLEmojiModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);
}

void OpenGLEmojiModel::prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData)
{
	int renderIDs = findMatchingRenderParamsIndexes
		(modelData.root->filter->commonRenderParams, modelData).front().second.get_value<int>();

	suit.put("renderParamsID", renderIDs);

	auto &emojiTexture = suit.get<std::string>("emojiTexture", "");
	if (!emojiTexture.empty())
	{
		boost::filesystem::path path(emojiTexture);
		suit.put("emojiTexture", path.filename().string());

		auto originFile = fs::path(modelData.root->resourcesRoot / path);
		auto copiedFile = fs::path(modelData.root->filterFolder / path.filename());
		if (!equivalent(originFile, copiedFile))
		{
			fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
		}
	}
}

boost::property_tree::ptree OpenGLEmojiModel::getPTree(ExtraModelData &data)
{
	auto tree = OpenGLModel::getPTree(data);

	tree.put("type", getTypeName());

	return tree;
}

void OpenGLEmojiModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately)
{
	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	if (!visible[faceIndex])
	{
		return;
	}

	auto renderParamsID = suit.get_optional<size_t>("renderParamsID");
	if (renderParamsID)
	{
		objectRenderParams[faceIndex].clear();
		objectRenderParams[faceIndex].push_back(commonRenderParams[renderParamsID.get()]);
	}

	symmetricState[faceIndex] = suit.get<int>("symmetricState", 0);

	emojisTextures[faceIndex] = suit.get<std::string>("emojiTexture", "");

	if (loadTexturesImmediately)
	{
		auto loadingResult = resourceManager.loadTexture(emojisTextures[faceIndex]);
		emojisTexturesIDs[faceIndex] = loadingResult.ID;
		textureSize[faceIndex] = { loadingResult.textureWidth, loadingResult.textureHeight };
	}

	emojiRotation[faceIndex] = suit.get<bool>("emojiRotation", false);
	emojiRolling[faceIndex] = suit.get<bool>("emojiRolling", true);

	match3D[faceIndex] = suit.get<bool>("match3D", false);

	auto tree = suit.get_child_optional("emojiOffset");
	emojisOffset[faceIndex] = tree ? JSONVectorReader::readVector3f(tree.get()) : Eigen::Vector3f(0, 0, 0);

	tree = suit.get_child_optional("emojiScale");
	emojisScale[faceIndex] = tree ? JSONVectorReader::readVector2f(tree.get()) : Eigen::Vector2f(1, 1);

	tree = suit.get_child_optional("emojiPivot");
	emojisPivot[faceIndex] = tree ? JSONVectorReader::readVector2f(tree.get()) : Eigen::Vector2f(0, 0);
}

bool OpenGLEmojiModel::load()
{
	objects.push_back(ModelLoader::CreateQuadModel());

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		auto loadingResult = resourceManager.loadTexture(emojisTextures[i]);
		emojisTexturesIDs[i] = loadingResult.ID;
		textureSize[i] = { loadingResult.textureWidth, loadingResult.textureHeight };
	}

	return true;
}

void OpenGLEmojiModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;
	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);
	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;



	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = 1;
	projection(1, 1) = 1;
	projection(3, 3) = 1;

	if (match3D[face.pointId])
	{
		projection(2, 2) = (zFar + zNear) / (zNear - zFar);
		projection(2, 3) = 2 * zFar * zNear / (zNear - zFar);
	}
	else
	{
		projection(2, 2) = 2 / (zNear - zFar);
		projection(2, 3) = (zFar + zNear) / (zNear - zFar);
	}

	renderParams.projection = projection;

	auto xRawSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
	auto yRawSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double rollSmooth, pitchSmooth, yawSmooth;

	pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);
	rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
	depth = -distance;

	Matrix4d modelMatrix;
	modelMatrix.setIdentity();

	double xShift = ((xRawSmooth / externalRenderParams.ULSWidth) - 0.5) * 2;
	double yShift = -((yRawSmooth / externalRenderParams.ULSHeight) - 0.5) * 2;

	switch(transformVersion)
	{
	case TransformVersion::V2:
	{
		Affine3d rotation;
		Affine3d anti_rotation = Affine3d::Identity();

		rotation = Affine3d(AngleAxisd(-pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
		rotation *= AngleAxisd(-yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		if (emojiRotation[face.pointId])
		{
			renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
		}
		else
		{
			renderParams.yawMatrix = Matrix3f::Identity();
			anti_rotation = Affine3d(AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
			anti_rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		}

		if (!emojiRolling[face.pointId])
		{
			anti_rotation *= AngleAxisd(-rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
		}

		bool XMirror = symmetricState[face.pointId] != 0 && -symmetricState[face.pointId] == ((0 < yawSmooth) - (yawSmooth < 0));

		modelMatrix *= Affine3d(Translation3d(Vector3d(xShift, yShift, -distance))).matrix(); // offset to faceCenter

		modelMatrix *= Affine3d(Scaling(1., 1. * width / height, 1.)).matrix(); // screen aspect ratio

		modelMatrix *= rotation.matrix(); // rotation

		// auto topPoint = PointCalcUtil::centerOf2Points({ (int)face.pts[38], (int)face.pts[39] }, { (int)face.pts[48], (int)face.pts[49] });
		// float faceHeight = PointCalcUtil::distanceBetween({ (int)face.pts[16], (int)face.pts[17] }, topPoint);

		double Xoffset = (XMirror ? -1.0 : 1.0) * emojisOffset[face.pointId][0] * faceWidth;
		double Yoffset = emojisOffset[face.pointId][1] * faceWidth;
		double Zoffset = -emojisOffset[face.pointId][2] * STD_HEAD_LENGTH;
		modelMatrix *= Affine3d(Translation3d(Vector3d(Xoffset, Yoffset, Zoffset))).matrix(); // offset relativly faceWidth

		modelMatrix *= anti_rotation.matrix(); // anti-rotation to prevent yaw & pitch rotation to keep screen-perpendicularly emoji orientation

		double Xscale = (XMirror ? -1.0 : 1.0) * emojisScale[face.pointId][0];
		double Yscale = -emojisScale[face.pointId][1] * textureSize[face.pointId][1] / textureSize[face.pointId][0];

		Xoffset = emojisPivot[face.pointId][0] * faceWidth * Xscale;
		Yoffset = -emojisPivot[face.pointId][1] * faceWidth * Yscale;
		modelMatrix *= Affine3d(Translation3d(Vector3d(Xoffset, Yoffset, 0))).matrix(); // go to pivot

		modelMatrix *= Affine3d(Scaling(faceWidth, faceWidth, 0.5)).matrix(); // resize for one atlas texture according to faceWidth
		modelMatrix *= Affine3d(Scaling(Xscale, Yscale, 1.)).matrix(); // head-relative scale & distortion fixing & Y mirror

		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
	
		renderParams.modelView = modelMatrix.cast<float>();
		break;
	}


	case TransformVersion::V1:
	{
		float h = 30;
		double planeH = 2.f * h * tan(vAngle / 2);
		double planeW = planeH * aspect;
		const double faceWidthToDepth = 1.0;
		double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
		double faceDepth = faceWidthToDepth * faceWidth;

		modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, -h - faceDepth))).matrix();
		modelMatrix *= Affine3d(Scaling(2., 2 * -width / height, 1.)).matrix();
		Affine3d rotation;
		if (emojiRotation[face.pointId])
		{
			//x
			rotation = Affine3d(AngleAxisd(-pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
			//y
			rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
			//z
			rotation *= AngleAxisd(-rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
			renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
		}
		else
		{
			//z
			rotation = Affine3d(AngleAxisd(-rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1)));
			renderParams.yawMatrix = Matrix3f::Identity();
		}
		modelMatrix *= rotation.matrix();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		bool XMirror = symmetricState[face.pointId] != 0 && -symmetricState[face.pointId] == ((0 < yawSmooth) - (yawSmooth < 0));
		modelMatrix *= Affine3d(Translation3d(Vector3d((XMirror ? -1 : 1) * emojisOffset[face.pointId][0],
			emojisOffset[face.pointId][1], emojisOffset[face.pointId][2]))).matrix();
		double Xscale = face.faceWidth * emojisScale[face.pointId][0] * (XMirror ? -1 : 1);
		double Yscale = face.faceWidth * emojisScale[face.pointId][1] * textureSize[face.pointId][1] / textureSize[face.pointId][0];
		modelMatrix *= Affine3d(Scaling(Xscale, Yscale, 1.)).matrix();
		depth = -h - faceDepth;
		renderParams.modelView = modelMatrix.cast<float>();
		break;
	}
	}
}

void OpenGLEmojiModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	GLuint texID = emojisTexturesIDs[face.pointId];
	if (texID <= 0) return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);

	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	SetUniformsForObject(*shader, 0, face.pointId);

	VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, objects[0].vb));
	VertexAttribSetter vTexCoord(VertexAttribSetter::TexCoordAttribSetter(*shader, objects[0].tb));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glPopAttrib();
}

std::shared_ptr<ObjectRenderParams> OpenGLEmojiModel::createDefaultObjectRenderParams()
{
	auto result = std::make_shared<ObjectRenderParams>();

	result->shadersSources.first = "./assets/shaders/vertex/orthoVertexShader.txt";
	result->shadersSources.second = "./assets/shaders/fragment/orthoFragmentShader.txt";

	auto shader2D = shaderManagerWrapper.LoadFromFile(result->shadersSources.first.data(), result->shadersSources.second.data());
	result->shader = shader2D;
	result->blend = true;
	result->cullFace = false;
	result->alphaTest = true;

	return result;
}

void OpenGLEmojiModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
	params.blend = true;
	params.cullFace = false;
}


const std::string Suit2DModel::TYPE_NAME = "Suit2DModel";

std::string Suit2DModel::getTypeName()
{
	return TYPE_NAME;
}

void Suit2DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;
	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);
	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;

	Matrix4f projection = Matrix4f::Zero();
	projection(0, 0) = 1;
	projection(1, 1) = 1;
	projection(2, 2) = 2 / (zNear - zFar);
	projection(3, 3) = 1;
	projection(0, 3) = 0;
	projection(1, 3) = 0;
	projection(2, 3) = (zFar + zNear) / (zNear - zFar);
	renderParams.projection = projection;

	auto xRawSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
	auto yRawSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
	depth = -distance;

	Matrix4d modelMatrix;
	modelMatrix.setIdentity();

	double xShift = ((xRawSmooth / externalRenderParams.ULSWidth) - 0.5) * 2;
	double yShift = -((yRawSmooth / externalRenderParams.ULSHeight) - 0.5) * 2;

	renderParams.yawMatrix = Matrix3f::Identity();

	modelMatrix *= Affine3d(Translation3d(Vector3d(xShift, yShift, -distance))).matrix(); // offset to faceCenter

	modelMatrix *= Affine3d(Scaling(1., 1. * width / height, 1.)).matrix(); // screen aspect ratio

	modelMatrix *= Affine3d(Scaling((double)faceWidth, (double)faceWidth, 0.5)).matrix(); // resize according to faceWidth

	double x = sin(yawSmooth / 180.f * M_PI);
	double y = sin(pitchSmooth / 180.f * M_PI);
	//double z = sqrt(1 - x * x + y * y);
	modelMatrix *= Affine3d(Translation3d(Eigen::Vector3d(-x, y, 0))).matrix(); // head center shift

	// 		auto topPoint = PointCalcUtil::centerOf2Points({ (int)face.pts[38], (int)face.pts[39] }, { (int)face.pts[48], (int)face.pts[49] });
	// 		float faceHeight = PointCalcUtil::distanceBetween({ (int)face.pts[16], (int)face.pts[17] }, topPoint);
	double Xoffset = emojisOffset[face.pointId][0];
	double Yoffset = emojisOffset[face.pointId][1];
	modelMatrix *= Affine3d(Translation3d(Vector3d(Xoffset, Yoffset, -emojisOffset[face.pointId][2]))).matrix(); // offset relativly faceWidth

	double Xscale = emojisScale[face.pointId][0];
	double Yscale = -emojisScale[face.pointId][1] * textureSize[face.pointId][1] / textureSize[face.pointId][0];
	modelMatrix *= Affine3d(Scaling(Xscale, Yscale, 1.)).matrix(); // head-relative scale & distortion fixing & Y mirror

	renderParams.rotationMatrix = Eigen::Matrix3f::Identity();

	renderParams.modelView = modelMatrix.cast<float>();
	
}