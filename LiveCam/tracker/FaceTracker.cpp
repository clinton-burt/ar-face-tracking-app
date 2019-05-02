#include "tracker/FaceTracker.h"
#include <array>


#include "../opencv/build/include/opencv2/objdetect/objdetect.hpp"

#include "../opencv/build/include/opencv2/imgproc/imgproc.hpp"

#include "../models/FXModel.h"


FaceCascadeOwner::FaceCascadeOwner()
{
	if (!face_cascade.load("./assets/haarcascades/haarcascade_frontalface_alt.xml")) {
		throw std::exception("Failed to load haarcascade_frontalface_alt");
	};
}


bool detectEyes(cv::CascadeClassifier& eyes_cascade, cv::Mat frame_gray, cv::Rect face)
{
	cv::Mat faceROI = frame_gray(face);

	std::vector<cv::Rect> eyes;
	//-- In each face, detect eyes
	eyes_cascade.detectMultiScale(faceROI, eyes, 1.05, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(4, 4));

	//return eyes.size() >= 2;
	return eyes.size() > 0;

}

FaceTracker::FaceTracker()
	: taskWork(std::make_shared<boost::asio::io_service::work>(taskService))
#ifdef ADDITIONAL_FALSE_FILTER
	, opencvTaskWork(std::make_shared<boost::asio::io_service::work>(opencvTaskService))
#endif
{

	lastTackedValuesTime = boost::posix_time::microsec_clock::universal_time();

#ifdef ADDITIONAL_FALSE_FILTER
	lastTackedOpencvFacesTime = boost::posix_time::microsec_clock::universal_time();
#endif

	//Unfortunately, ULSee tracker can't work in multiple threads
	taskThreadPool.create_thread([this]() {this->taskService.run(); });
	//taskThreadPool.create_thread([this]() {this->taskService.run(); });
	//taskThreadPool.create_thread([this]() {this->taskService.run(); });
	//taskThreadPool.create_thread([this]() {this->taskService.run(); });

#ifdef ADDITIONAL_FALSE_FILTER

	for (size_t i = 0; i < OPENCV_TRACK_THREAD_COUNT; i++)
	{
		opencvTaskThreadPool.create_thread([this]() {this->opencvTaskService.run(); });
	}
#endif
	
}

void FaceTracker::init(std::string modelPath){
	// Start tracker
	qDebug() << "ULS face tracker\n"
		<< "(c) 2016 ULSee Inc. \n"
		<< "All rights reserved\n"
		<< "www.ulsee.com\n\n";

	//Init Tracker with model and key
	int rtnValue = ULS_TrackerInit(const_cast<char*>(modelPath.c_str()), const_cast<char*>(kActivationKey.c_str()), 5, 0, 40);
	
	if (rtnValue == -33){
		qDebug() << "NOT FOUND!!!!";
		return;
	}
	else{
		qDebug() << "GOOD";
	}

	ULS_SetSmooth(false);
	ULS_SetTwoThresholds(0.27f, 0.33f);

	//ULS_SetTwoThresholds(0.25f, 0.25f);
	ULS_SetTwoThresholds(0.25f, 0.28f);

	lastModelPath = modelPath;
}

FaceTracker::~FaceTracker()
{
	taskWork.reset();

#ifdef ADDITIONAL_FALSE_FILTER
	opencvTaskWork.reset();
#endif

	taskThreadPool.join_all();

#ifdef ADDITIONAL_FALSE_FILTER
	opencvTaskThreadPool.join_all();
#endif


	destroy();

	
}

void FaceTracker::destroy()
{
	ULS_TrackerFree();
}


void FaceTracker::reset()
{
	ULS_ResetTracker();
}

void FaceTracker::resetHard()
{
	destroy();
	init(lastModelPath);
}


void FaceTracker::trackerValuesAsync(cv::Mat newNextFrame)
{
	boost::posix_time::ptime frameReceiveTime = boost::posix_time::microsec_clock::universal_time();

	std::unique_lock<std::mutex> lock(nextFrameMutex);

	bool frameWasEmpty = nextFrame.empty();
	
	newNextFrame.copyTo(nextFrame);

	if (frameWasEmpty)
	{
		taskService.post(std::bind(&FaceTracker::innerTrackerValues, this, frameReceiveTime));
	}


#ifdef ADDITIONAL_FALSE_FILTER


	bool opencv¿rameWasEmpty = opencvNextFrame.empty();

	newNextFrame.copyTo(opencvNextFrame);

	if (opencv¿rameWasEmpty)
	{
		opencvTaskService.post(std::bind(&FaceTracker::innerTrackOpenÒv, this, frameReceiveTime));
	}
#endif
	
	
}


void FaceTracker::innerTrackerValues(boost::posix_time::ptime frameReceiveTime)
{


	cv::Mat frame;

	{
		std::unique_lock<std::mutex> lock(nextFrameMutex);

		frame = nextFrame;
		nextFrame = cv::Mat();

		if (frame.empty())
		{
			return;
		}

	}

	do {

		std::vector<FXModel> trackedValues;

		// Get tracker values from tracker..

		trackedValues = trackerValuesFromMatImmediate(frame);

		std::unique_lock<std::mutex> lock(lastTrackedValuesMutex);


		if (frameReceiveTime > lastTackedValuesTime)
		{
			lastTrackedValues = trackedValues;

			frame.copyTo(lastTrackedFrame);

			lastTackedValuesTime = frameReceiveTime;
		}

		
		{
			std::unique_lock<std::mutex> lock(nextFrameMutex);
			frame = nextFrame;
			nextFrame = cv::Mat();
		}
	} while (!frame.empty());

	//std::cout << "";
	
}

#ifdef ADDITIONAL_FALSE_FILTER


void FaceTracker::innerTrackOpenÒv(boost::posix_time::ptime frameReceiveTime)
{


	cv::Mat frame;

	{
		std::unique_lock<std::mutex> lock(nextFrameMutex);

		frame = opencvNextFrame;
		opencvNextFrame = cv::Mat();

		if (frame.empty())
		{
			return;
		}

	}

	do {







		cv::Mat frame_gray, frameSmall;


		cv::resize(frame, frameSmall, cv::Size(frame.cols / 2, frame.rows / 2));

		cvtColor(frameSmall, frame_gray, cv::COLOR_BGR2GRAY);

		std::vector<cv::Rect> faces;

		faceCascadeMap[boost::this_thread::get_id()].face_cascade.detectMultiScale(frame_gray, faces, 1.15, 8, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(20, 20), cv::Size(450, 450));

		for (size_t i = 0; i < faces.size(); i++)
		{
			//cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			//cv::ellipse(prevFrame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(128, 255, 128), 4, 8, 0);

			faces[i].x *= 2;
			faces[i].y *= 2;
			faces[i].width *= 2;
			faces[i].height *= 2;
		}

		{
			std::unique_lock<std::mutex> lock(lastTrackedOpencvFacesMutex);


			if (frameReceiveTime > lastTackedOpencvFacesTime)
			{
				lastTrackedOpencvFaces = faces;

				lastTackedOpencvFacesTime = frameReceiveTime;
			}
			
		}

		{
			std::unique_lock<std::mutex> lock(nextFrameMutex);
			frame = opencvNextFrame;
			opencvNextFrame = cv::Mat();
		}
	} while (!frame.empty());

}

#endif

std::vector<FXModel> FaceTracker::getLastTrackedValues()
{
	std::unique_lock<std::mutex> lock(lastTrackedValuesMutex);

	return lastTrackedValues;
}

std::pair<std::vector<FXModel>, cv::Mat> FaceTracker::getLastTrackedValuesAndFrame()
{
	std::unique_lock<std::mutex> lock(lastTrackedValuesMutex);

	std::pair<std::vector<FXModel>, cv::Mat> result = { lastTrackedValues, lastTrackedFrame };

	//lastTrackedFrame = cv::Mat();
	return result;
}


#ifdef ADDITIONAL_FALSE_FILTER

std::vector<cv::Rect> FaceTracker::getLastTrackedOpencvFaces()
{
	std::unique_lock<std::mutex> lock(lastTrackedOpencvFacesMutex);

	return lastTrackedOpencvFaces;
}
#endif


std::vector<FXModel> FaceTracker::trackerValuesFromMatImmediate(cv::Mat frame) {
	
	// Get tracker values from tracker..

	float values[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES];
	const float* rawPoints[FaceTracker::MAX_TO_TRACK][2];
	const float* confidence[FaceTracker::MAX_TO_TRACK];


	std::vector<FXModel> result;

	int rtnstate;
	
	//process face tracking
#ifdef USE_OLD_RECOGNIZER

	rtnstate = ULS_TrackerProcess(&frame);
#else

	float yawAngle;

	rtnstate = ULS_TrackerProcessByte((void*)frame.data, frame.cols, frame.rows, 1, 0, &yawAngle);
	
#endif
	//draw results
	if (rtnstate < 0) {
		qDebug() << "state:" << rtnstate;
		values[0][0] = -1.0f;
		return {};
	}
	else if (rtnstate >= 0) {
		int n = ULS_GetTrackerPointNum();
		for (int j = 0; j < MAX_TO_TRACK; j++) {

			//	rawPoints[j] = NULL;
			const float* pts = ULS_GetTrackerPoint(j);

			const float* pts3d = ULS_GetTrackerPoint3D(j);

			if (pts == NULL) {
				values[j][POINT_ID] = -1.0f;
				continue;
			}

			int fx, fy, fw, fh;

			bool faceRectResult = ULS_GetTrackerFacerect(j, fx, fy, fw, fh);

			if (!faceRectResult)
			{
				values[j][POINT_ID] = -1.0f;
				continue;
			}

			// face center
			float imx, imy;
			//ULS_GetFaceCenter(imx, imy, j);

#if 0
			// without pose stability
			float pitch = ULS_GetPitchRadians(j);
			float yaw = ULS_GetYawRadians(j);
			float roll = ULS_GetRollRadians(j);
#else
			// with pose stability
			float pitch = ULS_GetStablePitchRadians(j);
			float yaw = ULS_GetStableYawRadians(j);
			float roll = ULS_GetStableRollRadians(j);
#endif			
			cv::Point2f pupils[2];
			ULS_GetLeftPupilXY(pupils[0].x, pupils[0].y, j);
			ULS_GetRightPupilXY(pupils[1].x, pupils[1].y, j);

			cv::Point3f gazes[2];
			ULS_GetLeftGaze(gazes[0].x, gazes[0].y, gazes[0].z, j);
			ULS_GetRightGaze(gazes[1].x, gazes[1].y, gazes[1].z, j);

			//if (yaw > 45 || yaw < -45) return{};
			
			// project head center in world coordinate to image plane
			//	float nx, ny;
			float hx, hy, hz;
			ULS_GetHead3DPosition(hx, hy, hz, j);

			float hxCC, hyCC, hzCC;
			ULS_GetHead3DPositionCC(hxCC, hyCC, hzCC, j);

			float faceScale;
			faceScale = ULS_GetScaleInImage(j);

			int lX = pts[0];
			int ly = pts[1];
			int rX = pts[32];
			int ry = pts[33];

			// #1 = pts[2 * i], #2 = pts[2 * i + 1]
			// point # 28
			int noseX = pts[56];
			int noseY = pts[57];

			int lipX = pts[102];
			int lipY = pts[103];

			values[j][TOP_LIP_CENTER_X] = lipX;
			values[j][TOP_LIP_CENTER_Y] = lipY;

			values[j][FACE_CENTER_X] = noseX;
			values[j][FACE_CENTER_Y] = noseY;
			values[j][PITCH] = pitch;
			values[j][YAW] = yaw;
			values[j][ROLL] = roll;
			values[j][LEFT_PUPIL_X] = pupils[0].x;
			values[j][LEFT_PUPIL_Y] = pupils[0].y;
			values[j][RIGHT_PUPIL_X] = pupils[1].x;
			values[j][RIGHT_PUPIL_Y] = pupils[1].y;
			values[j][LEFT_GAZE_X] = gazes[0].x;
			values[j][LEFT_GAZE_Y] = gazes[0].y;
			values[j][LEFT_GAZE_Z] = gazes[0].z;
			values[j][RIGHT_GAZE_X] = gazes[1].x;
			values[j][RIGHT_GAZE_Y] = gazes[1].y;
			values[j][RIGHT_GAZE_Z] = gazes[1].z;
			values[j][FACE_WIDTH] = PointCalcUtil::distanceBetween(int(pts[0]), int(pts[1]), int(pts[32]), int(pts[33]));
			values[j][POINT_ID] = (float)j;
			values[j][POINT_TOTAL] = (float)n;
			values[j][HEAD_3D_POS_X] = hx;
			values[j][HEAD_3D_POS_Y] = hy;
			values[j][HEAD_3D_POS_Z] = hz;
			values[j][HEAD_3D_POS_CC_X] = hxCC;
			values[j][HEAD_3D_POS_CC_Y] = hyCC;
			values[j][HEAD_3D_POS_CC_Z] = hzCC;
			values[j][FACE_SCALE] = faceScale;

			values[j][FACE_RECT_X] = fx;
			values[j][FACE_RECT_Y] = fy;

			values[j][FACE_RECT_WIDTH] = fw;
			values[j][FACE_RECT_HEIGHT] = fh;

			values[j][LIP_BOTTOM_CENTER_X] = pts[114];
			values[j][LIP_BOTTOM_CENTER_Y] = pts[115];

			// rawPoints set--
			rawPoints[j][RAW_POINTS] = pts;
			rawPoints[j][RAW_3D_POINTS] = pts3d;


		}

	}


	result = FXModel::CreateFxModelArrFromPoints(frame.cols, frame.rows, rawPoints, values, confidence);

	return result;
}

bool FaceTracker::trackerValuesFromMat(cv::Mat frame, const float* rawPoints[FaceTracker::MAX_TO_TRACK], float values[][NUMBER_OF_PRESET_VALUES], const float* confidence[FaceTracker::MAX_TO_TRACK]) {
	bool isTracking = false;
	//process face tracking

#ifdef USE_OLD_RECOGNIZER
	int rtnstate = ULS_TrackerProcess(&frame);
#else
	float yawAngle;

	int rtnstate = ULS_TrackerProcessByte((void*)frame.data, frame.cols, frame.rows, 1, 0, &yawAngle);
#endif
	//draw results
	if (rtnstate < 0) {
		qDebug() << "state:" << rtnstate;
		values[0][0] = -1.0f;
		return false;
	}
	else if (rtnstate >= 0) {
		int n = ULS_GetTrackerPointNum();


		
		for (int j = 0; j < MAX_TO_TRACK; j++) {
			
			rawPoints[j] = NULL;

			const float* pts = ULS_GetTrackerPoint(j);

			const float* pts3d = ULS_GetTrackerPoint3D(j);

			if (pts == NULL) {
				values[j][POINT_ID] = -1.0f;
				continue;
			}

			int fx, fy, fw, fh;

			bool faceRectResult = ULS_GetTrackerFacerect(j, fx, fy, fw, fh);

			if (!faceRectResult)
			{
				values[j][POINT_ID] = -1.0f;
				continue;
			}
			
			isTracking = true;
			// face center
			float imx, imy;
			ULS_GetFaceCenter(imx, imy, j);			
#if 0
			// without pose stability
			float pitch = ULS_GetPitchRadians(j);
			float yaw = ULS_GetYawRadians(j);
			float roll = ULS_GetRollRadians(j);
#else
			// with pose stability
			float pitch = ULS_GetStablePitchRadians(j);
			float yaw = ULS_GetStableYawRadians(j);
			float roll = ULS_GetStableRollRadians(j);
#endif			
			cv::Point2f pupils[2];
			ULS_GetLeftPupilXY(pupils[0].x, pupils[0].y, j);
			ULS_GetRightPupilXY(pupils[1].x, pupils[1].y, j);

			cv::Point3f gazes[2];
			ULS_GetLeftGaze(gazes[0].x, gazes[0].y, gazes[0].z, j);
			ULS_GetRightGaze(gazes[1].x, gazes[1].y, gazes[1].z, j);

			// project head center in world coordinate to image plane
			//	float nx, ny;
			float hx, hy, hz;
			ULS_GetHead3DPosition(hx, hy, hz, j);

			float hxCC, hyCC, hzCC;
			ULS_GetHead3DPositionCC(hxCC, hyCC, hzCC, j);
						
			int lX = pts[0];
			int ly = pts[1];
			int rX = pts[32];
			int ry = pts[33];

			// #1 = pts[2 * i], #2 = pts[2 * i + 1]
			// point # 28
			int noseX = pts[56];
			int noseY = pts[57];
			
			int lipX = pts[102];
			int lipY = pts[103];

			values[j][TOP_LIP_CENTER_X] = lipX;
			values[j][TOP_LIP_CENTER_Y] = lipY;

			values[j][FACE_CENTER_X] = noseX;
			values[j][FACE_CENTER_Y] = noseY;
			values[j][PITCH] = pitch;
			values[j][YAW] = yaw;
			values[j][ROLL] = roll;
			values[j][LEFT_PUPIL_X] = pupils[0].x;
			values[j][LEFT_PUPIL_Y] = pupils[0].y;
			values[j][RIGHT_PUPIL_X] = pupils[1].x;
			values[j][RIGHT_PUPIL_Y] = pupils[1].y;
			values[j][LEFT_GAZE_X] = gazes[0].x;
			values[j][LEFT_GAZE_Y] = gazes[0].y;
			values[j][RIGHT_GAZE_X] = gazes[1].x;
			values[j][RIGHT_GAZE_Y] = gazes[1].y;
			values[j][FACE_WIDTH] = PointCalcUtil::distanceBetween(int(pts[0]), int(pts[1]), int(pts[32]), int(pts[33]));
			values[j][POINT_ID] = (float)j;
			values[j][POINT_TOTAL] = (float)n;
			rawPoints[j] = pts;
			values[j][HEAD_3D_POS_X] = hx;
			values[j][HEAD_3D_POS_Y] = hy;
			values[j][HEAD_3D_POS_Z] = hz;
			values[j][HEAD_3D_POS_CC_X] = hxCC;
			values[j][HEAD_3D_POS_CC_Y] = hyCC;
			values[j][HEAD_3D_POS_CC_Z] = hzCC;

			values[j][FACE_RECT_X] = fx;
			values[j][FACE_RECT_Y] = fy;

			values[j][FACE_RECT_WIDTH] = fw;
			values[j][FACE_RECT_HEIGHT] = fh;

		}
	}
	return true;
}