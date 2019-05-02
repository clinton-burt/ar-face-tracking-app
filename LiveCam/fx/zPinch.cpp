#include "ZPinch.h"
#include "utils\PointCalcUtil.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define DEF_MESH_ROWS 91
#define DEF_MESH_COLS 121

ZPinch::ZPinch() {
//	m_gridsize = 8; // dont change
	m_mesh_rows = DEF_MESH_ROWS;
	m_mesh_cols = DEF_MESH_COLS;
	m_step = 16;
	m_margin = m_step * 2;
}
ZPinch::ZPinch(const cv::Size& imgsize)
{
	init(imgsize, DEF_MESH_ROWS, DEF_MESH_COLS, 8, 32);
}
ZPinch::ZPinch(const cv::Size& imgsize, const ImgMesh& srcMesh)
{
	init(imgsize, DEF_MESH_ROWS, DEF_MESH_COLS, 8, 32);
	m_imgMesh = srcMesh;
	m_dstMesh = srcMesh;
}

void ZPinch::init(const cv::Size& imgsize, int mesh_rows, int mesh_cols, int gridsize, int step)
{
	int width = imgsize.width;
	int height = imgsize.height;

	m_imgsize = imgsize;
	m_mesh_rows = mesh_rows;
	m_mesh_cols = mesh_cols;
	ImgMesh& imgMesh = m_imgMesh;
	imgMesh.init(mesh_rows, mesh_cols);

	ImgMesh& dstMesh = m_dstMesh;
	dstMesh.init(mesh_rows, mesh_cols);

	for (int iy = 0; iy < mesh_rows; iy++)
	{
		for (int ix = 0; ix < mesh_cols; ix++)
		{
			float rx = ix*(1.0f / (float)(mesh_cols - 1)) * width;
			float ry = iy*(1.0f / (float)(mesh_rows - 1)) * height;
			imgMesh.setPoint(ix, iy, rx, ry);
			dstMesh.setPoint(ix, iy, rx, ry);
		}
	}
}

void ZPinch::simplePinch()
{
	int xmin, xmax, ymin, ymax, xmid, ymid;
	xmin = 0;
	ymin = 0;
	xmax = m_mesh_cols;
	ymax = m_mesh_rows;

	xmid = (int)(m_imgsize.width / 2);
	ymid = (int)(m_imgsize.height / 2);
	int m_nrmax = (int)sqrt((float)((xmid - xmin)*(xmid - xmin) + (ymid - ymin)*(ymid - ymin)));

	ImgMesh& imgMesh = m_imgMesh;
	ImgMesh& dstMesh = m_dstMesh;

	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f pt = imgMesh.getPoint(ix, iy);
			float rx = pt.x;
			float ry = pt.y;
			imgMesh.setPoint(ix, iy, rx, ry);

			float nx = xmid - rx;
			float ny = ymid - ry;
			double radius = sqrt((float)(nx*nx + ny*ny));
			if (radius < m_nrmax)
			{
				double angle = atan2((double)ny, (double)nx);
				double rnew = radius*radius / m_nrmax;
				//				double rnew = sqrt(radius*m_nrmax);
				rx = xmid + (int)(rnew * cos(angle));
				ry = ymid - (int)(rnew * sin(angle));
				dstMesh.setPoint(ix, iy, rx, ry);
			}
		}
	}
}
void ZPinch::doForwardWarp(const cv::Point2f& fromPt, const cv::Point2f& toPt, float brush_size, float brush_pressure)
{
	ImgMesh prevMesh = m_dstMesh;
	float dx = (toPt.x - fromPt.x)*brush_pressure;
	float dy = (toPt.y - fromPt.y)*brush_pressure;
	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f vprev = prevMesh.getPoint(ix, iy);
			float r = PointCalcUtil::distanceBetween(vprev, fromPt);
			if (r > brush_size)
				continue;
			float t = r / brush_size;
			float scale = 0.5f*(cosf(t*M_PI) + 1.0f);
			m_dstMesh.setPoint(ix, iy, vprev.x + dx*scale, vprev.y + dy*scale);
		}
	}
}
void ZPinch::doMagnifyWarp(const cv::Point2f& pt, float brush_size, float brush_pressure)
{
	ImgMesh prevMesh = m_dstMesh;
	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f vprev = prevMesh.getPoint(ix, iy);
			float r = PointCalcUtil::distanceBetween(vprev, pt);
			if (r > brush_size)
				continue;
			float t = r / brush_size;
			float scale = 0.5f*(cosf(t*M_PI) + 1.0f);
			cv::Point2f newpt = vprev + brush_pressure*scale*(vprev - pt);
			m_dstMesh.setPoint(ix, iy, newpt.x, newpt.y);
		}
	}
}
void ZPinch::doMagnifyWarpHeight(const cv::Point2f& pt, float brush_size, float brush_pressure)
{
	ImgMesh prevMesh = m_dstMesh;
	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f vprev = prevMesh.getPoint(ix, iy);
			float r = PointCalcUtil::distanceBetween(vprev, pt);
			if (r > brush_size)
				continue;
			float t = r / brush_size;
			float scale = 0.5f*(cosf(t*M_PI) + 1.0f);
			cv::Point2f newpt = vprev + brush_pressure*scale*(vprev - pt);

			float tx = (float)std::abs(vprev.x - pt.x) / (float)brush_size;
			newpt.x = vprev.x + brush_pressure*scale*powf(tx, 4.0f)*(vprev.x - pt.x);
			if (vprev.y > pt.y)
			{
				float ty = (float)(vprev.y - pt.y) / (float)brush_size;
				scale *= powf(ty, 1.5f);
				newpt.y = vprev.y + brush_pressure*scale*(vprev.y - pt.y);
			}
			m_dstMesh.setPoint(ix, iy, newpt.x, newpt.y);
		}
	}
}
void ZPinch::doMagnifyWarpHeight2(const cv::Point2f& pt, float brush_size, float brush_pressure)
{
	ImgMesh prevMesh = m_dstMesh;
	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f vprev = prevMesh.getPoint(ix, iy);
			float r = PointCalcUtil::distanceBetween(vprev, pt);
			if (r > brush_size)
				continue;
			float t = r / brush_size;
			float scale = 0.5f*(cosf(t*M_PI) + 1.0f);
			cv::Point2f newpt = vprev + brush_pressure*scale*(vprev - pt);
			newpt.x = vprev.x;
			vprev.y = pt.y;
			m_dstMesh.setPoint(ix, iy, newpt.x, newpt.y);
		}
	}
}

void ZPinch::doMagnifyWarpEllipse(const cv::Point2f& pt, float brush_size, float brush_pressure_x, float brush_pressure_y)
{
	ImgMesh prevMesh = m_dstMesh;
	for (int iy = 0; iy < m_mesh_rows; iy++)
	{
		for (int ix = 0; ix < m_mesh_cols; ix++)
		{
			cv::Point2f vprev = prevMesh.getPoint(ix, iy);
			float r = PointCalcUtil::distanceBetween(vprev, pt);
			if (r > brush_size)
				continue;
			float t = r / brush_size;
			float scale = 0.5f*(cosf(t*M_PI) + 1.0f);
			cv::Point2f newpt;
			newpt.x = vprev.x + brush_pressure_x*scale*(vprev.x - pt.x);
			newpt.y = vprev.y + brush_pressure_y*scale*(vprev.y - pt.y);

			//newpt.x = vprev.x;
			//vprev.y = pt.y;
			/*if (vprev.y > pt.y)
			{
			float ty = (float)(vprev.y - pt.y) / (float)brush_size;
			scale *= powf(ty, 1.5f);
			newpt.y = vprev.y + brush_pressure*scale*(vprev.y - pt.y);
			}*/
			m_dstMesh.setPoint(ix, iy, newpt.x, newpt.y);
		}
	}
}