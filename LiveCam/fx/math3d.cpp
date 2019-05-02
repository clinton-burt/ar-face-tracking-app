#include "math3d.h"
#include "utils/PointCalcUtil.h"

using namespace cv;

RMat3D& RMat3D::operator= (const RMat3D& argUpdateData)
{
	m_rA1 = argUpdateData.m_rA1;
	m_rA2 = argUpdateData.m_rA2;
	m_rA3 = argUpdateData.m_rA3;
	m_rA4 = argUpdateData.m_rA4;
	m_rA5 = argUpdateData.m_rA5;
	m_rA6 = argUpdateData.m_rA6;
	m_rA7 = argUpdateData.m_rA7;
	m_rA8 = argUpdateData.m_rA8;
	m_rA9 = argUpdateData.m_rA9;
	return *this;
}

RMat3D RMat3D::operator* (float rMul) const
{
	RMat3D xRetMat;
	
	xRetMat.m_rA1 = m_rA1 * rMul;
	xRetMat.m_rA2 = m_rA2 * rMul;
	xRetMat.m_rA3 = m_rA3 * rMul;
	xRetMat.m_rA4 = m_rA4 * rMul;
	xRetMat.m_rA5 = m_rA5 * rMul;
	xRetMat.m_rA6 = m_rA6 * rMul;
	xRetMat.m_rA7 = m_rA7 * rMul;
	xRetMat.m_rA8 = m_rA8 * rMul;
	xRetMat.m_rA9 = m_rA9 * rMul;
	return xRetMat;
}

void RMat3D::SetRotateMat (float r1, float r2, float r3, float r4, float r5, float r6, float r7, float r8, float r9)
{
	m_rA1 = r1;
	m_rA2 = r2;
	m_rA3 = r3;
	m_rA4 = r4;
	m_rA5 = r5;
	m_rA6 = r6;
	m_rA7 = r7;
	m_rA8 = r8;
	m_rA9 = r9;
}

float RMat3D::GetDeterminant() const
{
	float rDeterminant = m_rA5 * m_rA1 * m_rA9 +
						m_rA6 * m_rA2 * m_rA7 +
						m_rA4 * m_rA3 * m_rA8 -
						m_rA6 * m_rA1 * m_rA8 -
						m_rA2 * m_rA4 * m_rA9 -
						m_rA3 * m_rA5 * m_rA7;
	return rDeterminant;
}

void RMat3D::ReverseMat()
{
	float rDet;
	rDet = GetDeterminant();
	if (rDet != 0)
	{
		float r1, r2, r3, r4, r5, r6, r7, r8, r9;
		rDet = 1 / rDet;
		r1 = (m_rA9 * m_rA5 - m_rA8 * m_rA6) * rDet;
		r2 = (m_rA8 * m_rA3 - m_rA2 * m_rA9) * rDet;
		r3 = (m_rA6 * m_rA2 - m_rA5 * m_rA3) * rDet;
		r4 = (m_rA6 * m_rA7 - m_rA4 * m_rA9) * rDet;
		r5 = (m_rA9 * m_rA1 - m_rA7 * m_rA3) * rDet;
		r6 = (m_rA4 * m_rA3 - m_rA6 * m_rA1) * rDet;
		r7 = (m_rA8 * m_rA4 - m_rA7 * m_rA5) * rDet;
		r8 = (m_rA2 * m_rA7 - m_rA8 * m_rA1) * rDet;
		r9 = (m_rA5 * m_rA1 - m_rA4 * m_rA2) * rDet;
		m_rA1 = r1;
		m_rA2 = r2;
		m_rA3 = r3;
		m_rA4 = r4;
		m_rA5 = r5;
		m_rA6 = r6;
		m_rA7 = r7;
		m_rA8 = r8;
		m_rA9 = r9;
		
	}
}
void RMat3D::Update( const Point3f& xPoint )
{
	RMat3D xRotateMat1, xRotateMat2;
	SetRotateMat(1, 0, 0, 0, 1, 0, 0, 0, 1);
	
	float rDist = PointCalcUtil::norm(xPoint);
	if( fabs( rDist ) > 9.999999682655225e-21 )
	{
		xRotateMat1.SetRotateMat(0, -xPoint.z, xPoint.y, xPoint.z, 0, -xPoint.x, -xPoint.y, xPoint.x, 0);
		xRotateMat2 = xRotateMat1 * (sinf(rDist) / rDist);
		xRotateMat2 += xRotateMat1 * xRotateMat1 * ((1 - cosf(rDist)) / (rDist * rDist));
		*this += xRotateMat2;
	}
}

RMat3D RMat3D::operator* (const RMat3D& xRotateMat) const
{
	RMat3D xRetMat;
	xRetMat.m_rA1 = xRotateMat.m_rA1 * m_rA1 + xRotateMat.m_rA2 * m_rA4 + xRotateMat.m_rA3 * m_rA7;
	xRetMat.m_rA2 = xRotateMat.m_rA1 * m_rA2 + xRotateMat.m_rA2 * m_rA5 + xRotateMat.m_rA3 * m_rA8;
	xRetMat.m_rA3 = xRotateMat.m_rA1 * m_rA3 + xRotateMat.m_rA2 * m_rA6 + xRotateMat.m_rA3 * m_rA9;
	xRetMat.m_rA4 = xRotateMat.m_rA4 * m_rA1 + xRotateMat.m_rA5 * m_rA4 + xRotateMat.m_rA6 * m_rA7;
	xRetMat.m_rA5 = xRotateMat.m_rA4 * m_rA2 + xRotateMat.m_rA5 * m_rA5 + xRotateMat.m_rA6 * m_rA8;
	xRetMat.m_rA6 = xRotateMat.m_rA4 * m_rA3 + xRotateMat.m_rA5 * m_rA6 + xRotateMat.m_rA6 * m_rA9;
	xRetMat.m_rA7 = xRotateMat.m_rA7 * m_rA1 + xRotateMat.m_rA8 * m_rA4 + xRotateMat.m_rA9 * m_rA7;
	xRetMat.m_rA8 = xRotateMat.m_rA7 * m_rA2 + xRotateMat.m_rA8 * m_rA5 + xRotateMat.m_rA9 * m_rA8;
	xRetMat.m_rA9 = xRotateMat.m_rA7 * m_rA3 + xRotateMat.m_rA8 * m_rA6 + xRotateMat.m_rA9 * m_rA9;
	return xRetMat;
}

RMat3D& RMat3D::operator*= (const RMat3D& RotateMat)
{
	float r1, r2, r3, r4, r5, r6, r7, r8, r9;
	r1 = m_rA1 * RotateMat.m_rA1 + m_rA2 * RotateMat.m_rA4 + m_rA3 * RotateMat.m_rA7;
	r2 = m_rA2 * RotateMat.m_rA5 + m_rA1 * RotateMat.m_rA2 + m_rA3 * RotateMat.m_rA8;
	r3 = m_rA1 * RotateMat.m_rA3 + m_rA2 * RotateMat.m_rA6 + m_rA3 * RotateMat.m_rA9;
	r4 = m_rA5 * RotateMat.m_rA4 + m_rA4 * RotateMat.m_rA1 + m_rA6 * RotateMat.m_rA7;
	r5 = m_rA5 * RotateMat.m_rA5 + m_rA4 * RotateMat.m_rA2 + m_rA6 * RotateMat.m_rA8;
	r6 = m_rA5 * RotateMat.m_rA6 + m_rA4 * RotateMat.m_rA3 + m_rA6 * RotateMat.m_rA9;
	r7 = m_rA7 * RotateMat.m_rA1 + m_rA8 * RotateMat.m_rA4 + m_rA9 * RotateMat.m_rA7;
	r8 = m_rA8 * RotateMat.m_rA5 + m_rA7 * RotateMat.m_rA2 + m_rA9 * RotateMat.m_rA8;
	r9 = m_rA7 * RotateMat.m_rA3 + m_rA8 * RotateMat.m_rA6 + m_rA9 * RotateMat.m_rA9;
	m_rA1 = r1;
	m_rA2 = r2;
	m_rA3 = r3;
	m_rA4 = r4;
	m_rA5 = r5;
	m_rA6 = r6;
	m_rA7 = r7;
	m_rA8 = r8;
	m_rA9 = r9;
	return *this;
}

Point3f RMat3D::operator* (const Point3f& pt) const
{
	Point3f xRetPoint;
	xRetPoint.x = m_rA1 * pt.x + m_rA2 * pt.y + m_rA3 * pt.z;
	xRetPoint.y = m_rA4 * pt.x + m_rA5 * pt.y + m_rA6 * pt.z;
	xRetPoint.z = m_rA7 * pt.x + m_rA8 * pt.y + m_rA9 * pt.z;
	return xRetPoint;
}

RMat3D& RMat3D::operator+= (const RMat3D& xRotateMat)
{
	m_rA1 += xRotateMat.m_rA1;
	m_rA2 += xRotateMat.m_rA2;
	m_rA3 += xRotateMat.m_rA3;
	m_rA4 += xRotateMat.m_rA4;
	m_rA5 += xRotateMat.m_rA5;
	m_rA6 += xRotateMat.m_rA6;
	m_rA7 += xRotateMat.m_rA7;
	m_rA8 += xRotateMat.m_rA8;
	m_rA9 += xRotateMat.m_rA9;
	return *this;
}

Linear3D::Linear3D(const Linear3D& t)
{
    m_xRotateMat=t.m_xRotateMat;
    m_xTranPoint=t.m_xTranPoint;
}

Linear3D& Linear3D::operator= (const Linear3D& argUpdateData)
{
	m_xRotateMat = argUpdateData.m_xRotateMat;
	m_xTranPoint = argUpdateData.m_xTranPoint;
	return *this;
}

void Linear3D::transform(Point3f& p) const
{
	p = m_xRotateMat * p + m_xTranPoint;
//	p = m_xRotateMat * (p - m_xTranPoint) + m_xTranPoint;
}
void Linear3D::reverse()
{
	m_xRotateMat.ReverseMat();
}

Linear3D Linear3D::operator* (float rValue) const
{
	Linear3D xTransform;
	xTransform.m_xRotateMat = m_xRotateMat * rValue;
	xTransform.m_xTranPoint = m_xTranPoint * rValue;
	return xTransform;
}