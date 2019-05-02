#include "ImgMesh.h"

ImgMesh::ImgMesh(const ImgMesh& t) {
	m_xmat = t.m_xmat;
	m_ymat = t.m_ymat;
}
ImgMesh& ImgMesh::operator=(const ImgMesh& t) {
	m_xmat = t.m_xmat;
	m_ymat = t.m_ymat;
	return *this;
}

ImgMesh::ImgMesh()
{

}
ImgMesh::~ImgMesh()
{

}
void ImgMesh::init(int rows, int cols)
{
	m_xmat.create(rows, cols);
	m_ymat.create(rows, cols);

	m_xmat = 0;
	m_ymat = 0;
	m_vecpoints.clear();
}
void ImgMesh::reset() {
	m_xmat = 0;
	m_ymat = 0;
}

cv::Size ImgMesh::size() const {
	return m_xmat.size();
}

void ImgMesh::setPoint(int ix, int iy, float x, float y)
{
	m_xmat[iy][ix] = x;
	m_ymat[iy][ix] = y;
}
cv::Point2f ImgMesh::getPoint(int ix, int iy) const
{
	return cv::Point2f(m_xmat[iy][ix], m_ymat[iy][ix]);
}