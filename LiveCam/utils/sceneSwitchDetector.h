#ifndef SCENE_SWITCH_DETECTOR_H_INCLUDED
#define SCENE_SWITCH_DETECTOR_H_INCLUDED



#include <iostream> 
#include <opencv2\opencv.hpp>

class SceneSwitchDetector
{
public:

	bool sceneSwitched(cv::Mat newFrame);

protected:
	cv::Mat prevFrame;
	int frameCounter = 0;


};



#endif //SCENE_SWITCH_DETECTOR_H_INCLUDED

