#ifndef VARINTERFACE_HPP
#define VARINTERFACE_HPP

#include <opencv2\opencv.hpp>
#include <qpoint.h>

#include "FxCore.h"

const int LEFT_VIEWER = 0;
const int RIGHT_VIEWER = 1;


const int TONGUE_CLOSED = 0;
const int TONGUE_OPEN = 1;
const int TONGUE_CLOSING = 2;

bool showComposite(int value, bool visible, std::shared_ptr<FxCore> filter);

void sendEffectToOpenGL(std::shared_ptr<FxCore> filter);

bool isTaking();

bool outputFilterToWindow();

void closeCameras();

void runTracker();

bool usingDragDrop(bool value);

bool dropImage(FilterModule *module);

bool doubleClickSelection(FilterModule *module);

bool draggingOver(QPoint qPoint);

//bool draggingOver(QPoint qPoint);

void stopTimer();

void loadFilter(int value);

//cv::Mat drawComposites(cv::Mat frame);

int startVideoFile(const std::string& fileName);
int startCamera(int cameraNumber);

void setWatermark(std::string watermarkString);

#endif // VARINTERFACE_HPP