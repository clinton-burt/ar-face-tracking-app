#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <models\DrawObjectsModel.h>
#include <models\OpenGLMesh.h>
#include <models\FXModel.h>

#include <tinyobjloader\tiny_obj_loader.h>
#include <utils\ModelLoader.h>
#include <utils\ModelUtil.h>

#include <tracker\FaceTracker.h>
#include <fx\FxCore.h>
#include <fx\FxDebugFalsePositive.h>

#include "Resolutions.h"

#include "FrameManager.h"

extern double cameraDefaultVerticalAngle;
extern double OpenGLzNear;
extern double OpenGLzFar;

extern FrameManager frameManager;

enum class AntiAliasing
{
	NONE,
	SSAA_4X
};

extern const std::map<std::string, AntiAliasing> AntiAliasingMap;

class OpenGLMain
{
public:
	int frame_width;
	int frame_height;
	GLFWwindow* window;
	OpenGLMesh mesh;

	Matrix4f textureProjection;
	Matrix4f particlesProjection;
	DrawObjectsModel textureQuad;
	std::shared_ptr<cwc::glShader> textureShader;
	std::shared_ptr<cwc::glShader> alphaMaskShader;
	std::shared_ptr<cwc::glShader> antiAliasingShader;

	FxCore *currentEffect;
	bool isTracking;

	size_t cameraFrameIndex;
	
	void drawForeground(std::vector<cv::Rect>& opencvFaces, cv::Mat frame, std::vector<FXModel>& faces);

	cv::Mat drawBackground(std::vector<cv::Rect>& opencvFaces, cv::Mat frame, std::vector<FXModel>& faces);
	
	cv::Mat addTrackerValues(std::vector<cv::Rect>& opencvFaces, cv::Mat frame, std::vector<FXModel>& faces);

	void updateAllTextures(FilterModule *module);
	void updateTexture(FilterModule *module, int faceIndex);

	bool loadEffect(FxCore *effect);
			
	void close();

	int init(AntiAliasing antiAliasing);

	void initAntiAliasing4XShader(); // 4X sample pattern
	
	void RecreateSimpleTexture(int width, int height);

	void PostProcessing();

	void DrawTexture(GLuint texId, std::shared_ptr<cwc::glShader> shader);

	typedef std::pair<GLuint, std::string> TextureUniform;
	void DrawTextures(std::vector<TextureUniform> textures, std::shared_ptr<cwc::glShader> shader);
};