#ifndef FXCORE_H
#define FXCORE_H

#include <iostream>
#include <vector>
#include <memory>

#include <boost/optional/optional.hpp>
#include "boost/property_tree/ptree.hpp"
#include "boost/filesystem.hpp"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <qdebug.h>
#include <utils\ModelUtil.h>
#include "utils/PointCalcUtil.h"
#include "utils/CVImageUtil.h"
#include "glsl.h"
#include "qicon.h"

#include <models\FilterUiModel.h>
#include <models\FXModel.h>
#include "ResourceManager.h"

#include "CommonClasses.h"

#include "particles/ParticleManager.h"

#include <models\OpenGLModel.h>

namespace fs = boost::filesystem;

extern ResourceManager resourceManager;
extern ShaderManagerWrapper shaderManagerWrapper;

struct FilterModule
{
	FilterModule();
	FilterModule(boost::property_tree::ptree &moduleRecord, size_t ID, const fs::path &resourceRoot);

	boost::property_tree::ptree getPTree(ExtraModuleData &data, std::vector<std::shared_ptr<OpenGLModel>> &models,
		TCommonRenderParams& commonRenderParams, int moduleIndex);

	std::vector<boost::property_tree::ptree> suits;

	size_t moduleID;
	std::string iconPath;
	QIcon icon;

	int externFilterID; // index of a filter in "extern" list in the JSON source, so don't need to know its ID in FilterManager
	int externModuleID; // index of a module in other filter, so need to know its ID in the other hardcoded/templated filter
};

class FxCore
{
protected:
	cwc::glShaderManager* shaderManager = nullptr;

public:
	FxCore();

	virtual ~FxCore();

	template <class T>
	static std::shared_ptr<FxCore> create()
	{
		return std::make_shared<T>();
	}

	bool useFrameLightParams;
	bool useFaceLightParams;
	bool useAlphaMask;

	bool editable;
	fs::path resourcesRoot;

	std::vector<std::shared_ptr<FxCore>> externFilters;
	std::vector<std::shared_ptr<OpenGLModel>> models;
	std::vector<FilterModule> filterModules;

	TCommonRenderParams commonRenderParams;

	std::array<std::string, 6> cubemapTextures;
	GLuint cubemapID = 0; // not unique for models and their objects

	std::map<int, bool> becameStable;

	std::map<size_t, std::vector<std::array<float, 66>>> confidencePastValuesMap;

	std::map<int, cv::Rect> lastFaceRect;

	bool initialModulesRandomize = true;

	std::array<size_t, FaceTracker::MAX_TO_TRACK> initialModules = std::array<size_t, FaceTracker::MAX_TO_TRACK> { 0, 0, 0, 0, 0 };

	bool detectInside(std::vector<cv::Rect>& opencvFaces, cv::Rect faceRect);

	void resetStable();

	void outerTransform(std::vector<cv::Rect>& opencvFaces, FXModel& face, ExternalRenderParams externalRenderParams);
	void outerDraw(FXModel& face, ExternalRenderParams &externalRenderParams);

	virtual void load();
	virtual void transform(FXModel& face, ExternalRenderParams &externalRenderParams);
	virtual void draw(FXModel& face, ExternalRenderParams &externalRenderParams);
	virtual void unload();

	virtual void drawParticles(Matrix4f& projection);

	virtual void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel); // temporary

	virtual void applyModule(FilterModule *module, size_t faceIndex, bool loadTexturesImmediately);

	virtual void onInputFrameResized();

	void loadFromJSON(const fs::path& path, FilterUiModel* externalInfo = nullptr);
	void saveToJSON(const fs::path& filterFolder, ExtraFilterData &extraData);

	virtual void transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model);

	std::vector<std::shared_ptr<ParticleEffect>> particleEffects;
};

#endif