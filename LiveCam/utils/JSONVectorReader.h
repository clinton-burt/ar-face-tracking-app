#pragma once

#include <array>
#include <vector>
#include <string>

#include "Eigen/Geometry"
#include "boost/property_tree/ptree.hpp"

#include "models/Uniforms.h"

class JSONVectorReader
{
public:
	template<typename T, size_t N>
	static std::array<T, N> readVector(boost::property_tree::ptree& JSONvector)
	{
		std::array<T, N> result;
		auto iter = result.begin();
		for (auto& element : JSONvector)
		{
			*iter++ = element.second.get_value<T>();
		}
		return result;
	}

	static std::pair<std::string, TUniformVariant> readUniform(boost::property_tree::ptree& JSONvector);

	static Eigen::Vector2f readVector2f(boost::property_tree::ptree& JSONvector);

	static Eigen::Vector3f readVector3f(boost::property_tree::ptree& JSONvector);

	static Eigen::Vector4f readVector4f(boost::property_tree::ptree& JSONvector);

	static Eigen::Matrix3f readMatrix3f(boost::property_tree::ptree& JSONvector);

	static Eigen::Matrix4f readMatrix4f(boost::property_tree::ptree& JSONvector);
};