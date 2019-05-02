#pragma once

#include "boost/variant.hpp"
#include "boost/property_tree/ptree.hpp"

#include "Eigen/Geometry"

#include "glsl.h"
#include <string>

typedef GLint TUniform1i;
typedef GLfloat TUniform1f;
typedef Eigen::Vector2f TUniform2f;
typedef Eigen::Vector3f TUniform3f;
typedef Eigen::Vector4f TUniform4f;
typedef Eigen::Matrix4f TUniform16;

typedef boost::variant<TUniform1i, TUniform1f, TUniform2f, TUniform3f, TUniform4f, TUniform16> TUniformVariant;

class ObjectRenderParamVisitor_GLSetter : public boost::static_visitor<void>
{
public:
	std::shared_ptr<cwc::glShader> usedShader;
	std::string uniformName;

	void operator()(TUniform1i i) const;

	void operator()(TUniform1f f) const;

	void operator()(TUniform2f f2) const;

	void operator()(TUniform3f f3) const;

	void operator()(TUniform4f f4) const;

	void operator()(TUniform16 M16) const;
};

class ObjectRenderParamVisitor_toPTree : public boost::static_visitor<boost::property_tree::ptree>
{
public:
	std::string uniformName;

	boost::property_tree::ptree operator()(TUniform1i i) const;

	boost::property_tree::ptree operator()(TUniform1f f) const;

	boost::property_tree::ptree operator()(TUniform2f f2) const;

	boost::property_tree::ptree operator()(TUniform3f f3) const;

	boost::property_tree::ptree operator()(TUniform4f f4) const;

	boost::property_tree::ptree operator()(TUniform16 M16) const;
};