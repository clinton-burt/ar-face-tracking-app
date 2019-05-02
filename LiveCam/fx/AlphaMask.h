#pragma once

#include <FxFacePaintAncestor.h>

#define _USE_MATH_DEFINES
#include <math.h>

class AlphaMask : public OpenGLModel
{
public:
	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	std::array<float, FaceTracker::MAX_TO_TRACK> smoothEdgeWidthFactor;
	std::array<bool, FaceTracker::MAX_TO_TRACK> cutOutLeftEye;
	std::array<bool, FaceTracker::MAX_TO_TRACK> cutOutRightEye;
	std::array<bool, FaceTracker::MAX_TO_TRACK> cutOutMouth;
	std::array<bool, FaceTracker::MAX_TO_TRACK> cutOutChin;
	std::array<bool, FaceTracker::MAX_TO_TRACK> outerLips; // if true, then cut out outer lips contour, otherwise inner lips counter

	const static size_t FACE_FAN_COUNT = 23;
	const static size_t LEFT_EYE_STRIP_COUNT = 6;
	const static size_t RIGHT_EYE_STRIP_COUNT = 6;
	const static size_t LIPS_INNER_STRIP_COUNT = 6;
	const static size_t LIPS_OUTER_STRIP_COUNT = 12;
	const static size_t CHIN_FAN_COUNT = 7;

	static const std::array<int, FACE_FAN_COUNT> face_fan;
	static const std::array<int, LEFT_EYE_STRIP_COUNT> left_eye_strip;
	static const std::array<int, RIGHT_EYE_STRIP_COUNT> right_eye_strip;
	static const std::array<int, LIPS_INNER_STRIP_COUNT> lips_inner_strip;
	static const std::array<int, LIPS_OUTER_STRIP_COUNT> lips_outer_strip;
	static const std::array<int, CHIN_FAN_COUNT> chin_fan;

	AlphaMask();
	~AlphaMask();

	void onInputFrameResized() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

protected:
	shared_ptr<cwc::glShader> shader2D;

	std::array<std::array<ValueSmoother, 66 * 2>, FaceTracker::MAX_TO_TRACK> facePointsSmoothers;

	std::array<Eigen::Vector2f, 24> face_contour;
	std::array<Eigen::Vector2f, 7> left_eye_contour;
	std::array<Eigen::Vector2f, 7> right_eye_contour;
	std::array<Eigen::Vector2f, 7> lips_inner_contour;
	std::array<Eigen::Vector2f, 13> lips_outer_contour;
	std::array<Eigen::Vector2f, 8> chin_contour;

	GLuint faceMask_VBO;
};