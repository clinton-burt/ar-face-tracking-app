#pragma once

#include <math.h>
#include <chrono>
#include <memory>
#include <array>
#include <vector>
#include <map>
#include <string>

#include <QPixmap>

#include <glsl.h>

#include "boost/filesystem.hpp"

#include <opencv2/core/core.hpp>

#include "Eigen/Geometry"

#include <models\FilterUiModel.h>

#include <CVImageUtil.h>

namespace fs = boost::filesystem;

struct ExternalRenderParams
{
public:
	int frameWidth; // screen width
	int frameHeight; // screen height
	int ULSWidth; // ULS tracker camera width
	int ULSHeight; // ULS tracker camera height
	double vAngle;
	double zNear;
	double zFar;
	float timeShift;
	CVImageUtil::FrameLightParams frameParams;
	CVImageUtil::FaceLightParams faceParams;
};

class ImagePoints
{
public:
	ImagePoints(std::array<float, 66 * 2>& inPts)
		:pts(inPts)
	{

	}
	cv::Point2f at(int i) {
		return cv::Point2f(pts[(i - 1) * 2], pts[(i - 1) * 2 + 1]);
	}
	Eigen::Vector2f getEigenAt(int i) {
		return Eigen::Vector2f(pts[(i - 1) * 2], pts[(i - 1) * 2 + 1]);
	}
	std::array<float, 66 * 2>& pts;
};

class ShaderManagerWrapper
{
public:

	std::shared_ptr<cwc::glShader> LoadFromFile(const char* vertexFile, const char* fragmentFile);

	void Clear();

protected:

	std::map<std::string, std::shared_ptr<cwc::glShader>> cachedShaders;

	std::shared_ptr<cwc::glShaderManager> shaderManager;
};

struct ValueSmoother {

public:

	bool inited = false;

	//double SNAP_MULTIPLIER;

	double SNAP_MULTIPLIER_SIMPLE;

	//static const bool SLEEP_ENABLE;

	//double ACTIVITY_THRESHOLD = 4.0;

	double smoothValue = 0;

	//double errorEMA = 0;

	//long long lastActivityMS = 0;

	//bool sleeping = false;

	double snapCurve(double x);

	double responsiveAnalogReadSimple(double newValue);
	//double responsiveAnalogRead(double newValue);
};

struct ExtraFilterData;

struct ExtraRenderParamsData
{
	size_t shaderIndex;
	ExtraFilterData *root;

	ExtraRenderParamsData(ExtraFilterData *root)
	{
		this->root = root;
	}
};

struct ExtraModelData
{
	std::vector<ExtraRenderParamsData> renderParams;
	ExtraFilterData *root;

	ExtraModelData(ExtraFilterData *root)
	{
		this->root = root;
	}
};

struct ExtraModuleData
{
	QPixmap moduleIcon;
	std::string moduleIconPath;

	std::vector<ExtraModelData> models;

	ExtraFilterData *root;

	ExtraModuleData(ExtraFilterData *root)
	{
		this->root = root;
	}
};

class FxCore;

struct ExtraFilterData
{
	fs::path resourcesRoot;
	fs::path filterFolder;

	std::vector<ExtraModuleData> modules;
	std::vector<ExtraRenderParamsData> renderParams;

	QPixmap filterIcon;
	std::string filterIconPath;

	std::string title;

	std::shared_ptr<FxCore> filter;
};

#include "FxCore.h"