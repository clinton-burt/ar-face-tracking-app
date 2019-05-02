#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <math.h>

#include "../opencv/build/include/opencv2/objdetect/objdetect.hpp"
#include "../opencv/build/include/opencv2/imgproc/imgproc.hpp"
#include "../opencv/build/include/opencv2/highgui/highgui.hpp"  // Video write

#include "utils/stb_image.h"
#include "utils/sceneSwitchDetector.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream> // stringstream
#include <iomanip> // put_time

#include "ui/CVImageWidget.h"
#include "ui/CVImageWidgetOrg.h"
#include "ui/LiveCamWindow.h"

#include "FxCore.h"
#include <tracker\FaceTracker.h>
#include <models\CameraModel.h>

#include <QStyleFactory>
#include <QtWidgets\QApplication>
#include <OpenGLMain.h>

#include "FrameManager.h"
#include "ResourceManager.h"

#include "utils/Resolutions.h"

#ifdef SOURCE_VIDEO

cv::VideoCapture inputVideo("C:/Users/mephi/Videos/Potentially Treason.avi");

#endif
#ifdef OUTPUT_VIDEO

cv::VideoWriter outputVideo;
#endif

std::string lastVideoFileName;
int videoFrameCounter = 0;
cv::VideoCapture inputVideo;

//std::string face_cascade_name, eyes_cascade_name;
//cv::CascadeClassifier face_cascade;
//cv::CascadeClassifier eyes_cascade;

const static int CAMERA_CLOSED = -1;
int leftViewerFeedIndex;

bool leftIsShowingComposite = false;

bool isRunningCamera = false;
bool isTracking;
bool isUsingDragAndDrop = false;
bool dropped = false;
bool isShowingBoundingBox = false;
int currentChosenFace = 0;
QString updatedImageDrop;

CameraModel cameraOject0;
int selectedCameraIndex = CAMERA_CLOSED;
cv::VideoCapture camera0;

CVImageWidget* rawImageWidget;
CVImageWidget* compositeImageWidget;
CVImageWidgetOrg* outputImageWidget;
FaceTracker faceTracker;
OpenGLMain openGlMain;

FrameManager frameManager;

ShaderManagerWrapper shaderManagerWrapper;

ResourceManager resourceManager;

cv::Mat teamLogo;
cv::Mat filterWatermarkImage;
std::string teamBrandingPath = "";

double bmFps = 0.0;

//Temp workaround to destroy shader manager in time
class ShaderManagerDestroyer
{
public:
	~ShaderManagerDestroyer()
	{
		shaderManagerWrapper.Clear();
	}
};

cv::Mat imageBoundingBox;

//const int amountOfImagesToUse = 31;
const int amountOfImagesToUse = 7;
cv::Mat imageSequence[amountOfImagesToUse];
int imageSequenceCounter;
bool imageSequenceUp;

QPoint mousePoint;
bool facesForDropping[FaceTracker::MAX_TO_TRACK];
float scale = 1.0;

LiveCamWindow *liveCamWindow;

GLuint cubemapTexId;

cv::Mat testImage;

#ifdef SCENE_SWITCH_DETECTOR
SceneSwitchDetector sceneSwitchDetector;
#endif

void drawPoint(const float xc, const float yc, const float rad,
	cv::Mat &I, const cv::Scalar &color, const bool full) {
	int x0 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(floor(xc - rad))));
	int x1 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(ceil(xc + rad))));
	int y0 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(floor(yc - rad))));
	int y1 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(ceil(yc + rad))));
	cv::Vec3b colorB(color[0], color[1], color[2]);
	if (!full) {
		for (int y = y0; y <= y1; y++) {
			for (int x = x0; x <= x1; x++) {
				float vx = x - xc, vy = y - yc;
				float v = fabsf(sqrtf(vx * vx + vy * vy) - rad);
				if (v <= 1.0f) {
					float w = 1.0f - v;
					I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
				}
			}
		}
	}
	else {
		for (int y = y0; y <= y1; y++) {
			for (int x = x0; x <= x1; x++) {
				float vx = x - xc, vy = y - yc;
				float v = sqrtf(vx * vx + vy * vy) - rad;
				if (v <= 0) {
					I.at<cv::Vec3b>(y, x) = colorB;
				}
				else if (v <= 1.0f) {
					float w = 1.0f - v;
					I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
				}
			}
		}
	}
}

void drawPointAA(const float xc, const float yc, const float rad,
	cv::Mat &I, const cv::Scalar &color, const bool full) {
	int x0 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(floor(xc - rad))));
	int x1 = cv::min(I.cols - 1, cv::max(0, static_cast<int>(ceil(xc + rad))));
	int y0 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(floor(yc - rad))));
	int y1 = cv::min(I.rows - 1, cv::max(0, static_cast<int>(ceil(yc + rad))));
	cv::Vec3b colorB(color[0], color[1], color[2]);
	if (!full) {
		for (int y = y0; y <= y1; y++) {
			for (int x = x0; x <= x1; x++) {
				float vx = x - xc, vy = y - yc;
				float v = fabsf(sqrtf(vx * vx + vy * vy) - rad);
				if (v <= 1.0f) {
					float w = 1.0f - v;
					I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
				}
			}
		}
	}
	else {
		for (int y = y0; y <= y1; y++) {
			for (int x = x0; x <= x1; x++) {
				float vx = x - xc, vy = y - yc;
				float v = sqrtf(vx * vx + vy * vy) - rad;
				if (v <= 0) {
					I.at<cv::Vec3b>(y, x) = colorB;
				}
				else if (v <= 1.0f) {
					float w = 1.0f - v;
					I.at<cv::Vec3b>(y, x) = v * I.at<cv::Vec3b>(y, x) + w * colorB;
				}
			}
		}
	}
}

bool LoadCubemapTexture(const std::array<std::string, 6>& texture_filename, GLuint& id);

void sendEffectToOpenGL(std::shared_ptr<FxCore> filter)
{
	openGlMain.loadEffect(filter.get());
}

void loadFilter(int value)
{
	liveCamWindow->filterManager.setCurrentFilterWithID(value);
	if (leftIsShowingComposite){
		sendEffectToOpenGL(liveCamWindow->filterManager.getCurrentFilter());
	}
}

bool usingDragDrop(bool value){
	isUsingDragAndDrop = value;
	return value;
}

bool showComposite(int value, bool visible, std::shared_ptr<FxCore> filter) {
	/*if (leftViewerFeedIndex == CAMERA_CLOSED){
		return false;
	}*/
	leftIsShowingComposite = visible;
	if (leftIsShowingComposite){
		sendEffectToOpenGL(filter);
	}	
 	return leftIsShowingComposite;
}

void openOutputWindow(){
}

void closeCameras(){
	if (camera0.isOpened()){
		camera0.release();
	}
	openGlMain.close();
	//liveCamWindow->exitCameraThread();
	cv::destroyAllWindows();
	isRunningCamera = false;
	qApp->quit();
}

bool draggingOver(QPoint qPoint){
	mousePoint = qPoint;
	return true;
}

bool doubleClickSelection(FilterModule *module)
{
//	FxCore* currentEffect = liveCamWindow->filterManager.getCurrentFilter().get();

// 	if (currentEffect->isSwitchingTextures){
// 		openGlMain.updateAllTextures(absolutePath);
// 	}
// 	else{
// 		FXModel fxModel2Send;
// 		currentEffect->selection(absolutePath, index, fxModel2Send);
// 	}

	openGlMain.updateAllTextures(module);

	return true;
}

bool dropImage(FilterModule *module)
{
	FxCore* currentEffect = liveCamWindow->filterManager.getCurrentFilter().get();

	if (facesForDropping[currentChosenFace])
	{
// 		if (currentEffect->isSwitchingTexture())
// 		{
// 			openGlMain.updateTexture(absolutePath, currentChosenFace);
// 		}
// 		else
// 		{
// 			FXModel fxModel2Send;
// 			fxModel2Send.pointId = currentChosenFace;
// 			currentEffect->switch2dImage(absolutePath, index, fxModel2Send);
// 		}

		openGlMain.updateTexture(module, currentChosenFace);

		return true;
	}
	return false;
}

cv::Mat testForDrop(cv::Mat frame, const std::vector<FXModel>& fxModelArr)
{
	for (int id = 0; id < fxModelArr.size(); id++){

		
		float mouseX = ((float)mousePoint.x() / (float)compositeImageWidget->width()) * Resolutions::INPUT_ACTUAL_WIDTH;
		float mouseY = ((float)mousePoint.y() / (float)compositeImageWidget->height()) * Resolutions::INPUT_ACTUAL_HEIGHT;
		

		int faceCenterX = (fxModelArr[id].xFaceCenterRaw);
		int faceCenterY = (fxModelArr[id].yFaceCenterRaw);
		float faceWidth = fxModelArr[id].faceWidthRaw;

		/*int faceCenterX = (trackerValues[id][FaceTracker::FACE_CENTER_X]);
		int faceCenterY = (trackerValues[id][FaceTracker::FACE_CENTER_Y]);
		float faceWidth = trackerValues[id][FaceTracker::FACE_WIDTH];		
		*/
		if ((int)mouseX > (int)(faceCenterX - (faceWidth / 2)) && (int)mouseX < (int)(faceCenterX + (faceWidth / 2))){
			if ((int)mouseY >(int)(faceCenterY - (faceWidth / 2)) && (int)mouseY < (int)(faceCenterY + (faceWidth / 2))){
				
				//int roll = 0 - (int)trackerValues[id][FaceTracker::ROLL];
				int roll = fxModelArr[id].roll;

				cv::Mat boundingBoxInside = imageBoundingBox;
				float size = faceWidth * ((float)openGlMain.frame_width / (float)Resolutions::INPUT_ACTUAL_WIDTH);
				boundingBoxInside = CVImageUtil::rotateAndSize(boundingBoxInside, roll, size);
				CVImageUtil::overlayImage(frame, boundingBoxInside,
					cv::Point((faceCenterX * ((float)openGlMain.frame_width / (float)Resolutions::INPUT_ACTUAL_WIDTH)) - (size / 2),
					(faceCenterY * ((float)openGlMain.frame_height / (float)Resolutions::INPUT_ACTUAL_HEIGHT)) - (size / 2)));
				facesForDropping[id] = true;
				currentChosenFace = id;
			}
		}
		else
		{
			facesForDropping[id] = false;
		}
	}
	return frame;
}

void runTracker(){
	if (isRunningCamera)
	{
		cv::Mat frame0, image0, output0;

		cv::Mat frame_gray, frameSmall;

		if (selectedCameraIndex != CAMERA_CLOSED)
		{
			camera0 >> image0;
		}
		else
		{
			if (inputVideo.isOpened())
			{
				inputVideo >> image0;

				videoFrameCounter++;

				if (videoFrameCounter >= inputVideo.get(CV_CAP_PROP_FRAME_COUNT))
				{
					videoFrameCounter = 0;
					inputVideo.set(CV_CAP_PROP_POS_FRAMES, 0);
				}
			}
		}

#ifdef OUTPUT_VIDEO
		if (image0.empty())
		{
			if (outputVideo.isOpened())
			{
				outputVideo = cv::VideoWriter();
			}
	}
#endif
		
		static cv::Mat image;
		
		frame0 = image0;

#if defined(SCENE_SWITCH_DETECTOR)

		if (!image0.empty())
		{
			cv::resize(frame0, frameSmall, cv::Size(frame0.cols / 8, frame0.rows / 8));

			if (sceneSwitchDetector.sceneSwitched(frameSmall))
			{
				faceTracker.resetHard();
				liveCamWindow->filterManager.getCurrentFilter()->resetStable();
			}
}
#endif

		rawImageWidget->showImage(image0);

		if (leftIsShowingComposite)
		{
			cv::Mat outputImage;
			// create float array for storing tracker values
			std::vector<cv::Rect> faces;
			std::vector<FXModel> fxModelArr;

			if (!image0.empty())
			{

#ifndef ASYNC_TRACKER
				//Sync
				fxModelArr = faceTracker.trackerValuesFromMatImmediate(frame0);

#else
				//async
				faceTracker.trackerValuesAsync(frame0);

				auto modelArrAndFrame = faceTracker.getLastTrackedValuesAndFrame();
				fxModelArr = modelArrAndFrame.first;
				if (!modelArrAndFrame.second.empty())
				{
					modelArrAndFrame.second.copyTo(frame0);
					modelArrAndFrame.second.copyTo(image0);
					//frame0 = modelArrAndFrame.second;
					//image0 = modelArrAndFrame.second;
			}
#endif

#ifdef ADDITIONAL_FALSE_FILTER
				faces = faceTracker.getLastTrackedOpencvFaces();

#endif
		}

			if (liveCamWindow->isVideoDisplaying && !liveCamWindow->isFilterDisplaying)
			{
				cv::Size size(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);//the dst image size,e.g.100x100
				cv::Mat dst;//dst image
				resize(image0, dst, size);//resize image
				if (teamBrandingPath != "" || !filterWatermarkImage.empty()) {
					if (!filterWatermarkImage.empty())
					{
						CVImageUtil::overlayImage2(dst, filterWatermarkImage, Resolutions::OUTPUT_WIDTH - filterWatermarkImage.cols, Resolutions::OUTPUT_HEIGHT - filterWatermarkImage.rows);
					}
					else if (!teamLogo.empty())
					{
						CVImageUtil::overlayImage2(dst, teamLogo, Resolutions::OUTPUT_WIDTH - teamLogo.cols, Resolutions::OUTPUT_HEIGHT - teamLogo.rows);
					}
				}

				outputImageWidget->showImage(dst);
				
			}
			// add the tracker values array to the opengl class and return the output image
			outputImage = openGlMain.addTrackerValues(faces, image0, fxModelArr);

			frame0 = outputImage;
			if (teamBrandingPath != "" || !filterWatermarkImage.empty()) {
				if (!filterWatermarkImage.empty())
				{
					CVImageUtil::overlayImage2(frame0, filterWatermarkImage, Resolutions::OUTPUT_WIDTH - filterWatermarkImage.cols, Resolutions::OUTPUT_HEIGHT - filterWatermarkImage.rows);
				}
				else if (!teamLogo.empty())
				{
					CVImageUtil::overlayImage2(frame0, teamLogo, Resolutions::OUTPUT_WIDTH - teamLogo.cols, Resolutions::OUTPUT_HEIGHT - teamLogo.rows);
				}
			}

			if (liveCamWindow->isVideoDisplaying && liveCamWindow->isFilterDisplaying)
			{
				outputImageWidget->showImage(frame0);
			}

			if (leftIsShowingComposite)
			{
				if (isUsingDragAndDrop)
				{
					testForDrop(frame0, fxModelArr);
				}

				compositeImageWidget->showImage(frame0);
				}

#ifdef OUTPUT_VIDEO
			outputVideo << frame0;
#endif

			}
		else if (liveCamWindow->isVideoDisplaying)
		{
			cv::Size size(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);//the dst image size,e.g.100x100
			cv::Mat dst;//dst image
			resize(image0, dst, size);//resize image
			outputImageWidget->showImage(dst);
		}
	}
}

int startVideoFile(const std::string& fileName)
{
	selectedCameraIndex = CAMERA_CLOSED;
	if (camera0.isOpened()) {
		camera0.release();
	}

	inputVideo = cv::VideoCapture("./videos/" + fileName);


	if (!inputVideo.isOpened())
	{
		return -1;
	}

	isRunningCamera = true;

	lastVideoFileName = fileName;

	videoFrameCounter = 0;

	Resolutions::INPUT_ACTUAL_WIDTH = inputVideo.get(CV_CAP_PROP_FRAME_WIDTH);
	Resolutions::INPUT_ACTUAL_HEIGHT = inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT);

	for (auto filter : liveCamWindow->filterManager.filters)
	{
		filter.second->onInputFrameResized();
	}

	liveCamWindow->editorWindow->getEditableFilter()->onInputFrameResized();

	return 0;
}

int startCamera(int cameraNumber) {

	/*		
	if (cameraOject0.getIndex() == CAMERA_CLOSED){
			camera0.open(cameraNumber);
			if (!camera0.isOpened()) return -1;
			cameraOject0.setIndex(cameraNumber);
		}
		else if (cameraOject0.getIndex() != cameraNumber){
			cameraOject0.setIndex(CAMERA_CLOSED);
			if (camera0.isOpened()){
				camera0.release();
			}
			camera0.open(cameraNumber);
			if (!camera0.isOpened()) return -1;
			cameraOject0.setIndex(cameraNumber);
		}

		if (camera0.isOpened()){
			camera0.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
			camera0.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

			cameraWidth = camera0.get(CV_CAP_PROP_FRAME_WIDTH);
			cameraHeight = camera0.get(CV_CAP_PROP_FRAME_HEIGHT);
		}

	
		leftViewerFeedIndex = cameraNumber;
		
	
	isRunningCamera = true;*/


	if (inputVideo.isOpened())
	{
		inputVideo = cv::VideoCapture();
	}


	if (selectedCameraIndex == CAMERA_CLOSED) {
		
		camera0.open(cameraNumber);

		if (!camera0.isOpened())
		{
			return -1;
		}

		selectedCameraIndex = cameraNumber;
	}
	else if (selectedCameraIndex != cameraNumber) {

		selectedCameraIndex = CAMERA_CLOSED;
		if (camera0.isOpened()) {
			camera0.release();
		}
		camera0.open(cameraNumber);
		if (!camera0.isOpened()) {
			return -1;
		}
		selectedCameraIndex = cameraNumber;
	}

	if (camera0.isOpened()) {

		camera0.set(CV_CAP_PROP_FRAME_WIDTH, Resolutions::INPUT_WIDTH);
		camera0.set(CV_CAP_PROP_FRAME_HEIGHT, Resolutions::INPUT_HEIGHT);

		qDebug() << "bmFps:::::: " << bmFps;
		if (bmFps != 0.0) camera0.set(CV_CAP_PROP_FPS, (double)(bmFps));

		Resolutions::INPUT_ACTUAL_WIDTH = camera0.get(CV_CAP_PROP_FRAME_WIDTH);
		Resolutions::INPUT_ACTUAL_HEIGHT = camera0.get(CV_CAP_PROP_FRAME_HEIGHT);
		qDebug() << "CV_CAP_PROP_FPS:::::: " << camera0.get(CV_CAP_PROP_FPS);

		for (auto filter : liveCamWindow->filterManager.filters)
		{
			filter.second->onInputFrameResized();
		}

		liveCamWindow->editorWindow->getEditableFilter()->onInputFrameResized();
	}

	leftViewerFeedIndex = cameraNumber;

	isRunningCamera = true;

	return 0;
}

void ImageBrightnessComputingTest()
{
	for (auto& entry : fs::directory_iterator("./assets/test/"))
	{
		cv::Mat input = cv::imread(entry.path().string());

		cv::Mat1b grayscale(input.rows, input.cols);
		cv::Mat1b brightness(input.rows, input.cols);

		float sum = 0;
		float del = 0;

		for (int row = 0; row < input.rows; row += 1)
		{
			for (int col = 0; col < input.cols; col += 1)
			{
				auto pixel = input.at<cv::Vec3b>(row, col);
				grayscale.at<uchar>(row, col) = (uchar)(pixel[0] * 0.11f + pixel[1] * 0.59f + pixel[2] * 0.3f);
				float x = (pixel[0] * 0.11f + pixel[1] * 0.59f + pixel[2] * 0.3f) / 255.f;
				float y = 3.3f * x * x - 1;
				if (y < 0) y = 0;
				if (y > 2) y = 2;
				sum += x * y;
				del += y;
				brightness.at<uchar>(row, col) = (uchar)(y / 2.3 * 255);
			}
		}

		auto path = (entry.path().parent_path() / entry.path().stem()).string();
		qDebug() << path.c_str();
		cv::imwrite(path + "A.png", grayscale);
		cv::imwrite(path + "B.png", brightness);
		qDebug() << 0.9 + (sum / del - 0.9) * 3;
	}
}

void JSONvalidation()
{
	fs::recursive_directory_iterator it("./assets/");
	fs::recursive_directory_iterator endit;
	std::vector<fs::path> ret;

	while (it != endit)
	{
		if (fs::is_regular_file(*it) && it->path().extension() == ".json")
			ret.push_back(it->path());
		++it;
	}

	qDebug() << "ALL JSON:";

	for (auto p : ret)
	{
		qDebug() << p.string().c_str();
	}

	qDebug() << "WRONG JSON:";

	boost::property_tree::ptree t;
	for (auto p : ret)
	{
		try
		{
			boost::property_tree::json_parser::read_json(p.string(), t);
		}
		catch (...)
		{
			qDebug() << p.string().c_str();
		}
	}
}

void RenameAnimationFrames()
{
	for (int i = 0; i < 91; ++i)
	{
	 	char old[150];
	 	char ne[150];
	 
	 	snprintf(old, sizeof(old), "./FOLDER/TongueAnimation_00%02i.png", i);
	 	snprintf(ne, sizeof(ne), "./FOLDER/%i.png", i);
	 	std::rename(old, ne);
	 
	 	snprintf(old, sizeof(old), "./FOLDER/TongueAnimation_000%02i.png", i);
	 	snprintf(ne, sizeof(ne), "./FOLDER/%i.png", i);
	 	std::rename(old, ne);
	}
}

void BlackToTransparent()
{
	cv::Mat image = cv::imread("./assets/fx/miamiHeat/fire-hairs/A-.png", cv::IMREAD_UNCHANGED);

	for (int row = 0; row < image.rows; ++row)
	{
		for (int col = 0; col < image.cols; ++col)
		{
			auto &pixel = image.at<cv::Vec4b>(row, col);
			pixel[3] = max(pixel[0], max(pixel[1],  pixel[2]));
		}
	}

	cv::imwrite("./assets/fx/miamiHeat/fire-hairs/A.png", image);
}

void setWatermark(std::string watermarkString) {
	if (!watermarkString.empty()) {
		filterWatermarkImage = cv::imread(watermarkString, cv::IMREAD_UNCHANGED);
	}
	else
	{
		filterWatermarkImage.release();
	}
}

int main(int argc, char** argv) {

	// Create the QT application framework
	QApplication app(argc, argv);
	
	/*
	if (!face_cascade.load("./assets/haarcascades/haarcascade_frontalface_alt.xml")) {
		return -1;
	};
	if (!eyes_cascade.load("./assets/haarcascades/haarcascade_eye_tree_eyeglasses.xml")) {
		return -1;
	};*/

	//detectAndDisplay(testImage);

	AntiAliasing antiAliasing = AntiAliasing::NONE;
	std::string antiAliasingString = "NONE";

	try
	{
		boost::property_tree::ptree config;
		boost::property_tree::read_json("./assets/application_config.json", config);

		Resolutions::INPUT_WIDTH = config.get<int>("input video width", Resolutions::INPUT_WIDTH);
		Resolutions::INPUT_HEIGHT = config.get<int>("input video height", Resolutions::INPUT_HEIGHT);

		Resolutions::OUTPUT_WIDTH = config.get<int>("output video width", Resolutions::OUTPUT_WIDTH);
		Resolutions::OUTPUT_HEIGHT = config.get<int>("output video height", Resolutions::OUTPUT_HEIGHT);

		bmFps = config.get<double>("bm_fps", 0.0);

		antiAliasingString = config.get<std::string>("anti-aliasing", "NONE");

		teamBrandingPath = config.get<std::string>("team-branding-image-path", "");
		if (teamBrandingPath!="") {
			teamLogo = cv::imread(teamBrandingPath, cv::IMREAD_UNCHANGED);
		}
	}
	catch (...)
	{
		qDebug() << "Couldn't load app config, default resolution settings";
	}

	try
	{
		antiAliasing = AntiAliasingMap.at(antiAliasingString);
	}
	catch (...)
	{
		qDebug() << "Wrong anti-aliasing setting, none anti-aliasing will be applied.";
	};

	// initialize the opengl window for processing
	openGlMain.init(antiAliasing);

	GLint p = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &p);
	qDebug() << p;
	p = 0;

	std::array<std::string, 6> cubemapTexNames;

	cubemapTexNames[0] = "./assets/fx/cubemap/posx.jpg";
	cubemapTexNames[1] = "./assets/fx/cubemap/negx.jpg";
	cubemapTexNames[2] = "./assets/fx/cubemap/posy.jpg";
	cubemapTexNames[3] = "./assets/fx/cubemap/negy.jpg";
	cubemapTexNames[4] = "./assets/fx/cubemap/posz.jpg";
	cubemapTexNames[5] = "./assets/fx/cubemap/negz.jpg";

	LoadCubemapTexture(cubemapTexNames, cubemapTexId);

	// start face tracker-- opencv based.
	///faceTracker.init("./assets/model/");

	//testImage = cv::imread("C:/Users/mephi/Downloads/LiveCamMinimalSources/LiveCam/assets/images/pimgpsh_fullsize_distr.png", cv::IMREAD_UNCHANGED);

	//faceTracker.init("./model/");

	faceTracker.init("./model/");

	// Create the outputImageWidget widget and set it in the UI
	outputImageWidget = new CVImageWidgetOrg();

	ShaderManagerDestroyer shaderManagerDestroyer; //Must be declared before LiveCamWindow

	LiveCamWindow liveWindow; // Create the Main Window
	liveCamWindow = &liveWindow;
	liveCamWindow->show();

	liveCamWindow->outputWindow->setWidget(outputImageWidget);

	//liveCamWindow->outputWindow->setWindowFlags(Qt::CustomizeWindowHint);

	liveCamWindow->outputWindow->setObjectName("outputWindow");
	liveCamWindow->outputWindow->setStyleSheet("#outputWindow { border: solid black; }");
	liveCamWindow->outputWindow->setStyleSheet("background-color: black;");

	// Create the rawLeftImageWidget widget and set it in the UI
	rawImageWidget = new CVImageWidget();
	liveCamWindow->setRawLeftWidget(rawImageWidget);

	// Create the image widget
	compositeImageWidget = new CVImageWidget();
	liveCamWindow->setCompositeLeftWidget(compositeImageWidget);
	
	// Set camera defaults
	leftViewerFeedIndex = CAMERA_CLOSED;
	cameraOject0.setId(0);

	imageBoundingBox = cv::imread("./assets/images/bounding_box2.png", cv::IMREAD_UNCHANGED);

#ifdef SOURCE_VIDEO

	if (!inputVideo.isOpened())
	{
		cout << "Could not open the input video" << endl;
		return -1;
	}
	int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));

	char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };

	cv::Size S = cv::Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
		(int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

#endif
#ifdef OUTPUT_VIDEO
	
	auto nowTime = std::chrono::system_clock::now();

	auto in_time_t = std::chrono::system_clock::to_time_t(nowTime);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H.%M.%S");

	//outputVideo.open("output"+ ss.str()+".avi", ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);
	outputVideo.open("output" + ss.str() + ".avi", CV_FOURCC('M', 'P', '4', '2'), 16, cv::Size(1920, 1080), true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write" << endl;
		return -1;
	}
	
	#endif

	return app.exec();
}

static bool FileExists(const std::string &abs_filename) {
	bool ret;
	FILE *fp = fopen(abs_filename.c_str(), "rb");
	if (fp) {
		ret = true;
		fclose(fp);
	}
	else {
		ret = false;
	}

	return ret;
}

//Create cubemap texture from 6 images: +X, -X, +Y, -Y, +Z, -Z
bool LoadCubemapTexture(const std::array<std::string, 6>& texture_filename, GLuint& id)
{
	std::array<unsigned char*, 6> imageData;

	int width = 0;
	int height = 0;

	for (size_t i = 0; i < 6; i++)
	{

		if (!FileExists(texture_filename[i])) {
			std::cerr << "Unable to load texture: " << texture_filename[i] << std::endl;
			return false;
		}
		int w, h;
		int comp;
		imageData[i] = stbi_load(texture_filename[i].c_str(), &w, &h, &comp, STBI_default);

		if (i == 0)
		{
			width = w;
			height = h;
		}
		else
		{
			if (w != width || h != height)
			{
				std::cerr << "Texture in cubemap have different size: " << texture_filename[i] << std::endl;
				return false;
			}
		}

		if (!imageData[i]) {
			std::cerr << "Unable to load texture: " << texture_filename[i] << std::endl;
			return false;
		}

		if (comp != 3)
		{
			std::cerr << "Texture should not contain alpha channel: " << texture_filename[i] << std::endl;
			return false;
		}
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

#ifdef _WIN32

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

#endif

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData[5]);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	for (size_t i = 0; i < 6; i++)
	{
		stbi_image_free(imageData[i]);
	}

	return true;
}