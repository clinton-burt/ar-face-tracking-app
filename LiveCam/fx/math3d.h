#pragma once

#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>

class RMat3D
{
public:
	float m_rA1;
	float m_rA2;
	float m_rA3;
	float m_rA4;
	float m_rA5;
	float m_rA6;
	float m_rA7;
	float m_rA8;
	float m_rA9;
	
public:
	RMat3D() {m_rA1 = 0; m_rA2 = 0; m_rA3 = 0; m_rA4 = 0; m_rA5 = 0; m_rA6 = 0; m_rA7 = 0; m_rA8 = 0; m_rA9 = 0;};
	virtual ~RMat3D() {};
	RMat3D& operator= (const RMat3D&);
	RMat3D operator* (float rMul) const;
	RMat3D operator* (const RMat3D&) const;
	RMat3D& operator+= (const RMat3D&);
	RMat3D& operator*= (const RMat3D&);
	cv::Point3f operator* (const cv::Point3f& pt) const;
	void ReverseMat();
	float GetDeterminant() const;
	void SetRotateMat (float,float,float,float,float,float,float,float,float);
	void Update(const cv::Point3f& xPoint);
};

class Linear3D
{
public:
	RMat3D	m_xRotateMat;
	cv::Point3f		m_xTranPoint;
	
public:
	Linear3D() {};
    Linear3D(const Linear3D& t);
	virtual ~Linear3D() {};
	Linear3D& operator= (const Linear3D& t);
	Linear3D operator* (float rValue) const;
	void transform(cv::Point3f& p) const;
	void reverse();
};