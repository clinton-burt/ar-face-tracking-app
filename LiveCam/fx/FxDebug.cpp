#include <fx/FxDebug.h>

#include <utils/DrawUtil.h>

const cv::Mat_<int> FxDebug::connections =
(cv::Mat_<int>(61, 2) <<
	0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
	10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 17, 18,
	18, 19, 19, 20, 20, 21, 22, 23, 23, 24, 24, 25, 25, 26,
	27, 28, 28, 29, 29, 30, 31, 32, 32, 33, 33, 34, 34, 35,
	36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 36, 42, 43,
	43, 44, 44, 45, 45, 46, 46, 47, 47, 42, 48, 49, 49, 50,
	50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57,
	57, 58, 58, 59, 59, 48, 60, 65, 60, 61, 61, 62, 62, 63,
	63, 64, 64, 65);

const std::vector<cv::Scalar> FxDebug::colors = {
	CV_RGB(0, 153, 204),
	CV_RGB(255, 255, 0),
	CV_RGB(0, 0, 255),
	CV_RGB(102, 102, 102),
	CV_RGB(255, 0, 0),
	CV_RGB(0, 255, 255)
};


FxDebug::FxDebug()
{
	auto model = make_shared<OpenGL3DModel>();

	models.push_back(model);
}

FxDebug::~FxDebug()
{

}

void FxDebug::drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) // just draw this directly from OpenGLMain::drawBackground
{
	const std::array<float, 66 * 2>& pts = fxModel.pts;

	cv::Point2f left_pupil(fxModel.pupilLX, fxModel.pupilLY);
	cv::Point2f right_pupil(fxModel.pupilRX, fxModel.pupilRY);
	if ((left_pupil.x >= 0) && (left_pupil.y >= 0) &&
		(right_pupil.x >= 0) && (right_pupil.y >= 0))
	{
		float r = static_cast<float>(cv::max(2.0, 0.075 * cv::norm(right_pupil - left_pupil)));
		DrawUtil::drawPoint(left_pupil.x, left_pupil.y, r, frame, colors[0], true);
		DrawUtil::drawPoint(right_pupil.x, right_pupil.y, r, frame, colors[0], true);
	}

	for (int i = 0; i < fxModel.pointTotal; i++) {
		DrawUtil::drawPointAA(pts[2 * i], pts[2 * i + 1], 3.0f, frame, CV_RGB(255, 255, 255), true);
	}
	for (int i = 0; i < connections.rows; i++) {
		int i1 = connections(i, 0), i2 = connections(i, 1);
		DrawUtil::drawLine(pts[2 * i1], pts[2 * i1 + 1],
			pts[2 * i2], pts[2 * i2 + 1],
			frame, CV_RGB(0, 0, 0));
	}

	// draw average health of estimation
	/*float health = 0.0f;
	for (int i = 0; i < numberOfPoints; i++) {
	health += con[i];
	}
	char str[256];*/

	// draw pupils
	

	//draw gaze
	cv::Point2f leftGaze(fxModel.gazeLX, fxModel.gazeLY);
	//ULS_GetLeftGaze(dir.x, dir.y, dir.z, j);
	cv::Point2f e = left_pupil + cv::Point2f(leftGaze.x, leftGaze.y) * 20;
	cv::line(frame, left_pupil, e, colors[1], 3, CV_AA);

	cv::Point2f rightGaze(fxModel.gazeRX, fxModel.gazeRY);
	//	ULS_GetRightGaze(dir.x, dir.y, dir.z, j);
	e = right_pupil + cv::Point2f(rightGaze.x, rightGaze.y) * 20;
	cv::line(frame, right_pupil, e, colors[1], 3, CV_AA);

	// cv::rectangle(img, right_pupil, e, CV_RGB(255, 255, 0), 1, CV_AA);
	DrawUtil::drawPoint(fxModel.xFaceCenterRaw, fxModel.yFaceCenterRaw, 3, frame, colors[0], false);
}