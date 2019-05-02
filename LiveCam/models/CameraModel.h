#include <opencv2\opencv.hpp>

class CameraModel {
public:
	void setIndex(int ind);
	int getIndex(void);

	void setId(int i);
	int getId(void);

	void setCamera(cv::VideoCapture cam);
	cv::VideoCapture getCamera(void);

	CameraModel();

private:
	int index = -1;
	int id;
	cv::VideoCapture camera;
};