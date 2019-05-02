
#include <models\CameraModel.h>

CameraModel::CameraModel(void) {
}
void CameraModel::setIndex(int ind) {
	index = ind;
}
int CameraModel::getIndex(void) {
	return index;
}
void CameraModel::setId(int i) {
	id = i;
}
int CameraModel::getId(void) {
	return id;
}
void CameraModel::setCamera(cv::VideoCapture cam) {
	camera = cam;
}
cv::VideoCapture CameraModel::getCamera(void) {
	return camera;
}