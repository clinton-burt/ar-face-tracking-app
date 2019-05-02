#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxDebug : public FxCore
{
public:
	FxDebug();
	~FxDebug();

	void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) override;

	static const cv::Mat_<int> connections;
	static const std::vector<cv::Scalar> colors;
}; 