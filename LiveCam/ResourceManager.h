#pragma once

#include "ModelLoader.h"
#include "ModelUtil.h"
#include <unordered_map>

#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

class ResourceManager
{
public:
	boost::filesystem::path resourceRoot = "";

	struct TextureLoadingResult
	{
		GLuint ID;
		size_t textureWidth;
		size_t textureHeight;
	};

	struct AnimationLoadingResult
	{
		std::vector<GLuint> IDs;
		size_t animationWidth;
		size_t animationHeight;
		size_t animationLength;
	};

	struct AtlasLoadingResult
	{
		struct AtlasTexture
		{
			//cv::Rect_<float> atlasRect;
			//Eigen::Vector2f leftTopPadding;
			//Eigen::Vector2f rightBottomPadding;
			Eigen::Vector2f shift;
		};

		GLuint ID;
		size_t atlasWidth;
		size_t atlasHeight;
		Eigen::Vector2f atlasTextureSize;
		size_t count;
		std::vector<AtlasTexture> atlasTextures;
	};

	TextureLoadingResult loadTexture(std::string &texture);
	TextureLoadingResult loadTexture(std::string &texture, cv::Mat image);
	AnimationLoadingResult loadAnimation(std::string &folderpath);
	AtlasLoadingResult loadAtlas(std::string &atlaspath); // atlas file must contain a file with similar name but .json extension in the same directory
	GLuint loadCubemap(std::array<std::string, 6> &cubemapTextures);

	void removeTextures(std::vector<std::string> &textures);
	void removeAnimation(std::string &animationPath);
	void removeCubemap(std::array<std::string, 6> &cubemapTextures);
	void clear();

private:
	std::unordered_map<std::string, TextureLoadingResult> cachedTextures;
	std::unordered_map<std::string, AnimationLoadingResult> cachedAnimations;
	std::unordered_map<std::string, AtlasLoadingResult> cachedAtlases;
	std::unordered_map<std::string, GLuint> cachedCubemaps;
};