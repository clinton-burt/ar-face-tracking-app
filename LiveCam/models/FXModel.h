#ifndef FXModel_H
#define FXModel_H


#include <array>
#include <opencv2/core/core.hpp>

#include "../tracker/FaceTracker.h"

class FXModel {
public:	

	
	bool inited;
	int pointId;
	int pointTotal;
	//const float *pts;
	std::array<float, 66 * 2> pts;
	float viewerSize;
	float faceWidth;
	float faceWidthRaw;
	float xFaceCenter;
	float yFaceCenter;
	float xTopLipCenter;
	float yTopLipCenter;
	int pitch;
	int yaw;
	int roll;
	float xFaceCenterRaw;
	float yFaceCenterRaw;
	float pupilLX;
	float pupilLY;
	float pupilRX;
	float pupilRY;
	float gazeLX;
	float gazeLY;
	float gazeRX;
	float gazeRY;
	float gazeLZ;
	float gazeRZ;
	int fWidth;
	int fHeight;
	int width;
	int height;
	float widthDiff;
	//const float *pts3d;
	std::array<float, 66 * 2> pts3d;

	std::array<float, 66> confidence;

	cv::Rect faceRect;

	float xTopLipCenterFixed;
	float yTopLipCenterFixed;

	float xBottomLipCenter;
	float yBottomLipCenter;

public: 
	FXModel();
	FXModel(const FXModel& t);
	FXModel& operator=(const FXModel& t);

	static std::vector<FXModel> CreateFxModelArrFromPoints(int frameCols, int frameRows, const float* rawPoints[FaceTracker::MAX_TO_TRACK][2], float trackerValues[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES], const float* confidence[FaceTracker::MAX_TO_TRACK]);

};


#endif