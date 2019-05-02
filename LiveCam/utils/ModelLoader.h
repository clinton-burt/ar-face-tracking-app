#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/filesystem.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include <array>

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "../models/DrawObjectsModel.h"
#include "../tracker/FaceTracker.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <tinyobjloader\tiny_obj_loader.h>

#include <utils\trackball.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <windows.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <mmsystem.h>
#ifdef __cplusplus
}
#endif
#pragma comment(lib, "winmm.lib")

using namespace std;
using namespace Eigen;

namespace fs = boost::filesystem;

class ModelLoader {

public:

	static std::string GetBaseDir(const std::string &filepath);

	static bool FileExists(const std::string &abs_filename);

	static Vector4f CalculateQuat(Vector3f vecFrom, Vector3f vecTo);

	static Vector3f RotateByQuat(Vector3f vec, Vector4f quat);


	static bool LoadTexture(const std::string& texture_filename, GLuint& id);

	static void LoadObj(std::vector<std::string> &objFiles, std::vector<DrawObjectsModel>& drawObjects,
		std::array<float, 3>& bmin, std::array<float, 3>& bmax, float extraScale = 1.f, Vector3f extraShift = { 0, 0, 0 }, Matrix3f extraRotateMatrix = Eigen::Matrix3f::Identity());

	static bool LoadObjAndConvert(std::string &filename, std::vector<DrawObjectsModel>& drawObjects,
		std::vector<tinyobj::material_t>& materials, std::map<std::string, GLuint>& textures,
		std::array<float, 3>&, std::array<float, 3>& bmax, float extraScale = 1.f, Vector3f extraShift = { 0, 0, 0 });

	static DrawObjectsModel CreateQuadModel();

	static bool LoadCubemapTexture(const std::array<std::string, 6>& texture_filename, GLuint& id);

	static boost::property_tree::ptree convertMTLtoPTree(const std::string &path, const std::string &resourcesRoot);

	static void loadMTLnames(std::vector<std::string> &objFiles, std::vector<DrawObjectsModel>& drawObjects);
	static void loadObjectsNames(std::vector<std::string> &objFiles, std::vector<std::string>& objectsNames);
};
#endif