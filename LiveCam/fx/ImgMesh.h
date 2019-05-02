#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>

class ImgMesh
{
public:
	ImgMesh();
	ImgMesh(const ImgMesh& t);
	~ImgMesh();

	ImgMesh& operator=(const ImgMesh& t);

	void init(int rows, int cols);
	void setPoint(int ix,int iy, float x, float y);
	cv::Point2f getPoint(int ix, int iy) const;
	cv::Size size() const;
	void reset();
private:
	cv::Mat1f m_xmat, m_ymat;
	std::vector<cv::Point2f>		m_vecpoints;
};
