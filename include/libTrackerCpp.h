#ifndef LIBTRACKER_H
#define LIBTRACKER_H


#include <vector>
#include <iostream>

#ifdef _BUILD_TRACKER_DLL
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif


#ifdef  __cplusplus
extern  "C" {
#endif 

	int EXPORT_API ULS_TrackerInit(std::string pModelPath, std::string kActivationKey, int nbrtracker = 5);//return value 1:Success (<0):false
	void EXPORT_API ULS_TrackerFree();

	int EXPORT_API ULS_TrackerProcess(void *input);//return value (<0):error (-50):No face (>0):Success
	//mode 0: gray
	//1: bgr2gray
	//2: rgb2gray
	//3: bgra2gray
	//4: rgba2gray
	int EXPORT_API ULS_TrackerProcessByte(void *inputdata, int imgw, int imgh, int mode, double face_angle);//return value (<0):error (-50):No face (>0):Success

	int EXPORT_API ULS_AddFaces(const int n, const int top[], const int left[], const int width[], const int height[]);

	int EXPORT_API ULS_GetTrackerPointNum();//Num of tracked points

	const float EXPORT_API * ULS_GetTrackerPoint(int faceNbr);//tracked 2D points in image plane
	const float EXPORT_API * ULS_GetTrackerPoint3D(int faceNbr);//tracked 3D points in camera coordinate 
	const float EXPORT_API * ULS_GetConfidence(int faceNbr);// confidence of each tracked 2D point
	bool EXPORT_API ULS_GetTrackerFacerect(int faceNbr, int &fx, int &fy, int &fw, int &fh);//tracked face rectangle in image plane

	float EXPORT_API ULS_GetPitchRadians(int faceNbr);//pitch
	float EXPORT_API ULS_GetYawRadians(int faceNbr);//yaw
	float EXPORT_API ULS_GetRollRadians(int faceNbr);//roll 
	float EXPORT_API ULS_GetScaleInImage(int faceNbr);//Scale


	int EXPORT_API ULS_GetSmoothFactor();
	void EXPORT_API ULS_SetStablePose(bool bStable, int smooth_factor);
	bool EXPORT_API ULS_GetStateStablePose();
	float EXPORT_API ULS_GetStablePitchRadians(int faceNbr);
	float EXPORT_API ULS_GetStableYawRadians(int faceNbr);
	float EXPORT_API ULS_GetStableRollRadians(int faceNbr);
	float EXPORT_API ULS_GetStableScaleInImage(int faceNbr);


	void EXPORT_API ULS_GetLeftPupilXY(float &x, float &y, int faceNbr);//left pupil position
	void EXPORT_API ULS_GetRightPupilXY(float &x, float &y, int faceNbr);//right pupil position
	void EXPORT_API ULS_GetLeftGaze(float &x, float &y, float &z, int faceNbr);//left eye gaze
	void EXPORT_API ULS_GetRightGaze(float &x, float &y, float &z, int faceNbr);//right eye gaze

	void EXPORT_API ULS_ResetTracker();//reset tracker
	int  EXPORT_API ULS_GetAliveInfoAt(int faceNbr);

	void EXPORT_API ULS_SetSmooth(bool bSmooth);// smooth position of landmarks

	char EXPORT_API *ULS_GetSDKInfo();// version information

	// ++ 1.081, add new functions 
	// ------------------------------
	void EXPORT_API  ULS_GetFaceCenter(float &x, float &y, int faceNbr); // (x,y) is face center of tracked face

	int EXPORT_API  ULS_GetHead3DPosition(float &x, float &y, float &z, int faceNbr);	 // get the 3D position of head center in world coordinate
	int EXPORT_API  ULS_GetHead3DPositionCC(float &x, float &y, float &z, int faceNbr); // get the 3D position of head center in camera (model) coordinate

	// ++ 1.082
	// ------------------------------
	// To find the 3D translation between camera coordinate system and model coordinate system
	// 3D translation is always changing every frame
	void EXPORT_API ULSee_GetTranslation(float &x, float &y, float &z,
		const float focal_length,     // K = [f, 0, cx; ... 
		const float image_centre_x,   //      0, f, cy; ...
		const float image_centre_y, int faceNbr);  //      0, 0, 1]
#if 0
	const float EXPORT_API *ULS_Get2DFrom3DPoints(int faceNbr); // camera coordinate -> world coordinate -> image plane

	// world -> image
	// if have one point in world coordinate, use intrsinic camera parameters to transfer it to 2D image plane
	void EXPORT_API ULS_GetOne2DFromOne3DPoint(float _3dx, float _3dy, float _3dz, float &_2dx, float &_2dy, int faceNbr);

	// camera -> world -> image
	// if have one point in camera coordinate system, use intrsinic camera parameters and extrinsic param to transfer it to 2D image plane 
	void EXPORT_API ULS_GetOne2DFromOne3DC(float _3dCx, float _3dCy, float _3dCz, float &_2dx, float &_2dy, int faceNbr);

	// camera -> world coordinate
	void EXPORT_API ULS_GetOne3DPointCtoW(float _3dx, float _3dy, float _3dz, float &_3dWx, float &_3dWy, float &_3dWz, int faceNbr);
	// ------------------------------
#endif
	bool EXPORT_API ULS_CheckKeyEffect(std::string kActivationKey, std::string &path);


#ifdef  __cplusplus
}
#endif 


#endif