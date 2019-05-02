#include "fx/FxFacePinch.h"
#include "fx/zPinch.h"
#include "MeshMaker.h"

FxFacePinch::FxFacePinch()
{
	auto facedistortionShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoColorTextureVertexShader.txt", "./assets/shaders/fragment/spriteFragmentShader.txt");

	auto faceDistortion = make_shared<OpenGLFaceDistortionModel>();
	faceDistortion->name = "FacePinch";

	auto faceDistortionRenderParams = make_shared<ObjectRenderParams>();
	faceDistortionRenderParams->shader = facedistortionShader;

	faceDistortion->objectRenderParams.fill({ faceDistortionRenderParams });

	faceDistortion->faceParams.fill({});

	models.push_back(faceDistortion);
}

FxFacePinch::~FxFacePinch()
{
}

void FxFacePinch::transformMesh(cv::Mat frame, std::vector<FXModel>& faces, OpenGLMesh *model)
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

void FxFacePinch::transformMeshOne(cv::Mat frame, const FXModel& fxModel, ImgMesh& imgMesh)
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


	// warping nose part
	{
		cv::Point2f ctrl_pt = points.at(31);
		cv::Point2f vfrom = ctrl_pt;// PointCalcUtil::interPoint(ctrl_pt, center, 0.5f);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		float brush_size = rdist / 3.f;
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.15f);
	}

	for (int i = 0; i < 3; i++)
		pinchMaker.doMagnifyWarpHeight(leye, leye_width*2.0f, 0.3f);
	pinchMaker.doMagnifyWarpHeight2(leye, leye_width*3.0f, 0.15f);
	for (int i = 0; i < 3; i++)
		pinchMaker.doMagnifyWarpHeight(reye, reye_width*2.0f, 0.3f);
	pinchMaker.doMagnifyWarpHeight2(reye, reye_width*3.0f, 0.15f);

	{
		float brush_size = rdist * 0.4f;
		float brush_pressure = 0.15f;
		cv::Point2f vfrom = points.at(3);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
		vfrom = points.at(15);
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
	}
	{
		float brush_size = rdist * 0.4f;
		float brush_pressure = 0.15f;
		cv::Point2f vfrom = points.at(5);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
		vfrom = points.at(13);
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
	}
	{
		float brush_size = rdist * 0.4f;
		float brush_pressure = 0.15f;
		cv::Point2f vfrom = points.at(7);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
		vfrom = points.at(11);
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
	}

	{
		int idx = 9;
		float brush_size = rdist*1.f;
		cv::Point2f vfrom = points.at(idx);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.3f);
		vfrom = PointCalcUtil::interPoint(vfrom, center, 0.35f);
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		//	pinchMaker.doForwardWarp(ctrl_pt, interpt, brush_size, 0.3f);
	}
	{
		cv::Point2f ctrl_pt = points.at(9);
		cv::Point2f vfrom = PointCalcUtil::interPoint(ctrl_pt, center, 0.5f);
		cv::Point2f vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		float brush_size = rdist * 0.5f;
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.15f);
	}

	// top forward warping
	if (true) {
		float brush_size = rdist*0.4f;
		float brush_pressure = 0.15f;
		float r = 0.55f;
		cv::Point2f d = leye - reye;
		cv::Point2f pt1, cross, vfrom, vto;

		pt1 = points.at(5);
		cross = crossPointTwoLines(pt1, pt1 + cv::Point2f(-d.y, d.x), leye, reye);
		vfrom = (cross - pt1*r)*(1.f / (1 - r));
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);

		pt1 = points.at(13);
		cross = crossPointTwoLines(pt1, pt1 + cv::Point2f(-d.y, d.x), leye, reye);
		vfrom = (cross - pt1*r)*(1.f / (1 - r));
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);

		r = 0.5f;
		pt1 = points.at(7);
		cross = crossPointTwoLines(pt1, pt1 + cv::Point2f(-d.y, d.x), leye, reye);
		vfrom = (cross - pt1*r)*(1.f / (1 - r));
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);

		pt1 = points.at(11);
		cross = crossPointTwoLines(pt1, pt1 + cv::Point2f(-d.y, d.x), leye, reye);
		vfrom = (cross - pt1*r)*(1.f / (1 - r));
		vto = PointCalcUtil::interPoint(vfrom, center, 0.5f);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, brush_pressure);
	}
	{
		cv::Point2f vtop;
		vtop.x = center.x * 2 - chinpt.x;
		vtop.y = center.y * 2 - chinpt.y;
		cv::Point2f vfrom, vto;
		vfrom = vtop;
		float r = 0.5f;
		vto = PointCalcUtil::interPoint(vtop, center, r);
		float brush_size = rdist*0.4f;
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.15f);
	}

	cv::Point2f leftMouthCorner = points.at(49);
	cv::Point2f rightMouthCorner = points.at(55);
	cv::Point2f mouthCenter = PointCalcUtil::interPoint(leftMouthCorner, rightMouthCorner, 0.5f);

	// warping left mouth corner
	for (int k = 0; k < 2; k++)
	{
		cv::Point2f vfrom, vto;
		vfrom = leftMouthCorner;
		vto = PointCalcUtil::interPoint(vfrom, mouthCenter, 0.66f);
		float brush_size = PointCalcUtil::distanceBetween(leftMouthCorner, rightMouthCorner);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.3f);
	}

	// warping right mouth corner
	for (int k = 0; k < 2; k++)
	{
		cv::Point2f vfrom, vto;
		vfrom = rightMouthCorner;
		vto = PointCalcUtil::interPoint(vfrom, mouthCenter, 0.66f);
		float brush_size = PointCalcUtil::distanceBetween(leftMouthCorner, rightMouthCorner);
		pinchMaker.doForwardWarp(vfrom, vto, brush_size, 0.3f);
	}


	imgMesh = pinchMaker.getDstMesh();
}
