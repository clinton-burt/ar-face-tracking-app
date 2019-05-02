#include "fx/FxAnimatedEmojiAncestor.h"

using namespace Eigen;

OpenGLAnimatedEmojiModel::OpenGLAnimatedEmojiModel()
{
	animationOffset.fill({ 0, 0, 0 });
	animationScale.fill({ 1, 1 });
	animationPivot.fill({ 0, 0 });

	animationRotation.fill(false);
	animationRolling.fill(true);

	symmetricState.fill(0);

	match3D.fill(false);

	animationSpeed.fill(1);
	animationOpened.fill(0);
}

void OpenGLAnimatedEmojiModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);
}

void OpenGLAnimatedEmojiModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams& commonRenderParams,
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

	animationPaths[faceIndex] = suit.get<std::string>("animationPath", "");

	animationOpened[faceIndex] = suit.get<size_t>("animationOpened", 1);
	//animationLength[faceIndex] = suit.get<size_t>("animationLength", 2); // now is autoloading

	if (loadTexturesImmediately)
	{
		auto loadingResult = resourceManager.loadAnimation(animationPaths[faceIndex]);
		animationFramesIDs[faceIndex] = loadingResult.IDs;
		animationSize[faceIndex] = { loadingResult.animationWidth, loadingResult.animationHeight };
		animationLength[faceIndex] = loadingResult.animationLength;
	}

	animationIndexes[faceIndex] = 0;
	animationState[faceIndex] = CLOSED;

	animationRotation[faceIndex] = suit.get<bool>("animationRotation", false);
	animationRolling[faceIndex] = suit.get<bool>("animationRolling", true);
	animationSpeed[faceIndex] = suit.get<float>("animationSpeed", 1);

	match3D[faceIndex] = suit.get<bool>("match3D", false);

	auto tree = suit.get_child_optional("animationOffset");
	animationOffset[faceIndex] = tree ? JSONVectorReader::readVector3f(tree.get()) : Eigen::Vector3f(0, 0, 0);

	tree = suit.get_child_optional("animationScale");
	animationScale[faceIndex] = tree ? JSONVectorReader::readVector2f(tree.get()) : Eigen::Vector2f(1, 1);

	tree = suit.get_child_optional("animationPivot");
	animationPivot[faceIndex] = tree ? JSONVectorReader::readVector2f(tree.get()) : Eigen::Vector2f(0, 0);
}

bool OpenGLAnimatedEmojiModel::load()
{
	objects.push_back(ModelLoader::CreateQuadModel());

	for (int faceIndex = 0; faceIndex < FaceTracker::MAX_TO_TRACK; ++faceIndex)
	{
		auto loadingResult = resourceManager.loadAnimation(animationPaths[faceIndex]);
		animationFramesIDs[faceIndex] = loadingResult.IDs;
		animationSize[faceIndex] = { loadingResult.animationWidth, loadingResult.animationHeight };
		animationLength[faceIndex] = loadingResult.animationLength;

		animationIndexes[faceIndex] = 0;
		animationState[faceIndex] = CLOSED;
	}

	return true;
}

void OpenGLAnimatedEmojiModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
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

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		Affine3d rotation;
		Affine3d anti_rotation = Affine3d::Identity();

		rotation = Affine3d(AngleAxisd(-pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
		rotation *= AngleAxisd(-yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		if (!animationRolling[face.pointId])
		{
			anti_rotation *= AngleAxisd(-rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
		}

		if (animationRotation[face.pointId])
		{
			renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
		}
		else
		{
			renderParams.yawMatrix = Matrix3f::Identity();
			anti_rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
			anti_rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		}

		bool XMirror = symmetricState[face.pointId] != 0 && -symmetricState[face.pointId] == ((0 < yawSmooth) - (yawSmooth < 0));

		modelMatrix *= Affine3d(Translation3d(Vector3d(xShift, yShift, -distance))).matrix(); // offset to faceCenter

		modelMatrix *= Affine3d(Scaling(1., 1. * width / height, 1.)).matrix(); // screen aspect ratio

		modelMatrix *= rotation.matrix(); // rotation

										  // auto topPoint = PointCalcUtil::centerOf2Points({ (int)face.pts[38], (int)face.pts[39] }, { (int)face.pts[48], (int)face.pts[49] });
										  // float faceHeight = PointCalcUtil::distanceBetween({ (int)face.pts[16], (int)face.pts[17] }, topPoint);

		double Xoffset = (XMirror ? -1.0 : 1.0) * animationOffset[face.pointId][0] * faceWidth;
		double Yoffset = animationOffset[face.pointId][1] * faceWidth;
		double Zoffset = -animationOffset[face.pointId][2] * STD_HEAD_LENGTH;
		modelMatrix *= Affine3d(Translation3d(Vector3d(Xoffset, Yoffset, Zoffset))).matrix(); // offset relativly faceWidth

		modelMatrix *= anti_rotation.matrix(); // anti-rotation to prevent yaw & pitch rotation to keep screen-perpendicularly emoji orientation

		double Xscale = (XMirror ? -1.0 : 1.0) * animationScale[face.pointId][0];
		double Yscale = -animationScale[face.pointId][1] * animationSize[face.pointId][1] / animationSize[face.pointId][0];

		Xoffset = animationPivot[face.pointId][0] * faceWidth * Xscale;
		Yoffset = -animationPivot[face.pointId][1] * faceWidth * Yscale;
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
		if (animationRotation[face.pointId])
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
		modelMatrix *= Affine3d(Translation3d(Vector3d((XMirror ? -1 : 1) * animationOffset[face.pointId][0],
			animationOffset[face.pointId][1], animationOffset[face.pointId][2]))).matrix();
		double Xscale = face.faceWidth * animationScale[face.pointId][0] * (XMirror ? -1 : 1);
		double Yscale = face.faceWidth * animationScale[face.pointId][1] * animationSize[face.pointId][1] / animationSize[face.pointId][0];
		modelMatrix *= Affine3d(Scaling(Xscale, Yscale, 1.)).matrix();
		depth = -h - faceDepth;
		renderParams.modelView = modelMatrix.cast<float>();
		break;
	}
	}

	animate(face);
}

void OpenGLAnimatedEmojiModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	//if (animationIndexes[face.pointId] == 0)
	if (animationIndexes[face.pointId] > animationLength[face.pointId] - 1e-2f)
 	{
 		return;
 	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);


	glEnable(GL_BLEND);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, animationFramesIDs[face.pointId][(int)animationIndexes[face.pointId]]);

	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	SetUniformsForObject(*shader, 0, face.pointId);

	VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, objects[0].vb));
	VertexAttribSetter vTexCoord(VertexAttribSetter::TexCoordAttribSetter(*shader, objects[0].tb));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glPopAttrib();
}

void OpenGLAnimatedEmojiModel::unload()
{
	OpenGLModel::unload();

	for (auto &animation : animationFramesIDs)
	{
		animation.clear();
	}
}

void OpenGLAnimatedEmojiModel::animate(FXModel& face)
{
	// animation reacting on a mouth opening, a tongue, for example.
	// now only the last part of animation is used: from the end to the opened state if the mouth is opened,
	// and if the mouth is closed, play animation from the opened state to the end.
	// earlier was: the mouth is opened -> from begin to opened state, if closed -> from opened state to end.

	size_t faceIndex = face.pointId;

	const std::array<float, 66 * 2>&  pts = face.pts;

	int lipTopX = pts[122];
	int lipTopY = pts[123];
	int lipBotX = pts[128];
	int lipBotY = pts[129];

	cv::Point topLip = cv::Point(lipTopX, lipTopY);
	cv::Point bottomLip = cv::Point(lipBotX, lipBotY);

	cv::Point middleOfMouth = PointCalcUtil::centerOf2Points(topLip, bottomLip);

	double lipToLip = PointCalcUtil::distanceBetween(lipTopX, lipTopY, lipBotX, lipBotY);

	// LET'S TRY TO USE ONLY LAST PART OF ANIMATION
	if (animationIndexes[faceIndex] < animationOpened[faceIndex])
	{
		animationIndexes[faceIndex] = animationLength[faceIndex] - 1e-2f;
		animationState[faceIndex] = CLOSING;
	}

	const float STD_FACE_DISTANCE = 0.6;

	lipToLip = -depth * lipToLip / STD_FACE_DISTANCE;

	if (lipToLip > 16)
	{
	 	animationState[faceIndex] = OPENING;
	}
	else if (lipToLip < 10)
	{
	 	animationState[faceIndex] = CLOSING;
	}
	 
	if (animationState[faceIndex] == OPENING)
	{
		animationIndexes[faceIndex] -= animationSpeed[faceIndex];
	 	if (animationIndexes[faceIndex] < animationOpened[faceIndex] + 1e-2f)
		{
			animationIndexes[faceIndex] = animationOpened[faceIndex] + 1e-2f;
	 	}
	}
	else if (animationState[faceIndex] == CLOSING)
	{
		animationIndexes[faceIndex] += animationSpeed[faceIndex];
	 	if (animationIndexes[faceIndex] > animationLength[faceIndex] - 1e-2f)
		{
			animationIndexes[faceIndex] = animationLength[faceIndex] - 1e-2f;
	 	}
	}

// 	if (lipToLip > 15 && animationState[faceIndex] == CLOSED)
// 	{
// 		animationState[faceIndex] = OPENING;
// 	}
// 	else if (lipToLip < 10 && animationState[faceIndex] == OPENED)
// 	{
// 		animationState[faceIndex] = CLOSING;
// 	}
// 
// 	if (animationState[faceIndex] == OPENING)
// 	{
// 		++animationIndexes[faceIndex];
// 		if (animationIndexes[faceIndex] == animationOpened[faceIndex])
// 		{
// 			animationState[faceIndex] = OPENED;
// 		}
// 	}
// 	else if (animationState[faceIndex] == CLOSING)
// 	{
// 		++animationIndexes[faceIndex];
// 		if (animationIndexes[faceIndex] == animationLength[faceIndex] - 1)
// 		{
// 			animationState[faceIndex] = CLOSED;
// 			animationIndexes[faceIndex] = animationOpened[faceIndex];
// 		}
// 	}
}

std::shared_ptr<ObjectRenderParams> OpenGLAnimatedEmojiModel::createDefaultObjectRenderParams()
{
	auto result = std::make_shared<ObjectRenderParams>();
	result->shadersSources.first = "./assets/shaders/vertex/ortho3dMatchVertexShader.txt";
	result->shadersSources.second = "./assets/shaders/fragment/orthoFragmentShader.txt";

	auto shader2D = shaderManagerWrapper.LoadFromFile(result->shadersSources.first.data(), result->shadersSources.second.data());
	result->shader = shader2D;
	result->blend = true;
	result->cullFace = false;
	result->alphaTest = true;
	return result;
}

void OpenGLAnimatedEmojiModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
	params.blend = true;
	params.cullFace = false;
}