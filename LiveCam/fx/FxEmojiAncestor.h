#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

class OpenGLEmojiModel : public OpenGLModel
{
protected:
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> textureSize;

public:
	OpenGLEmojiModel();

	std::array<std::string, FaceTracker::MAX_TO_TRACK> emojisTextures;
	std::array<GLuint, FaceTracker::MAX_TO_TRACK> emojisTexturesIDs;

	std::array<Eigen::Vector3f, FaceTracker::MAX_TO_TRACK> emojisOffset;
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> emojisScale;
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> emojisPivot; // texture rotation pivot in texture coord

	std::array<bool, FaceTracker::MAX_TO_TRACK> match3D;

	std::array<int, FaceTracker::MAX_TO_TRACK> symmetricState;
	// -1: default image is not symmetric & looks at left; 0: is symmetric, no X mirroring needed; 1: looks at right

	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;

	boost::property_tree::ptree getPTree(ExtraModelData &data) override;
	void prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;

	std::array<bool, FaceTracker::MAX_TO_TRACK> emojiRotation;
	std::array<bool, FaceTracker::MAX_TO_TRACK> emojiRolling;


	// for LiveCam Editor
	std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
	void setDefaultObjectRenderParams(ObjectRenderParams& params) override;

};

class Suit2DModel : public OpenGLEmojiModel
{
	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;

};