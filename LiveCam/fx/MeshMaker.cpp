#include "MeshMaker.h"

using namespace std;

#define ROWS 96
#define COLS 96

MeshMaker::MeshMaker()
{
}
MeshMaker::~MeshMaker()
{
}

void MeshMaker::init(const cv::Mat& image, const cv::Rect& viewRegion, const ImgMesh& mesh)
{
	m_srcMesh = mesh;

	m_image = image;
	m_imgsize = image.size();
	m_viewRegion = viewRegion;
	m_rows = m_srcMesh.size().height - 1;
	m_cols = m_srcMesh.size().width - 1;

	m_srcMesh = mesh;
	m_dstMesh = m_srcMesh;
}

void MeshMaker::setDstMesh(const ImgMesh& dst)
{
	m_dstMesh = dst;
}

OpenGLMesh	MeshMaker::getMesh()
{
	int count = (m_rows + 1)*(m_cols + 1);
	OpenGLMesh mesh;
	mesh.rows = m_rows;
	mesh.cols = m_cols;
	mesh.vertices.resize(count);
	mesh.uvs.resize(count);
	mesh.idxs.resize(m_rows*m_cols * 6);

	float aspectratio = 1.0f;
	float scalex = 1.0f / (float)m_imgsize.width;
	float scaley = 1.0f / (float)m_imgsize.height;
	for (int i = 0; i < count; i++)
	{
		int ix = i % (m_cols + 1);
		int iy = i / (m_cols + 1);
		mesh.uvs[i] = m_srcMesh.getPoint(ix, iy);
		mesh.uvs[i].x *= scalex;
		mesh.uvs[i].y = mesh.uvs[i].y * scaley;

		mesh.vertices[i] = m_dstMesh.getPoint(ix, iy);
		mesh.vertices[i].x *= scalex;
		mesh.vertices[i].y = (m_imgsize.height - mesh.vertices[i].y - 1) * scaley*aspectratio;

		mesh.vertices[i].y = 1.f - mesh.vertices[i].y;
	}
	int iquad = 0;
	for (int iy = 0; iy < m_rows; iy++)
	{
		for (int ix = 0; ix < m_cols; ix++, iquad += 6)
		{
			int i00 = iy*(m_cols + 1) + ix;
			int i01 = iy*(m_cols + 1) + ix + 1;
			int i10 = (iy + 1)*(m_cols + 1) + ix;
			int i11 = (iy + 1)*(m_cols + 1) + ix + 1;
			mesh.idxs[iquad] = i00;
			mesh.idxs[iquad + 1] = i01;
			mesh.idxs[iquad + 2] = i11;
			mesh.idxs[iquad + 3] = i00;
			mesh.idxs[iquad + 4] = i11;
			mesh.idxs[iquad + 5] = i10;
		}
	}
	return mesh;
}