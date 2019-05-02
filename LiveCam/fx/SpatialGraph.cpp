#include "SpatialGraph.h"
#include "utils/PointCalcUtil.h"

using namespace cv;

SpatialGraph::SpatialGraph()
{
	m_nNodeNum = 0;
	memset(&m_pxNodes[0], 0, sizeof(Point3f) * MAX_MODEL_NODE_NUM);
}
SpatialGraph::SpatialGraph(const SpatialGraph& t)
{
	m_nNodeNum = t.m_nNodeNum;
	memcpy(m_pxNodes, t.m_pxNodes, m_nNodeNum * sizeof(Point3f));
}
SpatialGraph::~SpatialGraph()
{

}
SpatialGraph& SpatialGraph::operator=(const SpatialGraph& t )
{
	m_nNodeNum = t.m_nNodeNum;
	memcpy(m_pxNodes, t.m_pxNodes, m_nNodeNum * sizeof(Point3f));
	return *this;
}

void SpatialGraph::operator-=(const Point2f& cPoint)
{
	for (int i = 0; i < m_nNodeNum; i ++)
	{
		m_pxNodes[i].x -= cPoint.x;
		m_pxNodes[i].y -= cPoint.y;
	}
}

void SpatialGraph::operator+=(const Point2f& cPoint)
{
	for (int i = 0; i < m_nNodeNum; i ++)
	{
		m_pxNodes[i].x += cPoint.x;
		m_pxNodes[i].y += cPoint.y;
	}
}

void SpatialGraph::operator-=(const Point3f& cPoint)
{
	for (int i = 0; i < m_nNodeNum; i ++)
	{
		m_pxNodes[i].x -= cPoint.x;
		m_pxNodes[i].y -= cPoint.y;
		m_pxNodes[i].z -= cPoint.z;
	}
}

void SpatialGraph::operator+=(const Point3f& cPoint)
{
	for (int i = 0; i < m_nNodeNum; i ++)
	{
		m_pxNodes[i].x += cPoint.x;
		m_pxNodes[i].y += cPoint.y;
		m_pxNodes[i].z += cPoint.z;
	}
}

Point2f SpatialGraph::getCenter2f() const
{
	Point2f center;
	if (m_nNodeNum <= 0)
		return center;
	for (int i = 0; i < m_nNodeNum; i++)
	{
		center.x += m_pxNodes[i].x;
		center.y += m_pxNodes[i].y;
	}
	center.x /= (float)m_nNodeNum;
	center.y /= (float)m_nNodeNum;
	return center;
}

Point3f SpatialGraph::getCenter3f() const
{
	Point3f center;
	if (m_nNodeNum <= 0)
		return center;
	for (int i = 0; i < m_nNodeNum; i++)
	{
		center += m_pxNodes[i];
	}
	center = center * (1.0f/(float)m_nNodeNum);
	return center;
}
void SpatialGraph::transform3D(const Linear3D& t)
{
	if(m_nNodeNum == 0)
		return;
	
	const Point3f& p = t.m_xTranPoint;
	Point3f xTemp1, xTemp2;
	for (int i = 0 ; i < m_nNodeNum ; i++)
	{
		xTemp1 = m_pxNodes[i] - p;
		xTemp2 = t.m_xRotateMat * xTemp1;
		m_pxNodes[i] = p + xTemp2;
	}
	return;
}

bool SpatialGraph::calcTransform3D(const SpatialGraph& src_points, Linear3D& trans) const
{
	float sumXX, sumZZ, sumXZ, sumXY, sumYZ, sumYY, sum;

	Point3f diffpt, diffpt_face, diffpt_sum, diffnormal, pt_offset;
	Linear3D trand3dM, trand3dN;
	int i;
	Point3f centerStd3f = src_points.getCenter3f();
	Point2f xPoint2D = getCenter2f();

	sumXX = 0.0f;  sumZZ = 0.0f; sumYY = 0.0f;
	sumYZ = 0.0f;  sumXZ = 0.0f; sumXY = 0.0f;
	for ( i = 0 ; i < src_points.m_nNodeNum ; i++)
	{
		diffpt.x = src_points.m_pxNodes[i].x - centerStd3f.x;
		diffpt.y = src_points.m_pxNodes[i].y - centerStd3f.y;
		diffpt.z = src_points.m_pxNodes[i].z - centerStd3f.z;

		sumXX += diffpt.x * diffpt.x;
		sumXY += diffpt.x * diffpt.y;
		sumXZ += diffpt.x * diffpt.z;
		sumYY += diffpt.y * diffpt.y;
		sumYZ += diffpt.y * diffpt.z;
		sumZZ += diffpt.z * diffpt.z;
	}
	trand3dM.m_xRotateMat.SetRotateMat(sumXX, sumXY, sumXZ, sumXY, sumYY, sumYZ, sumXZ, sumYZ, sumZZ);
	
	sumXX = 0.0f;  sumZZ = 0.0f; sumYY = 0.0f;
	sumYZ = 0.0f;  sumXZ = 0.0f; sumXY = 0.0f;

	for ( i = 0 ; i < m_nNodeNum ; i++ )
	{
		diffpt.x = src_points.m_pxNodes[i].x - centerStd3f.x;
		diffpt.y = src_points.m_pxNodes[i].y - centerStd3f.y;
		diffpt.z = src_points.m_pxNodes[i].z - centerStd3f.z;

		diffpt_face.x = m_pxNodes[i].x - xPoint2D.x;
		diffpt_face.y = m_pxNodes[i].y - xPoint2D.y;

		sumXX += diffpt.x * diffpt_face.x;
		sumXY += diffpt.y * diffpt_face.x;
		sumXZ += diffpt.z * diffpt_face.x;
		sumZZ += diffpt.x * diffpt_face.y;
		sumYY += diffpt.y * diffpt_face.y;
		sumYZ += diffpt.z * diffpt_face.y;
	}

	trand3dN.m_xRotateMat.SetRotateMat(sumXX, sumXY, sumXZ, sumZZ, sumYY, sumYZ, 0, 0, 0);
	trand3dM.m_xRotateMat.ReverseMat();
	trand3dN.m_xRotateMat *= trand3dM.m_xRotateMat;
	diffpt=Point3f(trand3dN.m_xRotateMat.m_rA1, trand3dN.m_xRotateMat.m_rA2, trand3dN.m_xRotateMat.m_rA3);
	diffpt_face=Point3f(trand3dN.m_xRotateMat.m_rA4, trand3dN.m_xRotateMat.m_rA5, trand3dN.m_xRotateMat.m_rA6);
	sum = 0.0f;

	diffpt_sum = diffpt + diffpt_face;
	sum = diffpt_sum.x * diffpt_sum.x + diffpt_sum.y * diffpt_sum.y + diffpt_sum.z * diffpt_sum.z;
	
	if (fabs(sum) != 0)
	{
		float rBuf = 0.0f;
		rBuf += diffpt.x * diffpt_face.x;
		rBuf += diffpt.y * diffpt_face.y;
		rBuf += diffpt.z * diffpt_face.z;

		sum = rBuf / sum;
		sumZZ = 0.25f - sum;
		
		sum = 0.0f;
		if (sumZZ  <= 0)
			sum = 0.5f - sqrtf(sum);
		else
			sum = 0.5f - sqrtf(sumZZ);

		diffpt_sum = diffpt_sum * sum;
		diffpt = diffpt - diffpt_sum;
		diffpt_face = diffpt_face - diffpt_sum;
	}
	
	sumZZ = 0.0f;
	sumXX = 0.0f;

	sumZZ += diffpt_face.x * diffpt_face.x;
	sumZZ += diffpt_face.y * diffpt_face.y;
	sumZZ += diffpt_face.z * diffpt_face.z;
	sumXX += diffpt.x * diffpt.x;
	sumXX += diffpt.y * diffpt.y;
	sumXX += diffpt.z * diffpt.z;

	sum = (sumZZ + sumXX) * 0.5f;
	sum = sqrtf(sum);
	
	if (fabs(sumXX) != 0)
		PointCalcUtil::normalize(diffpt);
	if (fabs(sumZZ) != 0)
		PointCalcUtil::normalize(diffpt_face);

	diffnormal = PointCalcUtil::cross(diffpt, diffpt_face);
	trand3dM.m_xRotateMat.SetRotateMat(diffpt.x * sum, diffpt.y * sum, diffpt.z * sum,
										diffpt_face.x * sum, diffpt_face.y * sum, diffpt_face.z * sum,
										diffnormal.x * sum, diffnormal.y * sum, diffnormal.z * sum);
	
	diffnormal = trand3dM.m_xRotateMat * centerStd3f;
	

	pt_offset.x = xPoint2D.x;
	pt_offset.y = xPoint2D.y;
	pt_offset.z = 0.0f;

	diffnormal.x = pt_offset.x - diffnormal.x;
	diffnormal.y = pt_offset.y - diffnormal.y;
	diffnormal.z = pt_offset.z - diffnormal.z;
	trans.m_xRotateMat = trand3dM.m_xRotateMat;
	trans.m_xTranPoint = diffnormal;
	return true;
}

Rect SpatialGraph::getBoundRect() const
{
	int nMinX = 1000000, nMaxX = -1000000, nMinY = 1000000, nMaxY = -1000000;
	for (int i = 0; i < m_nNodeNum; i ++)
	{
		if (nMinX > (int)(m_pxNodes[i].x + 0.5f))
			nMinX = (int)(m_pxNodes[i].x + 0.5f);
		if (nMaxX < (int)(m_pxNodes[i].x + 0.5f))
			nMaxX = (int)(m_pxNodes[i].x + 0.5f);
		if (nMinY > (int)(m_pxNodes[i].y + 0.5f))
			nMinY = (int)(m_pxNodes[i].y + 0.5f);
		if (nMaxY < (int)(m_pxNodes[i].y + 0.5f))
			nMaxY = (int)(m_pxNodes[i].y + 0.5f);
	}
	Rect rect;
	rect.x = nMinX;
	rect.y = nMinY;
	rect.width = nMaxX - nMinX;
	rect.height = nMaxY - nMinY;
	return rect;
}
int SpatialGraph::getPointNum() const
{
	return m_nNodeNum;
}

Rect_<float> SpatialGraph::getBoundary() const
{
	Point2f topLeft, xRightBottom;
	topLeft.x = xRightBottom.x = m_pxNodes[0].x;
	topLeft.y = xRightBottom.y = m_pxNodes[0].y;
	for (int i = 1; i < m_nNodeNum; i++)
	{
		if (topLeft.x > m_pxNodes[i].x)
			topLeft.x = m_pxNodes[i].x;
		if (topLeft.y > m_pxNodes[i].y)
			topLeft.y = m_pxNodes[i].y;
		if (xRightBottom.x < m_pxNodes[i].x)
			xRightBottom.x = m_pxNodes[i].x;
		if (xRightBottom.y < m_pxNodes[i].y)
			xRightBottom.y = m_pxNodes[i].y;
	}
	Rect_<float> rect;
	rect.x = topLeft.x;
	rect.y = topLeft.y;
	rect.width = xRightBottom.x - topLeft.x;
	rect.height = xRightBottom.y - topLeft.y;
	return rect;
}