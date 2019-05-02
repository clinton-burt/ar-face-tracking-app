#include <fx\FxRavensFacePaintAncestor.h>

class FxFacePaintRavens : public FxCore
{
public:
	FxFacePaintRavens();
	~FxFacePaintRavens();

	virtual void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel);
};