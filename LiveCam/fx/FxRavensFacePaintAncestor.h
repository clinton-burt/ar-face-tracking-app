#pragma once

#include "PointCalcUtil.h"

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

struct RavensHalfFaceParams
{
	friend class RavensHalfFacePaint;
	friend class RavensFacePaintModel;

	Eigen::Vector4f leftAmbient;
	Eigen::Vector4f rightAmbient;
	Eigen::Vector4f lineAmbient;

	Eigen::Vector4f leftDiffuse;
	Eigen::Vector4f rightDiffuse;
	Eigen::Vector4f lineDiffuse;

	std::string leftTexture;
	std::string rightTexture;
	std::string lineTexture;

	std::string iconImagePath;
	std::string facemaskTexturePath;

	float lineWidth = 0;

	RavensHalfFaceParams(); // is needed for compilation

	RavensHalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
		Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
		std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth);

	RavensHalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
		Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
		std::string iconImagePath, std::string facemaskTexturePath);

	void applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately);
	void load();

	bool makeTexture();

protected:
	GLuint leftTexId = 0;
	GLuint rightTexId = 0;
	GLuint lineTexId = 0;
};

class RavensHalfFacePaint
{
	friend class RavensFacePaintModel;

protected:
	const static size_t LINE_STRIP_COUNT = 16;

	const static size_t CHIN_STRIP_COUNT = 9;
	const static size_t CHEEK_FAN_COUNT = 10;
	const static size_t BROW_STRIP_COUNT = 10;
	const static size_t NOSE_FAN_COUNT = 8;
	const static size_t UNDER_NOSE_FAN_COUNT = 6;
	const static size_t FOREHEAD_STRIP_COUNT = 9;

	// point indexes start with 0
	static const std::vector<std::vector<std::vector<int>>> indexes;

	GLuint leftVBO, rightVBO, lineVBO;

	GLuint leftTexCoordVBO = 0, rightTexCoordVBO = 0, lineTexCoordVBO = 0;

	std::array<std::array<ValueSmoother, 66 * 2>, FaceTracker::MAX_TO_TRACK> facePointsSmoothers;

public:
	GLuint textureid = 0;

public:
	void load();
	void transform(FXModel& face, RavensHalfFaceParams &params);
	void draw(FXModel& face, RavensHalfFaceParams &params, std::shared_ptr<cwc::glShader> shader);
	void unload();
};

class RavensFacePaintModel : public OpenGLModel
{
public:
	RavensHalfFacePaint halfFacePaint;
	std::array<RavensHalfFaceParams, FaceTracker::MAX_TO_TRACK> halfFaceParams;

	RavensFacePaintModel();

	void onInputFrameResized() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	// for LiveCam Editor
	std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
	void setDefaultObjectRenderParams(ObjectRenderParams& params) override;

protected:
	GLuint textureid = 0;
};