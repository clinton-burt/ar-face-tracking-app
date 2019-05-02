#pragma once

#include <fx\Fx3dModelAncestor.h>


class FxYankeesBobbleBoy : public FxCore
{
public:
	FxYankeesBobbleBoy();
	~FxYankeesBobbleBoy();


	int maxMovementBoy = 9;
	void animateNextStep(size_t i);

	std::array<int, FaceTracker::MAX_TO_TRACK> animationCounter;
	std::array<bool, FaceTracker::MAX_TO_TRACK> animationDirection;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;

}; 