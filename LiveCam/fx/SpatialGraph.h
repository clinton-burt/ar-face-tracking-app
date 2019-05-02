#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include "math3d.h"

#define MAX_MODEL_NODE_NUM 66

class SpatialGraph
{
public:
	SpatialGraph();
	SpatialGraph(const SpatialGraph& t);
	~SpatialGraph();

	SpatialGraph& operator= (const SpatialGraph& cInGraph);
	void operator-=(const cv::Point3f& cPoint);
	void operator+=(const cv::Point3f& cPoint);
	void operator-=(const cv::Point2f& cPoint);
	void operator+=(const cv::Point2f& cPoint);

	int		getPointNum() const;
	cv::Rect_<float> getBoundary() const;
	cv::Rect	getBoundRect() const;

	void	transform3D(const Linear3D& t);

	cv::Point2f	getCenter2f() const;
	cv::Point3f	getCenter3f() const;
	bool	calcTransform3D(const SpatialGraph& src_points, Linear3D& trans) const;
public:
	int		m_nNodeNum;
	cv::Point3f	m_pxNodes[MAX_MODEL_NODE_NUM];
};