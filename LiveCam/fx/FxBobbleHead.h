#pragma once

#include <fx\Fx3dModelAncestor.h>

constexpr int maxMovement = 9;

class FxBobbleHead : public FxCore
{
public:
	FxBobbleHead();
	~FxBobbleHead();

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;

private:

	cv::Mat texture1;

	GLuint hairTextureID;
	GLuint headTextureID;
	
	cv::Point2f newFaceStd;
	cv::Point2f newFaceCenter;

	std::shared_ptr<cwc::glShader> regularShader;
	std::shared_ptr<cwc::glShader> faceMixShader;

	std::array<int, FaceTracker::MAX_TO_TRACK> animationCounter;
	std::array<bool, FaceTracker::MAX_TO_TRACK> animationDirection;

	GLuint normalTextureId = 0;

	void animateNextStep(size_t i);

	void calcNewFaceCenterAndStd();

	cv::Point2f calcFaceStd(const std::array<float, 66 * 2>& pts);

}; 