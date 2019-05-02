#include <CVImageUtil.h>

const float RED_INTENSITY = 0.3f;
const float GREEN_INTENSITY = 0.59f;
const float BLUE_INTENSITY = 0.11f;

cv::Mat CVImageUtil::rotateAndSize(cv::Mat src, double angle, double width, cv::Point2f rotateCenter)
{
	cv::Mat dst;
	double cWidthPercent = width / src.cols;
	double newHeight = src.rows * cWidthPercent;

	cv::resize(src, dst, cv::Size(width, newHeight), 1, 1);

	cv::Point2f pt(dst.cols * rotateCenter.x, dst.rows * rotateCenter.y);
	cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

	// determine bounding rectangle
	cv::Rect bbox = cv::RotatedRect(pt, dst.size(), angle).boundingRect();

	// adjust transformation matrix
	r.at<double>(0, 2) += bbox.width / 2.0 - pt.x;
	r.at<double>(1, 2) += bbox.height / 2.0 - pt.y;

	cv::warpAffine(dst, dst, r, bbox.size());
	return dst;
}

void CVImageUtil::overlayImage(cv::Mat src, cv::Mat overlay, const cv::Point& location)
{
	for (int y = cv::max(location.y, 0); y < src.rows; ++y)
	{
		int fY = y - location.y;

		if (fY >= overlay.rows)
			break;

		for (int x = cv::max(location.x, 0); x < src.cols; ++x)
		{
			int fX = x - location.x;

			if (fX >= overlay.cols)
				break;

			double opacity = ((double)overlay.data[fY * overlay.step + fX * overlay.channels() + 3]) / 255;

			for (int c = 0; opacity > 0 && c < src.channels(); ++c)
			{
				unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
				unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
				src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
			}
		}
	}
}

void CVImageUtil::overlayImageMaskWithColorCorrection(cv::Mat src, cv::Mat srcBlurred, cv::Mat overlay, cv::Mat overlayBlurred, cv::Mat overlayMask, const cv::Point& location)
{

	for (int y = cv::max(location.y, 0); y < src.rows; ++y)
	{
		int fY = y - location.y;

		if (fY >= overlay.rows)
			break;

		for (int x = cv::max(location.x, 0); x < src.cols; ++x)
		{
			int fX = x - location.x;

			if (fX >= overlay.cols)
				break;

			double opacity = (unsigned char)(overlayMask.data[fY * overlayMask.step + fX]) / 255.f;

			for (int c = 0; opacity > 0 && c < src.channels(); ++c)
			{
				unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
				unsigned char overlayBlurredPx = overlayBlurred.data[fY * overlay.step + fX * overlay.channels() + c];

				unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
				unsigned char srcBlurredPx = srcBlurred.data[y * src.step + x * src.channels() + c];

				double overlayBlurredPxDouble = overlayBlurredPx == 0 ? 1.0 : (double)overlayBlurredPx;

				unsigned int overlayCorrectedPxInt = (overlayPx / overlayBlurredPxDouble) * srcBlurredPx;

				unsigned char overlayCorrectedPx = overlayCorrectedPxInt > 255 ? 255 : (unsigned char)overlayCorrectedPxInt;

				src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayCorrectedPx * opacity;
			}
		}
	}
}



void CVImageUtil::makeColorCorrection(cv::Mat& result, cv::Mat src, cv::Mat srcBlurred, cv::Mat overlay, cv::Mat overlayBlurred, const cv::Point& location)
{

	for (int y = cv::max(location.y, 0); y < src.rows; ++y)
	{
		int fY = y - location.y;

		if (fY >= overlay.rows)
			break;

		for (int x = cv::max(location.x, 0); x < src.cols; ++x)
		{
			int fX = x - location.x;

			if (fX >= overlay.cols)
				break;

			for (int c = 0; c < src.channels(); ++c)
			{
				unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
				unsigned char overlayBlurredPx = overlayBlurred.data[fY * overlay.step + fX * overlay.channels() + c];

				unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
				unsigned char srcBlurredPx = srcBlurred.data[y * src.step + x * src.channels() + c];


				/*
				double overlayBlurredPxDouble = overlayBlurredPx == 0 ? 1.0 : (double)overlayBlurredPx;

				unsigned int overlayCorrectedPxInt = (overlayPx / overlayBlurredPxDouble) * srcBlurredPx;

				unsigned char overlayCorrectedPx = overlayCorrectedPxInt > 255 ? 255 : (unsigned char)overlayCorrectedPxInt;

				result.data[y * src.step + src.channels() * x + c] = overlayCorrectedPx;*/

				double overlayBlurredPxDouble = srcBlurredPx == 0 ? 1.0 : (double)srcBlurredPx;

				unsigned int overlayCorrectedPxInt = (srcPx / overlayBlurredPxDouble) * overlayBlurredPx;

				unsigned char overlayCorrectedPx = overlayCorrectedPxInt > 255 ? 255 : (unsigned char)overlayCorrectedPxInt;

				result.data[y * src.step + src.channels() * x + c] = overlayCorrectedPx;

			}
		}
	}
}


void CVImageUtil::addImage(cv::Mat src, cv::Mat overlay, const cv::Point& location)
{
	for (int y = cv::max(location.y, 0); y < src.rows; ++y)
	{
		int fY = y - location.y;

		if (fY >= overlay.rows)
			break;

		for (int x = cv::max(location.x, 0); x < src.cols; ++x)
		{
			int fX = x - location.x;

			if (fX >= overlay.cols)
				break;



			for (int c = 0; c < src.channels(); ++c)
			{
				unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
				unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];

				if (int(overlayPx) + int(srcPx) >= 255)
				{
					src.data[y * src.step + src.channels() * x + c] = 255;
				}
				else
				{
					src.data[y * src.step + src.channels() * x + c] = srcPx + overlayPx;
				}


			}
		}
	}

}

void updateLimits(cv::Point2i& leftTop, cv::Point2i& bottomRight, cv::Point2f& point, cv::Size2i& frameSize)
{
	if (point.x < leftTop.x) leftTop.x = point.x;
	if (point.y < leftTop.y) leftTop.y = point.y;
	if (point.x > bottomRight.x) bottomRight.x = point.x;
	if (point.y > bottomRight.y) bottomRight.y = point.y;

	if (leftTop.x < 0) leftTop.x = 0;
	if (leftTop.y < 0) leftTop.y = 0;
	if (bottomRight.x >= frameSize.width) bottomRight.x = frameSize.width - 1;
	if (bottomRight.y >= frameSize.height) bottomRight.y = frameSize.height - 1;
}

FaceContours CVImageUtil::extractFaceContourInFrame(FXModel& face)
{
	FaceContours result;

	cv::Size2i frameSize = { face.fWidth, face.fHeight };
	cv::Point2i leftTop = { face.fWidth, face.fHeight };
	cv::Point2i bottomRight = { 0, 0 };

	for (int i = 0; i < 17; ++i)
	{
		auto point = cv::Point2f((face.pts[i * 2]), (face.pts[i * 2 + 1]));
		updateLimits(leftTop, bottomRight, point, frameSize);
		result.face.push_back(point);
	}

	int index = 17;
	for (int i = 26; i > 16; --i)
	{
		auto point = cv::Point2f((face.pts[i * 2]), (face.pts[i * 2 + 1]));
		updateLimits(leftTop, bottomRight, point, frameSize);
		result.face.push_back(point);
	}

	result.face.push_back(result.face.front());

	result.shift = leftTop;
	result.size = bottomRight - leftTop;

	for (auto &point : result.face)
	{
		point.x = point.x - result.shift.x;
		point.y = point.y - result.shift.y;
	}

	for (int i = 36; i < 42; ++i)
	{
		result.leftEye.push_back(cv::Point2f(face.pts[i * 2] - result.shift.x, face.pts[i * 2 + 1] - result.shift.y));
	}
	result.leftEye.push_back(result.leftEye.front());

	for (int i = 42; i < 48; ++i)
	{
		result.rightEye.push_back(cv::Point2f(face.pts[i * 2] - result.shift.x, face.pts[i * 2 + 1] - result.shift.y));
	}
	result.rightEye.push_back(result.rightEye.front());

	for (int i = 48; i < 60; ++i)
	{
		result.mouth.push_back(cv::Point2f(face.pts[i * 2] - result.shift.x, face.pts[i * 2 + 1] - result.shift.y));
	}
	result.mouth.push_back(result.mouth.front());

	return result;
}

const float CVImageUtil::FrameLightParams::STD_FRAME_BRIGHTNESS = 0.9f;

CVImageUtil::FrameLightParams CVImageUtil::getFrameLightParams(cv::Mat& frame)
{
	FrameLightParams result;

	float sum = 0;
	float del = 0;

	result.light = { 0, 0, 0 };

	const int stepX = 8;
	const int stepY = 8;

	for (int row = 0; row < frame.rows; row += stepY)
	{
		for (int col = 0; col < frame.cols; col += stepX)
		{
			auto pixel = frame.at<cv::Vec3b>(row, col);
			float x = (pixel[0] * BLUE_INTENSITY + pixel[1] * GREEN_INTENSITY + pixel[2] * RED_INTENSITY) / 255.f;
			float y = 3.3f * x * x - 1;
			if (y < 0) y = 0;
			if (y > 2) y = 2;
			result.light += pixel * y;
			sum += x * y;
			del += y;
		}
	}

	if (del > 0)
	{
		result.light /= del * 255;
		result.light /= cv::max(result.light[0], cv::max(result.light[1], result.light[2]));

		const float STD_FRAME_BRIGHTNESS = CVImageUtil::FrameLightParams::STD_FRAME_BRIGHTNESS;
		result.brightness = sum / del;
		result.brightness = STD_FRAME_BRIGHTNESS + (result.brightness - STD_FRAME_BRIGHTNESS) * 3; // difference enhancement
	}
	else
	{
		result.light = { 1, 1, 1 };
		result.brightness = 0;
	}


	return result;
}

CVImageUtil::FaceLightParams CVImageUtil::getFaceLightParams(cv::Mat& frame, FaceContours& contours, FXModel& face)
{
	FaceLightParams result;

	cv::Mat1b mask(contours.size.height, contours.size.width, uchar(0));

	cv::polylines(mask, contours.face, false, cv::Scalar(255));
	cv::polylines(mask, contours.leftEye, false, cv::Scalar(255));
	cv::polylines(mask, contours.rightEye, false, cv::Scalar(255));
	cv::polylines(mask, contours.mouth, false, cv::Scalar(255));

	cv::Point center = { (int)face.pts[66] - contours.shift.x, (int)face.pts[67] - contours.shift.y };

	if (center.x >= 0 && center.x < mask.cols && center.y >= 0 && center.y < mask.rows)
	{
		cv::floodFill(mask, center, cv::Scalar(255));

		float sum = 0;
		int del = 0;

		cv::Mat faceArea = frame(cv::Rect(contours.shift, contours.size));

		const int stepX = 8;
		const int stepY = 8;

		for (int row = 0; row < faceArea.rows; row += stepY)
		{
			for (int col = 0; col < faceArea.cols; col += stepX)
			{
				if (mask.at<uchar>(row, col) > 0)
				{
					cv::Vec3b pixel = faceArea.at<cv::Vec3b>(row, col);
					sum += (pixel[0] * 0.11f + pixel[1] * 0.59f + pixel[2] * 0.3f);
					++del;
				}
			}
		}

		result.brightness = sum / del / 255.f;

	}
	else
	{
		//Center is outside of screen, do nothing...
	}

	return result;
}

void CVImageUtil::overlayImage2(cv::Mat src, cv::Mat overlay, int xVal, int yVal)
{
	for (int y = cv::max(yVal, 0); y < src.rows; ++y)
	{
		int fY = y - yVal;

		if (fY >= overlay.rows)
			break;

		for (int x = cv::max(xVal, 0); x < src.cols; ++x)
		{
			int fX = x - xVal;

			if (fX >= overlay.cols)
				break;

			double opacity = ((double)overlay.data[fY * overlay.step + fX * overlay.channels() + 3]) / 255;

			for (int c = 0; opacity > 0 && c < src.channels(); ++c)
			{
				unsigned char overlayPx = overlay.data[fY * overlay.step + fX * overlay.channels() + c];
				unsigned char srcPx = src.data[y * src.step + x * src.channels() + c];
				src.data[y * src.step + src.channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
			}
		}
	}
}
