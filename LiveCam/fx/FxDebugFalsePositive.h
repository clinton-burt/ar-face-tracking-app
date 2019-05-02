#include <fx\FxCore.h>

class FxDebugFalsePositive
{
public:
	static void draw2D(cv::Mat frame, FXModel& fxModel);

	static const cv::Mat_<int> connections;
	static const std::vector<cv::Scalar> colors;
};