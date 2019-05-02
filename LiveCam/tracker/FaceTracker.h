#pragma once

#include <iostream> 
#include <unordered_map>
#include <qdebug.h>

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opencv2\opencv.hpp>

#include "utils/stdafx.h"
#include "utils/PointCalcUtil.h"

#include "libTracker.h"


//#include "../models/FXModel.h"
class FXModel;

#ifdef ADDITIONAL_FALSE_FILTER
constexpr int OPENCV_TRACK_THREAD_COUNT = 2;
#endif

class FaceCascadeOwner
{
public:

	FaceCascadeOwner();

	cv::CascadeClassifier face_cascade;
};

class FaceTracker
{
public:
	FaceTracker();
	~FaceTracker();

#ifdef ADDITIONAL_FALSE_FILTER
	std::map<boost::thread::id, FaceCascadeOwner> faceCascadeMap;
#endif

	boost::asio::io_service taskService;
	std::shared_ptr<boost::asio::io_service::work> taskWork;
	boost::thread_group taskThreadPool;


#ifdef ADDITIONAL_FALSE_FILTER
	boost::asio::io_service opencvTaskService;
	std::shared_ptr<boost::asio::io_service::work> opencvTaskWork;
	boost::thread_group opencvTaskThreadPool;
#endif

	std::mutex nextFrameMutex;
	cv::Mat nextFrame;

#ifdef ADDITIONAL_FALSE_FILTER
	cv::Mat opencvNextFrame;
#endif

	std::mutex lastTrackedValuesMutex;
	std::vector<FXModel> lastTrackedValues;
	cv::Mat lastTrackedFrame;
	boost::posix_time::ptime lastTackedValuesTime;



#ifdef ADDITIONAL_FALSE_FILTER

	

	std::mutex lastTrackedOpencvFacesMutex;
	std::vector<cv::Rect> lastTrackedOpencvFaces;
	boost::posix_time::ptime lastTackedOpencvFacesTime;

	std::vector<cv::Rect> getLastTrackedOpencvFaces();
	void innerTrackOpenñv(boost::posix_time::ptime frameReceiveTime);


#endif

	void trackerValuesAsync(cv::Mat nextFrame);
	void innerTrackerValues(boost::posix_time::ptime frameReceiveTime);
	std::vector<FXModel> getLastTrackedValues();

	
	std::pair<std::vector<FXModel>, cv::Mat> getLastTrackedValuesAndFrame();

	std::vector<FXModel> savedValues;

	const static int MAX_TO_TRACK = 5;

	// if you add more tracker values, you must update this number with the total amount values.
	const static int NUMBER_OF_PRESET_VALUES = 33; // not including raw points

	const static int FACE_CENTER_X = 0;
	const static int FACE_CENTER_Y = 1;
	const static int PITCH = 2;
	const static int YAW = 3;
	const static int ROLL = 4;
	const static int LEFT_PUPIL_X = 5;
	const static int LEFT_PUPIL_Y = 6;
	const static int RIGHT_PUPIL_X = 7;
	const static int RIGHT_PUPIL_Y = 8;
	const static int LEFT_GAZE_X = 9;
	const static int LEFT_GAZE_Y = 10;
	const static int RIGHT_GAZE_X = 11;
	const static int RIGHT_GAZE_Y = 12;
	const static int FACE_WIDTH = 13;
	const static int POINT_ID = 14;
	const static int POINT_TOTAL = 15;
	const static int TOP_LIP_CENTER_X = 16;
	const static int TOP_LIP_CENTER_Y = 17;
	// Functionality: Get the 3D position of head center in world coordinate -- see doc
	const static int HEAD_3D_POS_X = 18;
	const static int HEAD_3D_POS_Y = 19;
	const static int HEAD_3D_POS_Z = 20;
	//Functionality: Get the 3D position of head center in camera coordinate -- see doc
	const static int HEAD_3D_POS_CC_X = 21;
	const static int HEAD_3D_POS_CC_Y = 22;
	const static int HEAD_3D_POS_CC_Z = 23;
	const static int FACE_SCALE = 24;

	const static int FACE_RECT_X = 25;
	const static int FACE_RECT_Y = 26;
	const static int FACE_RECT_WIDTH = 27;
	const static int FACE_RECT_HEIGHT = 28;

	const static int LEFT_GAZE_Z = 29;
	const static int RIGHT_GAZE_Z = 30;

	const static int LIP_BOTTOM_CENTER_X = 31;
	const static int LIP_BOTTOM_CENTER_Y = 32;

	// different set of presets, revolving around the const float*
	const static int RAW_POINTS = 0;
	const static int RAW_3D_POINTS = 1;
	
	void init(std::string assetPath);

	bool trackerValuesFromMat(cv::Mat frame, const float* rawPoints[FaceTracker::MAX_TO_TRACK], float values[][NUMBER_OF_PRESET_VALUES], const float* confidence[FaceTracker::MAX_TO_TRACK]);
	std::vector<FXModel> trackerValuesFromMatImmediate(cv::Mat frame);

	cv::Mat drawComposites(cv::Mat frame, int currentEffect);
	void setEffectModule(int value);
	void reset();

	void resetHard();
	void destroy();

private:
	const std::string kActivationKey = "e9f7xxphqp7F9TWcWJCouzFpbfQozp1I";

	std::string lastModelPath = "./model/";
};