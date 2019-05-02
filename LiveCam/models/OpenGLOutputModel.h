#ifndef OPENGLOUTPUTMODEL_H
#define OPENGLOUTPUTMODEL_H

#include <vector>
#include <models\OpenGLModel.h>

#pragma once

class OpenGLOutputModel {
public:
	bool isUp;
	std::vector<OpenGLModel> fxModels;
};
#endif