#pragma once

#include "ImgMesh.h"
#include "Models/OpenGLMesh.h"

class MeshMaker
{
public:
	MeshMaker();
	~MeshMaker();

	//void init(const Mat& image, const Rect& viewRegion);
	void init(const cv::Mat& image, const cv::Rect& viewRegion, const ImgMesh& mesh);
	void setDstMesh(const ImgMesh& dst);

	cv::Rect getViewRegion() const { return m_viewRegion; }
	OpenGLMesh	 getMesh();

protected:
	//private:
	cv::Mat m_image;
	cv::Size m_imgsize;
	cv::Rect m_viewRegion;

	int		m_rows;
	int		m_cols;
	ImgMesh		m_srcMesh;
	ImgMesh		m_dstMesh;
};