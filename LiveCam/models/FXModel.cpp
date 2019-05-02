#include "FXModel.h"

#include "utils/Resolutions.h"

	FXModel::FXModel()
	{
		inited = false;
		pointId = 0;
		pointTotal = 0;
		pts.fill(0);
		viewerSize = 0;
		faceWidth = 0;
		faceWidthRaw = 0;
		xFaceCenter = 0;
		yFaceCenter = 0;
		xTopLipCenter = 0;
		yTopLipCenter = 0;
		pitch = 0;
		yaw = 0;
		roll = 0;
		xFaceCenterRaw = 0;
		yFaceCenterRaw = 0;
		pupilLX = 0;
		pupilLY = 0;
		pupilRX = 0;
		pupilRY = 0;
		gazeLX = 0;
		gazeLY = 0;
		gazeRX = 0;
		gazeRY = 0;
		gazeRZ = 0;
		gazeLZ = 0;
		fWidth = 0;
		fHeight = 0;
		width = 0;
		height = 0;
		widthDiff = 0;
		pts3d.fill(0);
		confidence.fill(0);
		faceRect = { 0,0,0,0 };
		xTopLipCenterFixed = 0;
		yTopLipCenterFixed = 0;
		xBottomLipCenter = 0;
		yBottomLipCenter = 0;
	}

	FXModel::FXModel(const FXModel& t)
	{
		inited = t.inited;
		pointId = t.pointId;
		pointTotal = t.pointTotal;
		pts = t.pts;
		viewerSize = t.viewerSize;
		faceWidth = t.faceWidth;
		faceWidthRaw = t.faceWidthRaw;
		xFaceCenter = t.xFaceCenter;
		yFaceCenter = t.yFaceCenter;
		xTopLipCenter = t.xTopLipCenter;
		yTopLipCenter = t.yTopLipCenter;
		pitch = t.pitch;
		yaw = t.yaw;
		roll = t.roll;
		xFaceCenterRaw = t.xFaceCenterRaw;
		yFaceCenterRaw = t.yFaceCenterRaw;
		pupilLX = t.pupilLX;
		pupilLY = t.pupilLY;
		pupilRX = t.pupilRX;
		pupilRY = t.pupilRY;
		gazeLX = t.gazeLX;
		gazeLY = t.gazeLY;
		gazeRX = t.gazeRX;
		gazeRY = t.gazeRY;
		gazeRZ = t.gazeRZ;
		gazeLZ = t.gazeLZ;
		fWidth = t.fWidth;
		fHeight = t.fHeight;
		width = t.width;
		height = t.height;
		widthDiff = t.widthDiff;
		pts3d = t.pts3d;
		confidence = t.confidence;
		faceRect = t.faceRect;
		xTopLipCenterFixed = t.xTopLipCenterFixed;
		yTopLipCenterFixed = t.yTopLipCenterFixed;

		xBottomLipCenter = t.xBottomLipCenter;
		yBottomLipCenter = t.yBottomLipCenter;
	}
	
	FXModel& FXModel::operator=(const FXModel& t)
	{
		inited = t.inited;
		pointId = t.pointId;
		pointTotal = t.pointTotal;
		pts = t.pts;
		viewerSize = t.viewerSize;
		faceWidth = t.faceWidth;
		faceWidthRaw = t.faceWidthRaw;
		xFaceCenter = t.xFaceCenter;
		yFaceCenter = t.yFaceCenter;
		xTopLipCenter = t.xTopLipCenter;
		yTopLipCenter = t.yTopLipCenter;
		pitch = t.pitch;
		yaw = t.yaw;
		roll = t.roll;
		xFaceCenterRaw = t.xFaceCenterRaw;
		yFaceCenterRaw = t.yFaceCenterRaw;
		pupilLX = t.pupilLX;
		pupilLY = t.pupilLY;
		pupilRX = t.pupilRX;
		pupilRY = t.pupilRY;
		gazeLX = t.gazeLX;
		gazeLY = t.gazeLY;
		gazeRX = t.gazeRX;
		gazeRY = t.gazeRY;
		gazeRZ = t.gazeRZ;
		gazeLZ = t.gazeLZ;
		fWidth = t.fWidth;
		fHeight = t.fHeight;
		width = t.width;
		height = t.height;
		widthDiff = t.widthDiff;
		pts3d = t.pts3d;
		confidence = t.confidence;
		faceRect = t.faceRect;
		xTopLipCenterFixed = t.xTopLipCenterFixed;
		yTopLipCenterFixed = t.yTopLipCenterFixed;

		xBottomLipCenter = t.xBottomLipCenter;
		yBottomLipCenter = t.yBottomLipCenter;
		return *this;
	}

	std::vector<FXModel> FXModel::CreateFxModelArrFromPoints(int frameCols, int frameRows, const float* rawPoints[FaceTracker::MAX_TO_TRACK][2], float trackerValues[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES], const float* confidence[FaceTracker::MAX_TO_TRACK])
	{
		std::vector<FXModel> fxModelArr;

		for (int id = 0; id < FaceTracker::MAX_TO_TRACK; id++)
		{

			if (trackerValues[id][FaceTracker::POINT_ID] >= 0)
			{
				float imgWidth = frameCols;
				float imgHeight = frameRows;
				float widthDiff = 1.0f;
				float heightDiff = 1.0f;

				if (imgWidth != Resolutions::OUTPUT_WIDTH) {
					widthDiff = Resolutions::OUTPUT_WIDTH / imgWidth;
				}

				if (imgHeight != Resolutions::OUTPUT_HEIGHT) {
					heightDiff = Resolutions::OUTPUT_HEIGHT / imgHeight;
				}

				// calculate x and y face center values.
				float xValue = (float)-(1 - ((trackerValues[id][FaceTracker::FACE_CENTER_X]) / (imgWidth / 2)));
				float yValue = (float)(1 - ((trackerValues[id][FaceTracker::FACE_CENTER_Y]) / (imgHeight / 2)));

				// calculate x and y face center values.
				float lipsXValue = (float)-(1 - ((trackerValues[id][FaceTracker::TOP_LIP_CENTER_X]) / (imgWidth / 2)));
				float lipsYValue = (float)(1 - ((trackerValues[id][FaceTracker::TOP_LIP_CENTER_Y]) / (imgHeight / 2)));

				float facewidth = trackerValues[id][FaceTracker::FACE_WIDTH] / imgWidth;

				float viewerSize = (facewidth * 2) * widthDiff;

				int pitch = (int)trackerValues[id][FaceTracker::PITCH];
				int yaw = 0 - (int)trackerValues[id][FaceTracker::YAW];
				int roll = 0 - (int)trackerValues[id][FaceTracker::ROLL];

				FXModel fxModel = FXModel();
				fxModel.pointId = id;
				fxModel.pointTotal = trackerValues[id][FaceTracker::POINT_TOTAL];
				std::copy(&rawPoints[id][FaceTracker::RAW_POINTS][0], &rawPoints[id][FaceTracker::RAW_POINTS][66 * 2], &fxModel.pts[0]);
				std::copy(&rawPoints[id][FaceTracker::RAW_POINTS][0], &rawPoints[id][FaceTracker::RAW_POINTS][66 * 3], &fxModel.pts3d[0]);
				//fxModel.pts = rawPoints[id][FaceTracker::RAW_POINTS];
				//fxModel.pts3d = rawPoints[id][FaceTracker::RAW_3D_POINTS];
				fxModel.faceWidth = facewidth;
				fxModel.widthDiff = widthDiff;
				fxModel.faceWidthRaw = trackerValues[id][FaceTracker::FACE_WIDTH];
				fxModel.xFaceCenter = xValue;
				fxModel.yFaceCenter = yValue;
				fxModel.xTopLipCenter = trackerValues[id][FaceTracker::TOP_LIP_CENTER_X];
				fxModel.yTopLipCenter = trackerValues[id][FaceTracker::TOP_LIP_CENTER_Y];

				fxModel.xBottomLipCenter = trackerValues[id][FaceTracker::LIP_BOTTOM_CENTER_X];
				fxModel.yBottomLipCenter = trackerValues[id][FaceTracker::LIP_BOTTOM_CENTER_Y];

				fxModel.xTopLipCenterFixed = lipsXValue;
				fxModel.yTopLipCenterFixed = lipsYValue;

				fxModel.xFaceCenterRaw = trackerValues[id][FaceTracker::FACE_CENTER_X];
				fxModel.yFaceCenterRaw = trackerValues[id][FaceTracker::FACE_CENTER_Y];
				fxModel.pitch = pitch;
				fxModel.roll = roll;
				fxModel.yaw = yaw;
				fxModel.viewerSize = viewerSize;
				fxModel.gazeLX = trackerValues[id][FaceTracker::LEFT_GAZE_X];
				fxModel.gazeLY = trackerValues[id][FaceTracker::LEFT_GAZE_Y];
				fxModel.gazeRX = trackerValues[id][FaceTracker::RIGHT_GAZE_X];
				fxModel.gazeRY = trackerValues[id][FaceTracker::RIGHT_GAZE_Y];
				fxModel.gazeRZ = trackerValues[id][FaceTracker::RIGHT_GAZE_Z];
				fxModel.gazeLZ = trackerValues[id][FaceTracker::LEFT_GAZE_Z];
				fxModel.pupilLX = trackerValues[id][FaceTracker::LEFT_PUPIL_X];
				fxModel.pupilLY = trackerValues[id][FaceTracker::LEFT_PUPIL_Y];
				fxModel.pupilRX = trackerValues[id][FaceTracker::RIGHT_PUPIL_X];
				fxModel.pupilRY = trackerValues[id][FaceTracker::RIGHT_PUPIL_Y];
				fxModel.fWidth = frameCols;
				fxModel.fHeight = frameRows;
				fxModel.width = Resolutions::OUTPUT_WIDTH;
				fxModel.height = Resolutions::OUTPUT_HEIGHT;
				fxModel.faceRect = cv::Rect(trackerValues[id][FaceTracker::FACE_RECT_X], trackerValues[id][FaceTracker::FACE_RECT_Y], trackerValues[id][FaceTracker::FACE_RECT_WIDTH], trackerValues[id][FaceTracker::FACE_RECT_HEIGHT]);
				/*
				for (size_t i = 0; i < 66; i++)
				{
					fxModel.confidence[i] = confidence[id][i];
				}*/

				fxModel.inited = true;
				
				fxModelArr.push_back(fxModel);
			}


		}

		return fxModelArr;
	}