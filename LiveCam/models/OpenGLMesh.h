#ifndef OPENGLMESH_H
#define OPENGLMESH_H

#include <vector>
#include <opencv2/core/core.hpp>

#pragma once

class OpenGLMesh {
public:
	int rows, cols;
	std::vector<cv::Point2f>		vertices;
	std::vector<cv::Point2f>		uvs;
	std::vector<ushort>				idxs;

	OpenGLMesh() {}
	~OpenGLMesh() {}
	OpenGLMesh(const OpenGLMesh& t) {
		rows = t.rows;
		cols = t.cols;
		vertices.resize(t.vertices.size());
		uvs.resize(t.uvs.size());
		idxs.resize(t.idxs.size());
		std::copy(t.vertices.begin(), t.vertices.end(), vertices.begin());
		std::copy(t.uvs.begin(), t.uvs.end(), uvs.begin());
		std::copy(t.idxs.begin(), t.idxs.end(), idxs.begin());
	}
	void release() {
		vertices.clear();
		uvs.clear();
		idxs.clear();
	}
};
#endif