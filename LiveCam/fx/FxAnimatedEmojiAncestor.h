#pragma once

#include <utils\ModelLoader.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

enum AnimationState
{
	CLOSED,
	OPENING,
	OPENED,
	CLOSING
};

class OpenGLAnimatedEmojiModel : public OpenGLModel
{
protected:
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> animationSize;
public:
	OpenGLAnimatedEmojiModel();

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	virtual void animate(FXModel& face); // is being called from transform

	std::array<bool, FaceTracker::MAX_TO_TRACK> animationRotation;
	std::array<bool, FaceTracker::MAX_TO_TRACK> animationRolling;

	std::array<int, FaceTracker::MAX_TO_TRACK> symmetricState;
	std::array<std::string, FaceTracker::MAX_TO_TRACK> animationPaths;

	std::array<Eigen::Vector3f, FaceTracker::MAX_TO_TRACK> animationOffset;
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> animationScale;
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> animationPivot; // animation rotation pivot in texture coord

	std::array<bool, FaceTracker::MAX_TO_TRACK> match3D;

	std::array<size_t, FaceTracker::MAX_TO_TRACK> animationOpened;
	std::array<size_t, FaceTracker::MAX_TO_TRACK> animationLength;
	std::array<float, FaceTracker::MAX_TO_TRACK> animationSpeed; // New!

	std::array<std::vector<GLuint>, FaceTracker::MAX_TO_TRACK> animationFramesIDs;
	std::array<float, FaceTracker::MAX_TO_TRACK> animationIndexes;
	std::array<AnimationState, FaceTracker::MAX_TO_TRACK> animationState;

	// for LiveCam Editor
	std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
	void setDefaultObjectRenderParams(ObjectRenderParams& params) override;
};