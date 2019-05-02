#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#define _USE_MATH_DEFINES
#include <math.h>

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

using namespace Eigen;

class OpenGL3DModel : public OpenGLModel
{
public:
	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	bool backClipping; // discard fragment of NOT TRANSFORMED BUT CENTERED model (look for autoShift in transform()) if
	float Yclip; // 1) fragment of such model has Y < minX[1] + (maxX[1] - minX[1]) * Yclip      AND
	float Zclip; // 2) fragment of CENTERED & ROTATED model has Z < minZ[2] + (maxZ[2] - minX[2]) * Zclip
	std::vector<std::string> materialList;
	std::vector<std::string> modelList;

	std::string modelPath;
	std::string materialPath;

	std::array<float, 3> bmin = { 0, 0, 0 };
	std::array<float, 3> bmax = { 0, 0, 0 };

	Eigen::Matrix3f extraRotateMatrix = Eigen::Matrix3f::Identity();

	float modelScale = 1; // head-relative scale
	Eigen::Vector3f modelShift = { 0, 0, 0 }; // head-relative shift

	std::array<std::vector<std::string>, FaceTracker::MAX_TO_TRACK> texturesPaths;
	std::array<std::vector<GLuint>, FaceTracker::MAX_TO_TRACK> texturesIDs;

	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> pivotX; // gives smoothed coords of one pivot point from tracker,
	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> pivotY; // the particular point is specified in transform() of derived classes
	Eigen::Vector3d pivotOffset; // the pivot offset is specified in constructors of derived classes

	OpenGL3DModel();

	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	boost::property_tree::ptree getPTree(ExtraModelData &data) override;

	void prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData) override;

	void onInputFrameResized();

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	// for LiveCam Editor
	std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
	void setDefaultObjectRenderParams(ObjectRenderParams& params) override;

	bool reloadOBJ();
	void sortMeshesInZAscending();
};

#include "DepthMask.h" // after OpenGL3DModel

class LipsJoint3DModel : public OpenGL3DModel
{
public:
	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	LipsJoint3DModel();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};

class LipsBottomJoint3DModel : public OpenGL3DModel
{
public:
	LipsBottomJoint3DModel();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};

class LeftBrow3DModel : public OpenGL3DModel
{
public:
	LeftBrow3DModel();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};

class RightBrow3DModel : public OpenGL3DModel
{
public:
	RightBrow3DModel();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};

class Static3DModel : public OpenGL3DModel
{
public:
	Static3DModel();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};