#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxCrowsFootball: public FxCore
{
public:
	FxCrowsFootball();
	~FxCrowsFootball();

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	std::array<float, 3> bmin = { 0, 0, 0 };
	std::array<float, 3> bmax = { 0, 0, 0 };
	float modelScale = 1; // head-relative scale
	Eigen::Vector3f modelShift = { 0, 0, 0 }; // head-relative shift

	float depth = 0;
	//void onInputFrameResized() override;

	//std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> lipsDistanceSmoother;

}; 