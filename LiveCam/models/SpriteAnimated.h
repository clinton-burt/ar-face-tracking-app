#pragma once

#include <utils\ModelLoader.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

class SpriteAnimated
{
public:
	SpriteAnimated();

	virtual void load();
	virtual void draw(Eigen::Matrix4f &externalModelMatrix, Eigen::Matrix4f &externalProjectionMatrix);
	virtual void unload();

	virtual void animate();

	DrawObjectsModel object;

	std::shared_ptr<cwc::glShader> shader;

	Eigen::Matrix4f modelMatrix; // will be combined with 3dModel modelMatrix

	std::string animationPath;
	Eigen::Vector2f animationSize;

	Eigen::Vector3f animationOffset;
	Eigen::Vector2f animationScale;

	size_t animationLength;

	std::vector<GLuint>animationFramesIDs;
	size_t animationIndex;
};