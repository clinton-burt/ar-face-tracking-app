#include <fx\FxCore.h>


#include "utils/stb_image.h"

#include "../opencv/build/include/opencv2/objdetect/objdetect.hpp"

#include "../opencv/build/include/opencv2/imgproc/imgproc.hpp"

#include "Fx3dModelAncestor.h"
#include "FxEmojiAncestor.h"
#include "FxAnimatedEmojiAncestor.h"
#include "FxFacePaintAncestor.h"
#include "DepthMask.h"
#include "AlphaMask.h"
#include "FxFaceSwapModel.h"
#include "fx/FxAnimatedStandardEmojiAncestor.h"

#include "ui/LiveCamWindow.h"

ParticleManager particleManager;

FilterModule::FilterModule()
{
	moduleID = -1;
	externFilterID = -1;
	externModuleID = -1;
}

FilterModule::FilterModule(boost::property_tree::ptree &moduleTree, size_t moduleID, const fs::path &resourceRoot)
{
	externFilterID = moduleTree.get<int>("externFilterID", -1);
	externModuleID = moduleTree.get<int>("externModuleID", -1);

	this->moduleID = moduleID;

	iconPath = moduleTree.get<std::string>("iconPath", "");

	icon.addFile(QString(fs::path(resourceRoot / iconPath).string().c_str()), QSize(40, 40), QIcon::Normal);
	icon.addFile(QString(fs::path(resourceRoot / iconPath).string().c_str()), QSize(40, 40), QIcon::Disabled);

	if (externFilterID > -1)
	{
		return;
	}

	auto suitsTree = moduleTree.get_child_optional("suits");
	if (suitsTree)
	{
		for (auto &suit : suitsTree.get())
		{
			suits.push_back(suit.second);
		}
	}
}

boost::property_tree::ptree FilterModule::getPTree(ExtraModuleData &data, std::vector<std::shared_ptr<OpenGLModel>> &models,
	TCommonRenderParams& commonRenderParams, int moduleIndex)
{
	boost::property_tree::ptree result;

	fs::path path(data.moduleIconPath);

	result.put("iconPath", path.filename().string());

	boost::property_tree::ptree suitsTree;

	int i = 0;
	auto model = models.begin();
	for (auto &suitRecord : suits)
	{
		if (i++ == moduleIndex)
		{
			models[moduleIndex]->prepareSuitForJSON(suits[moduleIndex], data.models[0]);
		}
		suitsTree.push_back(make_pair("", suitRecord));
	}

	result.put_child("suits", suitsTree);

	if (!data.moduleIconPath.empty())
	{
		auto copiedFile = fs::path(data.root->filterFolder / path.filename());
		if (!equivalent(path.string(), copiedFile))
		{
			fs::copy_file(path.string(), copiedFile, fs::copy_option::overwrite_if_exists);
		}
	}

	return result;
}

FxCore::FxCore()
{ 
	resetStable();
	resourcesRoot = "";
	editable = false;
	useFrameLightParams = false;
	useFaceLightParams = false;
}

FxCore::~FxCore()
{ 
}


void FxCore::outerTransform(std::vector<cv::Rect>& opencvFaces, FXModel& face, ExternalRenderParams externalRenderParams)
{
#ifdef ADDITIONAL_FALSE_FILTER
		if (!becameStable[face.pointId] && detectInside(opencvFaces, face.faceRect))
		{
			becameStable[face.pointId] = true;

			lastFaceRect[face.pointId] = face.faceRect;

		}

		if (!becameStable[face.pointId])
		{
			return;
		}

		auto rect1 = face.faceRect;
		auto rect2 = lastFaceRect[face.pointId];

		auto rect3 = rect1 & rect2;

		std::cout << rect3.height;
		std::cout << rect3.width;

		std::cout << rect3.area();

		bool intersects = (rect3.area() > 0);

		if (!intersects)
		{
			becameStable[face.pointId] = false;
			return;
		}

		lastFaceRect[face.pointId] = face.faceRect;
#endif

	this->transform(face, externalRenderParams);

}

void FxCore::outerDraw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
#ifdef ADDITIONAL_FALSE_FILTER
	if (becameStable[face.pointId])
	{
		this->draw(face);
	}
#else
	this->draw(face, externalRenderParams);
#endif
}

bool FxCore::detectInside(std::vector<cv::Rect>& opencvFaces, cv::Rect faceRect)
{

	for (size_t i = 0; i < opencvFaces.size(); i++)
	{
		if ((faceRect.x+ faceRect.width/2 > opencvFaces[i].x + opencvFaces[i].width/4) &&
			(faceRect.x + faceRect.width / 2 < opencvFaces[i].x + (opencvFaces[i].width * 3) / 4) &&
			(faceRect.y + faceRect.height / 2 > opencvFaces[i].y + opencvFaces[i].height / 4) &&
			(faceRect.y + faceRect.height / 2 < opencvFaces[i].y + (opencvFaces[i].height * 3) / 4))
		{
			return true;
		}
	}

	return false;
}

void FxCore::resetStable()
{
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; i++)
	{
		becameStable[i] = false;
	}
}

void FxCore::applyModule(FilterModule *module, size_t faceIndex, bool loadTexturesImmediately)
{
	FxCore* filterPtr = nullptr;
	FilterModule* modulePtr = nullptr;

	if (module->externFilterID == -1)
	{
		modulePtr = module;
		filterPtr = this;
	}
	else
	{
		filterPtr = externFilters[module->externFilterID].get();
		if (module->externModuleID > -1)
		{
			modulePtr = &filterPtr->filterModules[module->externModuleID];
		}
	}

	std::vector<FxCore*> otherFilters;

	if (module->externFilterID != -1)
	{
		otherFilters.push_back(this);
	}

	int index = 0;
	for (auto other : externFilters)
	{
		if (module->externFilterID != index)
		{
			otherFilters.push_back(other.get());
		}
		++index;
	}

	boost::property_tree::ptree disablingSuit;
	disablingSuit.put("visible", "false");

	if (modulePtr)
	{
		std::vector<bool> appliedSuit(filterPtr->models.size(), false);

		int modelID = 0;
		int suitID = 0;
		for (auto suit : modulePtr->suits)
		{
			while (!filterPtr->models[modelID]->canSwapSuit)
			{
				appliedSuit[modelID] = true;
				++modelID;
			}

			int modelID_local = modelID;

			auto modelIDrecord = suit.get_optional<int>("modelID");
			if (modelIDrecord)
			{
				modelID_local = modelIDrecord.get();
			}

			++suitID;
			++modelID;
			
			auto model = filterPtr->models[modelID_local];

			appliedSuit[modelID_local] = true;

			if (model->canSwapSuit)
			{
				model->applySuit(suit, faceIndex, filterPtr->commonRenderParams, loadTexturesImmediately);
			}
		}

		for (modelID = 0; modelID < filterPtr->models.size(); ++modelID)
		{
			if (!appliedSuit[modelID])
			{
				filterPtr->models[modelID]->applySuit(disablingSuit, faceIndex, filterPtr->commonRenderParams, loadTexturesImmediately);
			}
		}
	}
	else
	{
		boost::property_tree::ptree enablingSuit;
		enablingSuit.put("visible", "true");
		for (auto model : filterPtr->models)
		{
			if (model->canSwapSuit)
			{
				model->applySuit(enablingSuit, faceIndex, filterPtr->commonRenderParams, loadTexturesImmediately);
			}
		}
	}

	for (auto other : otherFilters) // this & external filters
	{
		for (auto model : other->models)
		{
			if (model->canSwapSuit)
			{
				model->applySuit(disablingSuit, faceIndex, other->commonRenderParams, loadTexturesImmediately);
			}
		}
	}
}

void FxCore::onInputFrameResized()
{
	for (auto model : models)
	{
		model->onInputFrameResized();
	}

	for (auto filter : externFilters)
	{
		for (auto model : filter->models)
		{
			model->onInputFrameResized();
		}
	}
}

void FxCore::load()
{
	if (!cubemapTextures[0].empty())
	{
		cubemapID = resourceManager.loadCubemap(cubemapTextures);
	}
	
	srand(time(NULL));
	
	if (filterModules.size() > 0)
	{
		for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		{
			initialModulesRandomize
				? applyModule(&filterModules[rand() % filterModules.size()], i, false)
				: applyModule(&filterModules[initialModules[i]], i, false);
		}

	}
	
	useAlphaMask = false;

	for (auto model : models)
	{
		model->load();
		if (!useAlphaMask && model->getTypeName() == "AlphaMask")
		{
			useAlphaMask = true;
		}
	}

	for (auto filter : externFilters)
	{
		for (auto model : filter->models)
		{
			model->load();
		}
	}

	for (auto effect : particleEffects)
	{
		effect->load();
		effect->startSpawn();
	}
}

void FxCore::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	for (auto model : models)
	{
		if (model->visible[face.pointId])
		{
			model->transform(face, externalRenderParams);
		}
	}

	for (auto filter : externFilters)
	{
		filter->transform(face, externalRenderParams);
	}

	for (auto effect : particleEffects)
	{
		ParticleUpdateParams params;

		params.outer_transformation = Matrix4f::Identity();
		params.effect_rotation = Matrix3f::Identity();
		params.effect_scaling = Affine3f(Scaling(effect->getEffectWidth() / Resolutions::OUTPUT_WIDTH)).matrix();
		params.enable_effect_rotation = false;
		params.globalGravity = { 0, 0, 0 };
		params.enable_effect_gravity = true;
		params.enable_global_gravity = false;

		Matrix3f identity3 = Matrix3f::Identity();
		effect->update(externalRenderParams.timeShift, params);
	}
}

void FxCore::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	if (cubemapID != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	}

	for (auto model : models)
	{
		if (!model->visible[face.pointId])
		{
			continue;
		}

		if (model->useAlphaMask[face.pointId])
		{
			frameManager.SwitchToDrawBuffer((size_t)ColorBuffer::FILTER_MASK);
		}
		else
		{
			frameManager.SwitchToDrawBuffer((size_t)ColorBuffer::COLOR_1);
		}

		model->draw(face, externalRenderParams);
	}

	for (auto filter : externFilters)
	{
		filter->draw(face, externalRenderParams);
	}
}

void FxCore::drawParticles(Matrix4f& projection)
{
	for (auto effect : particleEffects)
	{
		effect->draw(projection);
	}
}

void FxCore::unload()
{
	for (auto model : models)
	{
		model->unload();
	}

	for (auto filter : externFilters)
	{
		for (auto model : filter->models)
		{
			model->unload();
		}
	}

	for (auto effect : particleEffects)
	{
		effect->freeze();
		effect->unload();
	}
}

void FxCore::drawOnCVFrame_OLD(cv::Mat frame, FXModel& face)
{
	for (auto model : models)
	{
		if (model->visible[face.pointId])
		{
			model->drawOnCVFrame_OLD(frame, face);
		}
	}

	for (auto filter : externFilters)
	{
		for (auto model : filter->models)
		{
			model->drawOnCVFrame_OLD(frame, face);
		}
	}
}

std::map<std::string, std::function<std::shared_ptr<OpenGLModel>()>> modelTypeAssociations =
{
	{ "3DModel", &OpenGLModel::create<OpenGL3DModel> },
	{ "LipsJoint3DModel", &OpenGLModel::create<LipsJoint3DModel> },
	{ "LipsBottomJoint3DModel", &OpenGLModel::create<LipsBottomJoint3DModel> },
	{ "LeftBrow3DModel", &OpenGLModel::create<LeftBrow3DModel> },
	{ "RightBrow3DModel", &OpenGLModel::create<RightBrow3DModel> },
	{ "EmojiModel", &OpenGLModel::create<OpenGLEmojiModel> },
	{ "Suit2DModel", &OpenGLModel::create<Suit2DModel> },
	{ "AnimatedEmojiModel", &OpenGLModel::create<OpenGLAnimatedEmojiModel> },
	{ "FacePaintModel", &OpenGLModel::create<OpenGLFacePaintModel> },
	{ "DepthMask", &OpenGLModel::create<DepthMask> },
	{ "AlphaMask", &OpenGLModel::create<AlphaMask> },
	{ "FaceSwapModel", &OpenGLModel::create<FaceSwapModel> },
	{ "Static3DModel", &OpenGLModel::create<Static3DModel> },
	{ "AnimatedStandardEmojiModel", &OpenGLModel::create<OpenGLAnimatedStandardEmojiModel> },
	{ "AtlasAnimatedEmojiModel", &OpenGLModel::create<AtlasAnimatedEmojiModel> },
	{ "AtlasAnimatedToungeEmojiModel", &OpenGLModel::create<AtlasAnimatedToungeEmojiModel> }
};

void FxCore::loadFromJSON(const fs::path& path, FilterUiModel* externalInfo)
{
	boost::property_tree::ptree filterTree;
	boost::property_tree::read_json(path.string(), filterTree);

	auto transformVersionString = filterTree.get<std::string>("transform version", "V2");
	TransformVersion transformVersion;

	if (transformVersionString == "V1")
	{
		transformVersion = TransformVersion::V1;
	}
	else if (transformVersionString == "V2")
	{
		transformVersion = TransformVersion::V2;
	}
	else
	{
		throw std::exception("Invalid transform version.");
	}

	useFaceLightParams = filterTree.get<bool>("use face light params", false);
	useFrameLightParams = filterTree.get<bool>("use frame light params", false);

	bool JSONroot = filterTree.get<bool>("JSON root", false);
	if (JSONroot)
	{
		resourcesRoot = path.parent_path();
	}

	auto iconPath = filterTree.get_optional<std::string>("iconPath");
	if (iconPath && externalInfo)
	{
		externalInfo->setIcon(iconPath.get());
	}

	auto title = filterTree.get_optional<std::string>("title");
	if (title && externalInfo)
	{
		externalInfo->setTitle(title.get());
	}

	auto skyboxRecord = filterTree.get_child_optional("skybox");
	if (skyboxRecord)
	{
		cubemapTextures = JSONVectorReader::readVector<std::string, 6>(skyboxRecord.get());
	}

	auto modelsList = filterTree.get_child_optional("models");
	if (modelsList)
	{
		editable = true;

		for (auto &modelRecord : modelsList.get())
		{
			std::string modelType = modelRecord.second.get<std::string>("type", "");

			auto model = modelTypeAssociations[modelType]();

			model->loadFromJSON(modelRecord.second);

			model->useHardCodedUniforms = false;

			model->transformVersion = transformVersion;

			if (transformVersion == TransformVersion::V1) // some workaround ?
			{
				if (model->getTypeName() == DepthMask::TYPE_NAME)
				{
					DepthMask* mask = (DepthMask*)model.get();
					mask->modelScale = mask->modelScale == 1.9f ? 1 : mask->modelScale;
					mask->modelShift = mask->modelShift == Eigen::Vector3f(0, -0.33f, 0) ?
						Eigen::Vector3f(0, 0, 0) : mask->modelShift;
				}
			}

			models.push_back(model);
		}
	}

	auto renderParamsList = filterTree.get_child_optional("renderParams");
	if (renderParamsList)
	{
		for (auto &renderParams : renderParamsList.get())
		{
			commonRenderParams.push_back(make_shared<ObjectRenderParams>(renderParams.second, resourcesRoot));
		}
	}

	auto modulesList = filterTree.get_child_optional("modules");
	if (modulesList)
	{
		size_t ID = 0;
		for (auto &module : modulesList.get())
		{
			filterModules.push_back(FilterModule(module.second, ID++, resourcesRoot));
		}
	}

	auto initialModulesList = filterTree.get_child_optional("initialModules");
	initialModules = initialModulesList ?
		JSONVectorReader::readVector<size_t, FaceTracker::MAX_TO_TRACK>(initialModulesList.get()) :
		array<size_t, FaceTracker::MAX_TO_TRACK> { 0, 0, 0, 0, 0 };

	initialModulesRandomize = filterTree.get<bool>("initialModulesRandomize", true);

	auto particles = filterTree.get_child_optional("particles");
	if (particles)
	{
		for (auto &particleEffect : particles.get())
		{
			auto effect = particleManager.addEffect(particleEffect.second.get<std::string>("JSONsource", ""));
			if (effect)
			{
				particleEffects.push_back(effect);
			}

			auto coordsRecord = particleEffect.second.get_child_optional("coords");
			auto coords = coordsRecord ? JSONVectorReader::readVector3f(coordsRecord.get()) : Vector3f(0.5f, 0.5f, 0);
			coords -= Vector3f(0.5, 0.5, 0);

			effect->setCoords({ coords[0] * Resolutions::OUTPUT_WIDTH, coords[1] * Resolutions::OUTPUT_HEIGHT, coords[2] / 10000 });
			effect->setDirection({ 0, 0 });
			effect->setSpeed({ 0, 0, 0 });
			effect->setDepthTest(false);
		}
	}
}

void FxCore::saveToJSON(const fs::path& filterFolder, ExtraFilterData &data)
{
	boost::property_tree::ptree filterTree;

	data.filterFolder = filterFolder;

	filterTree.put("JSON root", "true");

	filterTree.put("use newest transform", "true");

	if (!data.title.empty())
	{
		filterTree.put("title", data.title);
	}

	if (!data.filterIconPath.empty())
	{
		filterTree.put("iconPath", fs::path(data.filterIconPath).filename().string());

		auto copiedFile = fs::path(filterFolder / fs::path(data.filterIconPath).filename());
		if (!equivalent(data.filterIconPath, copiedFile))
		{
			fs::copy_file(data.filterIconPath, copiedFile, fs::copy_option::overwrite_if_exists);
		}
	}

	if (!cubemapTextures[0].empty())
	{
		boost::property_tree::ptree skyboxTree;

		for (auto &texture : cubemapTextures)
		{
			boost::property_tree::ptree skyboxTreeChild;
			skyboxTreeChild.put("", texture);
			skyboxTree.push_back(make_pair("", skyboxTreeChild));
		}

		filterTree.put_child("skybox", skyboxTree);
	}

	boost::property_tree::ptree modelsTree;
	if (models.size() > 0)
	{
		auto extraModuleData = data.modules.begin();
		for (auto model : models)
		{
			modelsTree.push_back(std::make_pair("", (model->getPTree(extraModuleData++->models[0]))));
		}
	}

	boost::property_tree::ptree initialModulesTree;
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		boost::property_tree::ptree initialModulesTreeChild;
		initialModulesTreeChild.put("", 0);
		initialModulesTree.push_back(make_pair("", initialModulesTreeChild));
	}

	data.renderParams.clear();
	commonRenderParams.clear();
	boost::property_tree::ptree modulesTree;
	if (filterModules.size() > 0)
	{
		auto extraModuleData = data.modules.begin();
		for (int index = 0; index < filterModules.size(); ++index)
		{
			modulesTree.push_back(std::make_pair("", (filterModules[index].getPTree(*extraModuleData++, models, commonRenderParams, index))));
		}
	}

	boost::property_tree::ptree renderParamsTree;
	if (commonRenderParams.size() > 0)
	{
		auto renderParamsData = data.renderParams.begin();
		for (auto renderParams : commonRenderParams)
		{
			renderParamsTree.push_back(std::make_pair("", (renderParams->getPTree(*renderParamsData++))));
		}
	}

	if (models.size() > 0)
	{
		filterTree.put_child("models", modelsTree);
	}

	if (commonRenderParams.size() > 0)
	{
		filterTree.put_child("renderParams", renderParamsTree);
	}

	if (filterModules.size() > 0)
	{
		filterTree.put_child("initialModules", initialModulesTree);
		filterTree.put_child("modules", modulesTree);
	}

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		applyModule(&filterModules[0], i, true); // FIX
	}

	boost::property_tree::write_json((filterFolder / "filter-config.json").string(), filterTree);
}

void FxCore::transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model)
{

}