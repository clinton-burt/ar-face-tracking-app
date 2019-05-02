#ifndef CV_IMAGE_OBJECT
#define CV_IMAGE_OBJECT

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>

#include <functional>

#include <models/FxModel.h>

extern const float RED_INTENSITY;
extern const float GREEN_INTENSITY;
extern const float BLUE_INTENSITY;

struct FaceContours
{
	std::vector<cv::Point2i> face;
	std::vector<cv::Point2i> leftEye;
	std::vector<cv::Point2i> rightEye;
	std::vector<cv::Point2i> mouth;

	cv::Point2i shift;
	cv::Size2i size;
};

class CVImageUtil
{

public:
	static cv::Mat rotateAndSize(cv::Mat src, double angle, double width, cv::Point2f rotateCenter = cv::Point2f(0.5, 0.5));
	static void overlayImage(cv::Mat src, cv::Mat overlay, const cv::Point& location);
	static void overlayImageMaskWithColorCorrection(cv::Mat src, cv::Mat srcBlurred, cv::Mat overlay, cv::Mat overlayBlurred, cv::Mat overlayMask, const cv::Point& location);
	static void makeColorCorrection(cv::Mat& result, cv::Mat src, cv::Mat srcBlurred, cv::Mat overlay, cv::Mat overlayBlurred, const cv::Point& location);
	static void addImage(cv::Mat src, cv::Mat overlay, const cv::Point& location);
	static FaceContours extractFaceContourInFrame(FXModel& face);
	static void overlayImage2(cv::Mat src, cv::Mat overlay, int xVal, int yVal);

	struct FrameLightParams
	{
		static const float STD_FRAME_BRIGHTNESS;

		float brightness = 0.f;
		cv::Vec3f light = cv::Vec3f(0.f, 0.f, 0.f);
	};

	struct FaceLightParams
	{
		float brightness = 0.f;
	};

	static FrameLightParams getFrameLightParams(cv::Mat& frame);
	static FaceLightParams getFaceLightParams(cv::Mat& frame, FaceContours& contours, FXModel& face);
};

#endif