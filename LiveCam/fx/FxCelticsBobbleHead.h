#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxCelticsBobbleHead: public FxCore
{
public:
	FxCelticsBobbleHead();
	~FxCelticsBobbleHead();

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	
private:
	void animateNextStep(size_t i);

	std::array<int, FaceTracker::MAX_TO_TRACK> animationCounter;
	std::array<bool, FaceTracker::MAX_TO_TRACK> animationDirection;

	//void draw(OpenGLModel& model, const std::vector<FXModel>& faces);
}; 