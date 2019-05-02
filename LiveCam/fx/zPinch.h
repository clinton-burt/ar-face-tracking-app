#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>

#include "ImgMesh.h"

class ZPinch
{
public:
	ZPinch();
	ZPinch(const cv::Size& imgsize);
	ZPinch(const cv::Size& imgsize, const ImgMesh& srcMesh);

	void simplePinch();
	void doForwardWarp(const cv::Point2f& fromPt, const cv::Point2f& toPt, float brush_size, float brush_pressure);
	void doMagnifyWarp(const cv::Point2f& pt, float brush_size, float brush_pressure);
	void doMagnifyWarpHeight(const cv::Point2f& pt, float brush_size, float brush_pressure);
	void doMagnifyWarpHeight2(const cv::Point2f& pt, float brush_size, float brush_pressure);
	void doMagnifyWarpEllipse(const cv::Point2f& pt, float brush_size, float brush_pressure_x, float brush_pressure_y);

	ImgMesh getSrcMesh() const { return m_imgMesh; }
	ImgMesh getDstMesh() const { return m_dstMesh; }
	cv::Size size() const{ return cv::Size(m_mesh_cols, m_mesh_rows); }
private:
	void init(const cv::Size& imgsize, int mesh_rows, int mesh_cols, int gridsize = 8, int step = 32);
	
	int m_mesh_rows;
	int m_mesh_cols;

	int m_step;

	ImgMesh		m_imgMesh;
	ImgMesh		m_dstMesh;

	cv::Size		m_imgsize;

	int m_margin;
};