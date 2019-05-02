#include "fx/FxBigEyes.h"
#include "fx/zPinch.h"
#include "MeshMaker.h"

FxBigEyes::FxBigEyes()
{
	auto facedistortionShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");

	auto faceDistortion = make_shared<OpenGLFaceDistortionModel>();
	faceDistortion->name = "big eye";

	auto faceDistortionRenderParams = make_shared<ObjectRenderParams>();
	faceDistortionRenderParams->shader = facedistortionShader;

	faceDistortion->objectRenderParams.fill({ faceDistortionRenderParams });

	faceDistortion->faceParams.fill({});

	faceDistortion->transformVersion = TransformVersion::V1;

	models.push_back(faceDistortion);
}

FxBigEyes::~FxBigEyes()
{
}

void FxBigEyes::transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model)
{
	ZPinch pinchMaker(frame.size());
	ImgMesh srcMesh = pinchMaker.getSrcMesh();
	ImgMesh workMesh = pinchMaker.getDstMesh();

	for (int iface = 0; iface < faces.size(); iface++)
	{
		transformMeshOne(frame, faces[iface], workMesh);
	}

	cv::Size sz = workMesh.size();
	// processing of edge part
	for (int iy = 0; iy < sz.height; iy++) {
		cv::Point2f pt = srcMesh.getPoint(0, iy);
		workMesh.setPoint(0, iy, pt.x, pt.y);
		pt = srcMesh.getPoint(sz.width - 1, iy);
		workMesh.setPoint(sz.width - 1, iy, pt.x, pt.y);
	}
	for (int ix = 0; ix < sz.width; ix++) {
		cv::Point2f pt = srcMesh.getPoint(ix, 0);
		workMesh.setPoint(ix, 0, pt.x, pt.y);
		pt = srcMesh.getPoint(ix, sz.height - 1);
		workMesh.setPoint(ix, sz.height - 1, pt.x, pt.y);
	}

	MeshMaker maker;
	cv::Rect viewRegion(0, 0, frame.cols, frame.rows);
	maker.init(frame, viewRegion, srcMesh);
	maker.setDstMesh(workMesh);

	*model = maker.getMesh();
}

static cv::Point2f crossPointTwoLines(const cv::Point2f& p1, const cv::Point2f& p2, const cv::Point2f& p3, const cv::Point2f& p4)
{
	float d = (p4.y - p3.y)*(p2.x - p1.x) - (p4.x - p3.x)*(p2.y - p1.y);
	if (d == 0)
		return cv::Point2f();

	float ua;
	ua = ((p4.x - p3.x)*(p1.y - p3.y) - (p4.y - p3.y)*(p1.x - p3.x)) / (float)d;
	cv::Point2f pt;
	pt.x = p1.x + ua*(p2.x - p1.x);
	pt.y = p1.y + ua*(p2.y - p1.y);
	return pt;
}

void FxBigEyes::transformMeshOne(cv::Mat frame, const FXModel& fxModel, ImgMesh& imgMesh)
{
	const std::array<float, 66 * 2>&  pts = fxModel.pts;


	// #1 = pts[2 * i], #2 = pts[2 * i + 1]

	ZPinch pinchMaker(frame.size(), imgMesh);

	if (fxModel.pupilLX < 0 || fxModel.pupilLY < 0)
	{
		imgMesh = pinchMaker.getDstMesh();
		return;
	}

	ImagePoints points(const_cast<std::array<float, 66 * 2>&>(pts));
	cv::Point2f leye = cv::Point2f(fxModel.pupilLX, fxModel.pupilLY);
	cv::Point2f reye = cv::Point2f(fxModel.pupilRX, fxModel.pupilRY);
	float leye_width = points.at(40).x - points.at(37).x;
	float reye_width = points.at(46).x - points.at(43).x;
	cv::Point2f center = PointCalcUtil::centerOf2Points(leye, reye);

	cv::Point2f chinpt = points.at(9);
	double rdist = PointCalcUtil::distanceBetween(chinpt, center);

	// condition near image boundary
	{
		float r = 0.55f;
		cv::Point2f d = leye - reye;
		cv::Point2f pt1, cross, vfrom, vto;

		pt1 = points.at(5);
		cross = crossPointTwoLines(pt1, pt1 + cv::Point2f(-d.y, d.x), leye, reye);
		vfrom = (cross - pt1*r)*(1.f / (1 - r));
		if (vfrom.y < 0) {
			imgMesh = pinchMaker.getDstMesh();
			return;
		}

		if (chinpt.y > frame.rows) {
			imgMesh = pinchMaker.getDstMesh();
			return;
		}
	}

	cv::Mat drawimage = frame.clone();

	// enlarge eye
	for (int i = 0; i < 2; i++)
	{
		pinchMaker.doMagnifyWarpEllipse(leye, leye_width*2.0f, 0.3f, 0.4f);
	}

	for (int i = 0; i < 2; i++)
	{
		pinchMaker.doMagnifyWarpEllipse(reye, reye_width*2.0f, 0.3f, 0.4f);
	}




	// make small top part of head
	//Xperimental -- Vladislav Khorev vladislav.khorev@fishrungames.com
	/*
	if (true) {
		for (int k = 0; k < 3; k++)
		{
			float extend_parameter = 1.3f;
			float reduce_parameter = 0.5f;
			{
				float brush_size = rdist * 0.4f;
				float brush_pressure = 0.15f;
				cv::Point2f vfrom = center * 2 - PointCalcUtil::interPoint(points.at(17), center, extend_parameter);

				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);

				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = center * 2 - PointCalcUtil::interPoint(points.at(1), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
			{
				float brush_size = rdist * 0.4f;
				float brush_pressure = 0.15f;
				cv::Point2f vfrom = center * 2 - PointCalcUtil::interPoint(points.at(15), center, extend_parameter);

				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);

				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = center * 2 - PointCalcUtil::interPoint(points.at(3), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
			{
				float brush_size = rdist * 0.4f;
				float brush_pressure = 0.15f;
				cv::Point2f vfrom = center * 2 - PointCalcUtil::interPoint(points.at(13), center, extend_parameter);
				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = center * 2 - PointCalcUtil::interPoint(points.at(5), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
			{
				float brush_size = rdist * 0.4f;
				float brush_pressure = 0.15f;
				cv::Point2f vfrom = center * 2 - PointCalcUtil::interPoint(points.at(11), center, extend_parameter);
				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);

				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = center * 2 - PointCalcUtil::interPoint(points.at(7), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
		}
		for (int k = 0; k < 1; k++)
		{
			const float extend_parameter = 1.2f;
			const float reduce_parameter = 0.6f;
			float brush_size = rdist * 0.2f;
			float brush_pressure = 0.15f;
			{
				cv::Point2f vfrom = PointCalcUtil::interPoint(points.at(3), center, extend_parameter);
				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = PointCalcUtil::interPoint(points.at(15), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
			{
				cv::Point2f vfrom = PointCalcUtil::interPoint(points.at(2), center, extend_parameter);
				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = PointCalcUtil::interPoint(points.at(16), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
			{
				cv::Point2f vfrom = PointCalcUtil::interPoint(points.at(1), center, extend_parameter);
				cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				vfrom = PointCalcUtil::interPoint(points.at(17), center, extend_parameter);
				vto = PointCalcUtil::interPoint(vfrom, center, reduce_parameter);
				pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
				cv::line(drawimage, vfrom, vto, cv::Scalar(255, 0, 0), 2);
			}
		}
	}
	*/

	// make large below nose.

	//Xperimental -- Vladislav Khorev vladislav.khorev@fishrungames.com


	center = points.at(34);

	//Xperimental -- Vladislav Khorev vladislav.khorev@fishrungames.com
	//const float extend_param = 1.3f;
	const float extend_param = 1.0f;


	const float extend_start_parameter = 0.8f;
	for (int k = 0; k < 3; k++)
	{
		
		{
			
			//Xperimental -- Vladislav Khorev vladislav.khorev@fishrungames.com
			//float brush_size = rdist * 0.4f;
			float brush_size = rdist * 0.3f;
			float brush_pressure = 0.15f;
			cv::Point2f vto = PointCalcUtil::interPoint(points.at(4), center, extend_param);
			cv::Point2f vfrom = PointCalcUtil::interPoint(vto, center, extend_start_parameter);
			pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
			cv::line(drawimage, vfrom, vto, cv::Scalar(0, 255, 0), 2);
			vto = PointCalcUtil::interPoint(points.at(14), center, extend_param);
			vfrom = PointCalcUtil::interPoint(vto, center, extend_start_parameter);
			pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
			cv::line(drawimage, vfrom, vto, cv::Scalar(0, 255, 0), 2);
		}
		
	}

	// enlarge mouth
	cv::Point2f leftMouthCorner = points.at(49);
	cv::Point2f rightMouthCorner = points.at(55);
	cv::Point2f mouthCenter = PointCalcUtil::interPoint(leftMouthCorner, rightMouthCorner, 0.5f);
	// enlarge mouth
	// warping left mouth corner

	const float mouth_size = PointCalcUtil::distanceBetween(leftMouthCorner, rightMouthCorner);
	
	
	pinchMaker.doMagnifyWarpEllipse(mouthCenter, mouth_size * 1.5f, 1.0f, 0.5f);
	
	// make nose part small

	const float arw[2] = { 1.0f, 0.5f };
	for (int k = 0; k < 2; k++) {
		for (int i = 32; i <= 36; i++)
		{
			if (i == 34)
				continue;
			cv::Point2f vfrom = points.at(i);
			cv::Point2f vto = points.at(30);
			if (i == 32) {
				cv::Point2f vdiff = points.at(32) - points.at(33);
				vfrom = vfrom + vdiff;
			}
			else if (i == 36)
			{
				cv::Point2f vdiff = points.at(36) - points.at(35);
				vfrom = vfrom + vdiff;
			}
			else {
				vfrom = PointCalcUtil::interPoint(vfrom, vto, 0.7f);
				continue;
			}
			vfrom = PointCalcUtil::interPoint(vfrom, vto, arw[k]);
			float nose_size = PointCalcUtil::distanceBetween(points.at(32), points.at(36));
			float brush_size = nose_size * 0.5f;
			pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.15f);
			cv::line(drawimage, vfrom, vto, cv::Scalar(0, 255, 0), 2);
		}
	}
	{
		for (int i = 32; i <= 36; i++)
		{
			if (i == 34)
				continue;
			cv::Point2f vfrom = points.at(i);
			//cv::Point2f vto = points.at(30);
			cv::Point2f nose_center = (points.at(32) + points.at(36))*0.5f;
			if (i == 32) {
				//cv::Point2f vdiff = points.at(32) - points.at(33);
				//vfrom = vfrom + vdiff;
			}
			else if (i == 36)
			{
				//cv::Point2f vdiff = points.at(36) - points.at(35);
				//vfrom = vfrom + vdiff;
			}
			else {
				continue;
			}
			//vfrom = PointCalcUtil::interPoint(vfrom, vto, arw[k]);
			float nose_size = PointCalcUtil::distanceBetween(points.at(32), points.at(36));
			float brush_size = nose_size * 0.5f;
			pinchMaker.doForwardWarp(vfrom, nose_center, brush_size, 0.15f);
			cv::line(drawimage, vfrom, nose_center, cv::Scalar(0, 255, 0), 2);
		}
	}

	
	
	imgMesh = pinchMaker.getDstMesh();
}
