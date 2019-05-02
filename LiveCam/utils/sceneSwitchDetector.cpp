#include "sceneSwitchDetector.h"


static const double CHANGE_DETECT_RATIO = 15.0;

double getPSNR(const cv::Mat& I1, const cv::Mat& I2)
{
	cv::Mat s1;
	cv::absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2
	cv::Scalar s = sum(s1);        // sum elements per channel
	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;

	double mse = sse / (double)(I1.channels() * I1.total());
	double psnr = 10.0*log10((255 * 255) / mse);
	return psnr;
}


bool SceneSwitchDetector::sceneSwitched(cv::Mat newFrame)
{
	bool result = false;
	double psnrV;
	if ((prevFrame.cols == newFrame.cols) && (prevFrame.rows == newFrame.rows))
	{

		psnrV = getPSNR(prevFrame, newFrame);

		if (psnrV < CHANGE_DETECT_RATIO)
		{
			frameCounter++;

			result = true;

		}
	}
	else
	{
		result = true;
	}

	newFrame.copyTo(prevFrame);

	return result;
}


	//cv::Mat lastFrame;


