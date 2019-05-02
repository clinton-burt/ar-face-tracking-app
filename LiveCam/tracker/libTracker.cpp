#include "libTracker.h";
int ULS_TrackerInit(std::string pModelPath, std::string kActivationKey){

	return 1;
}//return value 1:Success (<0):false
void ULS_TrackerFree(){

}
int ULS_TrackerProcess(cv::Mat inputImg){
	return 1;
}
//return value (<0):error (-50):No face (>0):Success
int ULS_AddFaces(const int n, const int top[], const int left[], const int width[], const int height[]){
	return 1;
}
int ULS_GetTrackerPointNum(){
	return 1;
}
//Num of tracked points
const float * ULS_GetTrackerPoint(int faceNbr){
	const float *value;
	return value;
}
//tracked 2D points in image plane
const float * ULS_GetTrackerPoint3D(int faceNbr){
	const float *value;
	return value;
}
//tracked 3D points in camera coordinate 
const float * ULS_GetConfidence(int faceNbr){
	const float *value;
	return value;
}
// confidence of each tracked 2D point
float ULS_GetPitchRadians(int faceNbr){
	return 1;
}
//pitch
float ULS_GetYawRadians(int faceNbr){
	return 1;
}
//yaw
float ULS_GetRollRadians(int faceNbr){
	return 1;
}
//roll 
float ULS_GetScaleInImage(int faceNbr){
	return 1;
}
//Scale
int ULS_GetSmoothFactor(){
	return 1;
}
void ULS_SetStablePose(bool bStable, int smooth_factor){

}
bool ULS_GetStateStablePose(){
	return true;
}
float ULS_GetStablePitchRadians(int faceNbr){
	return 1;
}
float ULS_GetStableYawRadians(int faceNbr){
	return 1;
}
float ULS_GetStableRollRadians(int faceNbr){
	return 1;
}
float ULS_GetStableScaleInImage(int faceNbr){
	return 1;
}
void ULS_GetLeftPupilXY(float &x, float &y, int faceNbr){

}
//left pupil position
void ULS_GetRightPupilXY(float &x, float &y, int faceNbr){
}
//right pupil position
void ULS_GetLeftGaze(float &x, float &y, float &z, int faceNbr){

}
//left eye gaze
void ULS_GetRightGaze(float &x, float &y, float &z, int faceNbr){

}
//right eye gaze
void ULS_ResetTracker(){

}
//reset tracker
int  ULS_GetAliveInfoAt(int faceNbr){
	return 1;
}
void ULS_SetSmooth(bool bSmooth){

}
// smooth position of landmarks
char *ULS_GetSDKInfo(){
    char *c = new char;
    return c;
}
// version information
// ++ 1.081, add new functions 
// ------------------------------
void ULS_GetFaceCenter(float &x, float &y, int faceNbr){

}
// (x,y) is face center of tracked face
int ULS_GetHead3DPosition(float &x, float &y, float &z, int faceNbr){
	return 1;
}	 // get the 3D position of head center in world coordinate
int ULS_GetHead3DPositionCC(float &x, float &y, float &z, int faceNbr){
	return 1;
}
// get the 3D position of head center in camera (model) coordinate
// ++ 1.082
// ------------------------------
// To find the 3D translation between camera coordinate system and model coordinate system
// 3D translation is always changing every frame
void ULSee_GetTranslation(float &x, float &y, float &z,
	const float focal_length,     // K = [f, 0, cx; ... 
	const float image_centre_x,   //      0, f, cy; ...
	const float image_centre_y, int faceNbr){

}
			  //      0, 0, 1]
const float *ULS_Get2DFrom3DPoints(int faceNbr){
	const float *value;
	return value;
}
// camera coordinate -> world coordinate -> image plane
// world -> image
// if have one point in world coordinate, use intrsinic camera parameters to transfer it to 2D image plane
void ULS_GetOne2DFromOne3DPoint(float _3dx, float _3dy, float _3dz, float &_2dx, float &_2dy, int faceNbr){

}
// camera -> world -> image
// if have one point in camera coordinate system, use intrsinic camera parameters and extrinsic param to transfer it to 2D image plane 
void ULS_GetOne2DFromOne3DC(float _3dCx, float _3dCy, float _3dCz, float &_2dx, float &_2dy, int faceNbr){

}
// camera -> world coordinate
void ULS_GetOne3DPointCtoW(float _3dx, float _3dy, float _3dz, float &_3dWx, float &_3dWy, float &_3dWz, int faceNbr){

}
// ------------------------------
bool ULS_CheckKeyEffect(std::string kActivationKey){
	return true;
}
