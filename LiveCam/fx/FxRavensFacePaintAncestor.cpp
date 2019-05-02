#include "fx/FxRavensFacePaintAncestor.h"

static const cv::Point2f topPointTexCoord = { 0.5f, 0.2f }; // 500, 200

static const std::array<cv::Point2f, 66 + 7> texCoordInitTable = {
	cv::Point2f{ 100, 348 }, //100, 348 0
	cv::Point2f{ 110, 439 }, //110, 439 1
	cv::Point2f{ 131, 543 }, //131, 543 2
	cv::Point2f{ 142, 642 }, //142, 642 3
	cv::Point2f{ 196, 738 }, //196, 738 4
	cv::Point2f{ 243, 822 }, //243, 822 5
	cv::Point2f{ 312, 890 }, //312, 890 6
	cv::Point2f{ 401, 948 }, //401, 948 7
	cv::Point2f{ 500, 942 }, //500, 942 8
	cv::Point2f{ 603, 946 }, //603, 946 9
	cv::Point2f{ 688, 890 }, //688, 890 10
	cv::Point2f{ 758, 822 }, //758, 822 11
	cv::Point2f{ 814, 738 }, //814, 738 12
	cv::Point2f{ 847, 642 }, //847, 642 13
	cv::Point2f{ 866, 543 }, //866, 543 14
	cv::Point2f{ 886, 447 }, //886, 447 15 !!
	cv::Point2f{ 895, 348 }, //895, 348 16
	cv::Point2f{ 174, 254 }, //174, 254 17
	cv::Point2f{ 236, 222 }, //236, 222 18
	cv::Point2f{ 303, 217 }, //19
	cv::Point2f{ 373, 239 }, //20
	cv::Point2f{ 431, 273 }, //21
	cv::Point2f{ 571, 273 }, //22
	cv::Point2f{ 631, 241 }, //23
	cv::Point2f{ 701, 211 }, //24
	cv::Point2f{ 766, 220 }, //766, 220 25 !!!
	cv::Point2f{ 828, 246 }, //828, 246 26
	cv::Point2f{ 500, 366 }, //500, 366 27
	cv::Point2f{ 500, 430 }, //500, 430 28
	cv::Point2f{ 500, 494 }, //500, 494 29
	cv::Point2f{ 500, 558 }, //500, 558, 30
	cv::Point2f{ 439, 609 }, //31
	cv::Point2f{ 471, 619 }, //32
	cv::Point2f{ 500, 623 }, //500, 624 33
	cv::Point2f{ 533, 623 }, //34
	cv::Point2f{ 563, 609 }, //35
	cv::Point2f{ 210, 348 }, //210, 348 36
	cv::Point2f{ 272, 322 }, //272, 322 37
	cv::Point2f{ 342, 326 }, //342, 326 38
	cv::Point2f{ 406, 370 }, //406, 370 39
	cv::Point2f{ 334, 390 }, //334, 390 40
	cv::Point2f{ 266, 390 }, //266, 390 41
	cv::Point2f{ 596, 372 }, //596, 372 42
	cv::Point2f{ 660, 328 }, //660, 328 43
	cv::Point2f{ 730, 324 }, //730, 324 44
	cv::Point2f{ 792, 350 }, //792, 350 45
	cv::Point2f{ 736, 390 }, //736, 390 46
	cv::Point2f{ 668, 392 }, //668, 392 47
	cv::Point2f{ 354, 734 }, //354, 734 48
	cv::Point2f{ 398, 706 }, //398, 706 49
	cv::Point2f{ 446, 688 }, //446, 688 50
	cv::Point2f{ 500, 686 }, //500, 686 51
	cv::Point2f{ 556, 688 }, //556, 688 52
	cv::Point2f{ 602, 706 }, //602, 706 53
	cv::Point2f{ 648, 734 }, //648, 734 54
	cv::Point2f{ 602, 782 }, //602, 782 55
	cv::Point2f{ 556, 806 }, //556, 806 56
	cv::Point2f{ 500, 810 }, //500, 810 57
	cv::Point2f{ 446, 806 }, //446, 806 58
	cv::Point2f{ 398, 781 }, //398, 781 59
	cv::Point2f{ 0, 0 },
	cv::Point2f{ 0, 0 },
	cv::Point2f{ 0, 0 },
	cv::Point2f{ 0, 0 },
	cv::Point2f{ 0, 0 },
	cv::Point2f{ 0, 0 }, // 65 - last ULS Tracker point
	cv::Point2f{ 171, 215}, // above 17
	cv::Point2f{ 237, 155 }, // above 18
	cv::Point2f{ 375, 127 }, // above 20
	cv::Point2f{ 490, 95 }, // above TOP
	cv::Point2f{ 623, 127 }, // above 23
	cv::Point2f{ 765, 155 }, // above 25
	cv::Point2f{ 829, 215 } // above 26
};

const static cv::Point icon_position(296, 538);

//================================================================================
//================================================================================
//================================================================================

RavensHalfFaceParams::RavensHalfFaceParams() { }

RavensHalfFaceParams::RavensHalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
	Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
	std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth)
	: leftAmbient(leftAmbient)
	, rightAmbient(rightAmbient)
	, lineAmbient(lineAmbient)
	, leftDiffuse(leftDiffuse)
	, rightDiffuse(rightDiffuse)
	, lineDiffuse(lineDiffuse)
	, leftTexture(leftTexture)
	, rightTexture(rightTexture)
	, lineTexture(lineTexture)
	, lineWidth(lineWidth)
{ }

RavensHalfFaceParams::RavensHalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
	Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
	std::string _iconImagePath, std::string _facemaskTexturePath)
	: leftAmbient(leftAmbient)
	, rightAmbient(rightAmbient)
	, lineAmbient(lineAmbient)
	, leftDiffuse(leftDiffuse)
	, rightDiffuse(rightDiffuse)
	, lineDiffuse(lineDiffuse)
	, lineWidth(0)
	, iconImagePath(_iconImagePath)
	, facemaskTexturePath(_facemaskTexturePath)
{}


void RavensHalfFaceParams::applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately)
{
	auto tree = suit.get_child_optional("leftAmbient");
	leftAmbient = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	tree = suit.get_child_optional("rightAmbient");
	rightAmbient = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	tree = suit.get_child_optional("lineAmbient");
	lineAmbient = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	tree = suit.get_child_optional("leftDiffuse");
	leftDiffuse = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	tree = suit.get_child_optional("rightDiffuse");
	rightDiffuse = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	tree = suit.get_child_optional("lineDiffuse");
	lineDiffuse = tree ? JSONVectorReader::readVector4f(tree.get()) : Eigen::Vector4f(0, 0, 0, 0);

	leftTexture = suit.get<std::string>("leftTexture", "");
	rightTexture = suit.get<std::string>("rightTexture", "");
	lineTexture = suit.get<std::string>("lineTexture", "");

	lineWidth = suit.get<float>("lineWidth", 0);

	if (loadTexturesImmediately)
	{
		leftTexId = resourceManager.loadTexture(leftTexture).ID;
		rightTexId = resourceManager.loadTexture(rightTexture).ID;
		lineTexId = resourceManager.loadTexture(lineTexture).ID;
	}
}

static std::vector<cv::Point> makeNaturalContour(const std::vector<cv::Point>& points, double strength, std::vector<double>& offset_values)
{
	const int inter_count = offset_values.size();
	std::vector<cv::Point> naturalPoints;
	naturalPoints.push_back(points[0]);
	for (int i = 1; i < points.size(); i++)
	{
		cv::Point pt1 = points[i - 1];
		cv::Point pt2 = points[i];
		double dist = PointCalcUtil::distanceBetween(pt1.x, pt1.y, pt2.x, pt2.y);
		double step = dist / (inter_count + 2);
		double dx = (double)(pt2.x - pt1.x) / dist;
		double dy = (double)(pt2.y - pt1.y) / dist;
		double nx = -dy;
		double ny = dx;
		double var_offset = strength*dist;
		for (int k = 0; k < inter_count; k++)
		{
			double dist_elem = step*(k + 1);
			double dx1 = dist_elem*dx;
			double dy1 = dist_elem*dy;
			cv::Point newPt(pt1.x + dx1 + nx*offset_values[k] * var_offset, pt1.y + dy1 + ny*offset_values[k] * var_offset);
			naturalPoints.push_back(newPt);
		}
		naturalPoints.push_back(pt2);
	}
	return naturalPoints;
}

static std::vector<cv::Point> makeSmallNaturalContour(const std::vector<cv::Point>& points, double strength)
{
	const double arr[] = { 0.8, 0.2, 0.4, -0.6, -0.3, 0.2, -0.5, -0.2 };
	std::vector<double> vec(arr, arr + sizeof(arr) / sizeof(arr[0]));
	return makeNaturalContour(points, strength, vec);
}

static cv::Mat gtexture;

static cv::Rect getBoundRegion(cv::Mat image)
{
	cv::Rect region(image.cols, image.rows, 0, 0);
	int idx = 0;
	for (int y = 0; y < image.rows; y++)
	{
		for (int x = 0; x < image.cols; x++, idx++)
		{
			if (image.data[idx * 4 + 3] != 0)
			{
				region.x = std::min(region.x, x);
				region.y = std::min(region.y, y);
				region.x = std::max(region.x, x);
				region.y = std::max(region.y, y);
			}
		}
	}
	region.width = region.width - region.x + 1;
	region.height = region.height - region.y + 1;
	return region;
}

static cv::Point calcCenter(std::vector<cv::Point>& points)
{
	cv::Point p;
	int count = 0;
	for (int i = 0; i < points.size(); i++, count++)
	{
		p = p + points[i];
	}
	return cv::Point(p.x / count, p.y / count);
}

static float distBetweenPoints(cv::Point p1, cv::Point p2)
{
	return sqrtf((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

bool RavensHalfFaceParams::makeTexture()
{
	cv::Mat ravensTexture = cv::imread(facemaskTexturePath, cv::IMREAD_GRAYSCALE);
	if (ravensTexture.cols == 0 || ravensTexture.rows == 0)
		return false;

	cv::resize(ravensTexture, ravensTexture, cv::Size(1000, 1000), 0, 0, cv::INTER_LINEAR);
	{
		uchar minr = 255;
		uchar maxr = 0;
		int size = ravensTexture.rows*ravensTexture.cols;
		for (int i = 0; i < size; i++)
		{
			uchar vcolor = ravensTexture.data[i];
			minr = std::min(minr, vcolor);
			maxr = std::max(maxr, vcolor);
		}
		int thresh = 200;
		for (int i = 0; i < size; i++)
		{
			uchar vcolor = ravensTexture.data[i];
			int new_value = 255 - (vcolor - minr) * 255 / (maxr - minr);
			if (new_value > thresh)
				new_value = 255;
			else
				new_value += (255 - thresh);
			ravensTexture.data[i] = new_value;
		}
	}

	cv::Mat iconImage = cv::imread(iconImagePath, cv::IMREAD_UNCHANGED);
	if (iconImage.channels() < 4)
		return false;
	cv::Rect iconRegion = getBoundRegion(iconImage);

	const int rows = 1000, cols = 1000;

	cv::Mat mtexture = cv::Mat::zeros(rows, cols, CV_8UC4);

	std::vector<cv::Point> mask_out_contour;
	const int facemask_idx[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,72, 71, 70, 69, 68, 67, 66, 0
	};
	for (int i = 0; i < sizeof(facemask_idx) / sizeof(facemask_idx[0]); i++) {
		mask_out_contour.push_back(texCoordInitTable[facemask_idx[i]]);
	}
	std::vector<cv::Point> left_eye_contour;
	for (int i = 36; i <= 41; i++)
		left_eye_contour.push_back(texCoordInitTable[i]);
	cv::Point lefteye = calcCenter(left_eye_contour);

	left_eye_contour.push_back(texCoordInitTable[36]);

	std::vector<cv::Point> right_eye_contour;
	for (int i = 42; i <= 47; i++)
		right_eye_contour.push_back(texCoordInitTable[i]);
	cv::Point righteye = calcCenter(right_eye_contour);

	right_eye_contour.push_back(texCoordInitTable[42]);

	std::vector<cv::Point> mouth_contour;
	for (int i = 48; i <= 59; i++)
		mouth_contour.push_back(texCoordInitTable[i]);
	mouth_contour.push_back(texCoordInitTable[48]);

	std::vector<cv::Point> center_contour;
	const int centerline_point_idx[] = { 69, 27, 28, 29, 30, 33, 51, 57, 8 };
	center_contour.push_back(cv::Point(500, 0));
	for (int i = 0; i < sizeof(centerline_point_idx) / sizeof(centerline_point_idx[0]); i++) {
		center_contour.push_back(texCoordInitTable[centerline_point_idx[i]]);
	}
	center_contour.push_back(cv::Point(500, 999));


	int icon_size = distBetweenPoints(lefteye, righteye) * 0.5f;
	cv::Size size_icon(icon_size*0.8, icon_size*0.8*iconImage.rows / iconImage.cols);

	cv::Mat resizedIcon;
	cv::resize(iconImage, resizedIcon, size_icon);
	assert(resizedIcon.channels() == 4);


	mask_out_contour = makeSmallNaturalContour(mask_out_contour, 0.1);
	mask_out_contour = makeSmallNaturalContour(mask_out_contour, 0.1);
	left_eye_contour = makeSmallNaturalContour(left_eye_contour, 0.1);
	left_eye_contour = makeSmallNaturalContour(left_eye_contour, 0.1);
	right_eye_contour = makeSmallNaturalContour(right_eye_contour, 0.1);
	right_eye_contour = makeSmallNaturalContour(right_eye_contour, 0.1);
	mouth_contour = makeSmallNaturalContour(mouth_contour, 0.1);
	mouth_contour = makeSmallNaturalContour(mouth_contour, 0.1);

	center_contour = makeSmallNaturalContour(center_contour, 0.05);
	center_contour = makeSmallNaturalContour(center_contour, 0.1);

	cv::Mat mask = cv::Mat::zeros(rows, cols, CV_8UC1);
	for (int i = 0; i < left_eye_contour.size() - 1; i++)
		cv::line(mask, left_eye_contour[i], left_eye_contour[i + 1], CV_RGB(1, 1, 1), 1);
	for (int i = 0; i < right_eye_contour.size() - 1; i++)
		cv::line(mask, right_eye_contour[i], right_eye_contour[i + 1], CV_RGB(1, 1, 1), 1);
	for (int i = 0; i < mouth_contour.size() - 1; i++)
		cv::line(mask, mouth_contour[i], mouth_contour[i + 1], CV_RGB(1, 1, 1), 1);
	for (int i = 0; i < mask_out_contour.size() - 1; i++)
		cv::line(mask, mask_out_contour[i], mask_out_contour[i + 1], CV_RGB(1, 1, 1), 1);

	cv::Mat leftmask = mask.clone();
	for (int i = 0; i < center_contour.size() - 1; i++)
		cv::line(leftmask, center_contour[i], center_contour[i + 1], CV_RGB(1, 1, 1), 1);

	cv::Point2f left_seed, right_seed;
	left_seed = (texCoordInitTable[0] + texCoordInitTable[2] + texCoordInitTable[31] + texCoordInitTable[28] + texCoordInitTable[40]) * 0.2f;

	cv::floodFill(mask, cv::Point(left_seed.x, left_seed.y), cv::Scalar(255, 255, 255, 255));
	cv::floodFill(leftmask, cv::Point(left_seed.x, left_seed.y), cv::Scalar(255, 255, 255, 255));
	cv::GaussianBlur(leftmask, leftmask, cv::Size(15, 15), 5., 5.);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(23, 23), cv::Point(11, 11));
	cv::erode(mask, mask, element);
	cv::erode(mask, mask, element);

	cv::GaussianBlur(mask, mask, cv::Size(51, 51), 10., 10.);

	int idx = 0;
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++, idx++)
		{
			mtexture.data[idx * 4 + 3] = mask.data[idx];
			int alpha = leftmask.data[idx];
			mtexture.data[idx * 4 + 2] = leftAmbient[0] * alpha + rightAmbient[0] * (255 - alpha);
			mtexture.data[idx * 4 + 1] = leftAmbient[1] * alpha + rightAmbient[1] * (255 - alpha);
			mtexture.data[idx * 4 + 0] = leftAmbient[2] * alpha + rightAmbient[2] * (255 - alpha);
		}
	}

	// overlay icon
	cv::Rect overlap_icon_region(icon_position.x - resizedIcon.cols / 2, icon_position.y - resizedIcon.rows / 2, resizedIcon.cols, resizedIcon.rows);
	idx = 0;
	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++, idx++)
		{
			cv::Vec4b org_color = mtexture.at<cv::Vec4b>(y, x);
			int new_alpha = ravensTexture.data[idx] * org_color.val[3] / 255;
			if (x >= overlap_icon_region.x && x < overlap_icon_region.x + overlap_icon_region.width &&
				y >= overlap_icon_region.y && y < overlap_icon_region.y + overlap_icon_region.height)
			{
				cv::Vec4b icon_color = resizedIcon.at<cv::Vec4b>(y - overlap_icon_region.y, x - overlap_icon_region.x);
				int alpha = icon_color.val[3] * 0.8;
				org_color.val[0] = (org_color.val[0] * (255 - alpha) + icon_color.val[0] * alpha) / 255;
				org_color.val[1] = (org_color.val[1] * (255 - alpha) + icon_color.val[1] * alpha) / 255;
				org_color.val[2] = (org_color.val[2] * (255 - alpha) + icon_color.val[2] * alpha) / 255;
				org_color.val[3] = new_alpha;//(new_alpha * (255 - alpha) + icon_color.val[3] * alpha) / 255;
				mtexture.at<cv::Vec4b>(y, x) = org_color;
			}
			else
			{
				org_color.val[3] = new_alpha;
				mtexture.at<cv::Vec4b>(y, x) = org_color;
			}
		}
	}
	gtexture = mtexture.clone();
	return true;
}

void RavensHalfFaceParams::load()
{
	if (!makeTexture())
	{
		leftTexId = resourceManager.loadTexture(leftTexture).ID;
		rightTexId = resourceManager.loadTexture(rightTexture).ID;
		lineTexId = resourceManager.loadTexture(lineTexture).ID;
	}
}

const std::vector<std::vector<std::vector<int>>> RavensHalfFacePaint::indexes =
{
	{
		{ 8, 57, 7, 58, 6, 59, 5, 48, 4 }, // left chin strip
		{ 41, 36, 1, 2, 3, 4, 48, 49, 31, 40 }, // left cheek fan
		{ 1, 0, 36, 17, 37, 18, 38, 20, 39, -1 }, // left brow strip, -1 is the TOP point
		{ 39,-1, 27, 28, 29, 30, 31, 40 }, // left nose fan, -1 is the TOP point
		{ 31, 30, 33, 51, 50, 49 } // left under nose fan
	},
	{
		{ 8, 57, 9, 56, 10, 55, 11, 54, 12 }, // right chin strip
		{ 46, 45, 15, 14, 13, 12, 54, 53, 35, 47 }, // right cheek fan
		{ 15, 16, 45, 26, 44, 25, 43, 23, 42, -1 }, // right brow strip
		{ 42, -1, 27, 28, 29, 30, 35, 47 }, // right nose fan
		{ 35, 30, 33, 51, 52, 53 } // right under nose fan
	}
};

static std::array<cv::Point2f, 66 + 7> texCoordTable;
void RavensHalfFacePaint::load()
{
	glGenBuffers(1, &leftVBO);
	glGenBuffers(1, &rightVBO);
	glGenBuffers(1, &lineVBO);

	glGenBuffers(1, &leftTexCoordVBO);
	glGenBuffers(1, &rightTexCoordVBO);
	glGenBuffers(1, &lineTexCoordVBO);

	for (size_t i = 0; i < 66 + 7; i++)
	{
		texCoordTable[i] = { texCoordInitTable[i].x / 1000.f, texCoordInitTable[i].y / 1000.f };
	}

	const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

	for (int i = 0; i < 2; ++i)
	{
		std::array<float, total * 2> texCoordBuffer;

		auto iter = texCoordBuffer.begin();

		for (auto &facePart : indexes[i])
		{
			for (auto index : facePart)
			{
				if (index > -1)
				{
					*iter++ = texCoordTable[index].x;
					*iter++ = texCoordTable[index].y;
				}
				else
				{
					*iter++ = topPointTexCoord.x;
					*iter++ = topPointTexCoord.y;
				}
			}
		}

		if (i == 0)
		{
			std::array<cv::Point2f, FOREHEAD_STRIP_COUNT> leftForehead =
			{
				texCoordTable[0], texCoordTable[66], texCoordTable[17], texCoordTable[67], texCoordTable[18],
				texCoordTable[68], texCoordTable[20], texCoordTable[69], topPointTexCoord
			};

			for (int i = 0; i < FOREHEAD_STRIP_COUNT; ++i)
			{
				*iter++ = leftForehead[i].x;
				*iter++ = leftForehead[i].y;
			}

			glBindBuffer(GL_ARRAY_BUFFER, leftTexCoordVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 2, texCoordBuffer.data(), GL_STATIC_DRAW);
		}
		else
		{
			std::array<cv::Point2f, FOREHEAD_STRIP_COUNT> rightForehead =
			{
				texCoordTable[16], texCoordTable[72], texCoordTable[26], texCoordTable[71], texCoordTable[25],
				texCoordTable[70], texCoordTable[23], texCoordTable[69], topPointTexCoord
			};

			for (int i = 0; i < FOREHEAD_STRIP_COUNT; ++i)
			{
				*iter++ = rightForehead[i].x;
				*iter++ = rightForehead[i].y;
			}

			glBindBuffer(GL_ARRAY_BUFFER, rightTexCoordVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 2, texCoordBuffer.data(), GL_STATIC_DRAW);
		}
	}
}

void RavensHalfFacePaint::transform(FXModel& face, RavensHalfFaceParams &params)
{
	std::array<float, 66 * 2> smoothedPoints;

	for (int i = 0; i < 66; ++i)
	{
		smoothedPoints[i * 2] = facePointsSmoothers[face.pointId][i * 2].responsiveAnalogReadSimple(face.pts[i * 2]);
		smoothedPoints[i * 2 + 1] = facePointsSmoothers[face.pointId][i * 2 + 1].responsiveAnalogReadSimple(face.pts[i * 2  +1]);
	}

	ImagePoints points(smoothedPoints);

	cv::Point2f chinPoint = points.at(9);
	cv::Point2f tmp = PointCalcUtil::crossPointTwoLines(chinPoint, points.at(28), points.at(21), points.at(24));
	Eigen::Vector2f topPoint = { tmp.x, tmp.y };

	Eigen::Vector2f verticalOffset = points.getEigenAt(28) - Eigen::Vector2f(chinPoint.x, chinPoint.y);
	verticalOffset *= 0.2;

	// indexes start with 1
	const std::array<int, 6> leftEyePoints =
	{
		37, 38, 39, 40, 41, 42
	};
	const std::array<int, 6> rightEyePoints =
	{
		46, 45, 44, 43, 48, 47
	};

	auto leftEyeCenter = PointCalcUtil::crossPointTwoLines(points.at(38), points.at(41), points.at(39), points.at(42));
	auto rightEyeCenter = PointCalcUtil::crossPointTwoLines(points.at(44), points.at(47), points.at(45), points.at(48));

	for (int i = 0; i < 6; ++i)
	{
		auto newLeft = leftEyeCenter + (points.at(leftEyePoints[i]) - leftEyeCenter) * 1.4;
		auto newRight = rightEyeCenter + (points.at(rightEyePoints[i]) - rightEyeCenter) * 1.4;

		points.pts[(leftEyePoints[i] - 1) * 2] = newLeft.x;
		points.pts[(leftEyePoints[i] - 1) * 2 + 1] = newLeft.y;
		points.pts[(rightEyePoints[i] - 1) * 2] = newRight.x;
		points.pts[(rightEyePoints[i] - 1) * 2 + 1] = newRight.y;
	}

	const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

	for (int i = 0; i < 2; ++i)
	{
		if (i == 0 && (params.leftAmbient[3] == 0 && (params.leftTexId == 0 || params.leftDiffuse[3] == 0)))
		{
			continue;
		}

		if (i == 1 && (params.rightAmbient[3] == 0 && (params.rightTexId == 0 || params.rightDiffuse[3] == 0)))
		{
			continue;
		}

		std::array<float, (total) * 3> buffer;

		auto iter = buffer.begin();

		for (auto &facePart : indexes[i])
		{
			for (auto index : facePart)
			{
				if (index > -1)
				{
					*iter++ = points.pts[index * 2];
					*iter++ = points.pts[index * 2 + 1];
					*iter++ = 0;
				}
				else
				{
					*iter++ = topPoint[0];
					*iter++ = topPoint[1];
					*iter++ = 0;
				}
			}
		}

		if (i == 0)
		{
			std::array<Eigen::Vector2f, FOREHEAD_STRIP_COUNT> leftForehead =
			{
				points.getEigenAt(1), points.getEigenAt(18) + verticalOffset * 0.25, points.getEigenAt(18),
				points.getEigenAt(19) + verticalOffset * 0.5, points.getEigenAt(19), points.getEigenAt(21) + verticalOffset * 0.75,
				points.getEigenAt(21), topPoint + verticalOffset, topPoint
			};

			for (int i = 0; i < FOREHEAD_STRIP_COUNT; ++i)
			{
				*iter++ = leftForehead[i][0];
				*iter++ = leftForehead[i][1];
				*iter++ = 0;
			}

			glBindBuffer(GL_ARRAY_BUFFER, leftVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 3, buffer.data(), GL_STREAM_DRAW);
		}
		else
		{
			std::array<Eigen::Vector2f, FOREHEAD_STRIP_COUNT> rightForehead =
			{
				points.getEigenAt(17), points.getEigenAt(27) + verticalOffset * 0.25, points.getEigenAt(27),
				points.getEigenAt(26) + verticalOffset * 0.5, points.getEigenAt(26), points.getEigenAt(24) + verticalOffset * 0.75,
				points.getEigenAt(24), topPoint + verticalOffset, topPoint
			};

			for (int i = 0; i < FOREHEAD_STRIP_COUNT; ++i)
			{
				*iter++ = rightForehead[i][0];
				*iter++ = rightForehead[i][1];
				*iter++ = 0;
			}

			glBindBuffer(GL_ARRAY_BUFFER, rightVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 3, buffer.data(), GL_STREAM_DRAW);
		}
	}

	if ((params.lineAmbient[3] != 0 || params.lineTexId != 0 && params.lineDiffuse[3] != 0) && params.lineWidth != 0)
	{
		const std::array<Eigen::Vector2f, LINE_STRIP_COUNT / 2> line_strip =
		{
			topPoint + verticalOffset, topPoint,
			points.getEigenAt(28), points.getEigenAt(29), points.getEigenAt(30), points.getEigenAt(31), points.getEigenAt(34), points.getEigenAt(52)
		};

		std::array<float, (LINE_STRIP_COUNT + 4) * 3 > buffer;
		auto iter = buffer.begin();

		Eigen::Vector2f route = PointCalcUtil::getVectorNormal(line_strip[1] - line_strip[0]);

		Eigen::Vector2f point = line_strip[0] + route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		 point = line_strip[0] - route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		for (int i = 1; i < LINE_STRIP_COUNT / 2 - 1; ++i)
		{
			route = PointCalcUtil::getBisector(line_strip[i - 1] - line_strip[i], line_strip[i + 1] - line_strip[i]);
			if (route[0] < 0) route[0] = -route[0];

			point = line_strip[i] + route * params.lineWidth;
			*iter++ = point[0];
			*iter++ = point[1];
			*iter++ = 0;

			point = line_strip[i] - route * params.lineWidth;
			*iter++ = point[0];
			*iter++ = point[1];
			*iter++ = 0;
		}

		route = PointCalcUtil::getVectorNormal(line_strip[LINE_STRIP_COUNT / 2 - 1] - line_strip[LINE_STRIP_COUNT / 2 - 2]);
		
		point = line_strip[LINE_STRIP_COUNT / 2 - 1] + route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		point = line_strip[LINE_STRIP_COUNT / 2 - 1] - route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		route = PointCalcUtil::getVectorNormal(points.getEigenAt(9) - points.getEigenAt(58));

		point = points.getEigenAt(58) + route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		point = points.getEigenAt(58) - route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		point = points.getEigenAt(9) + route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		point = points.getEigenAt(9) - route * params.lineWidth;
		*iter++ = point[0];
		*iter++ = point[1];
		*iter++ = 0;

		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (LINE_STRIP_COUNT + 4) * 3, buffer.data(), GL_STREAM_DRAW);
	}
}

void RavensHalfFacePaint::draw(FXModel& face, RavensHalfFaceParams &params, std::shared_ptr<cwc::glShader> shader)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);

	const static size_t CHIN_STRIP_START = 0;
	const static size_t CHEEK_FAN_START = 9;
	const static size_t BROW_STRIP_START = 19;
	const static size_t NOSE_FAN_START = 29;
	const static size_t UNDER_NOSE_FAN_START = 37;
	const static size_t FOREHEAD_STRIP_START = 43;

	if (params.facemaskTexturePath.empty())
	{
		throw std::exception("Not implemented, use OpenGLFacePaintModel");
	}
	else
	{
		{
			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, leftVBO));
			VertexAttribSetter vTexCoord;
			vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, leftTexCoordVBO));

			glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);

			assert(glGetError() == GL_NO_ERROR);
		}

		{
			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, rightVBO));
			VertexAttribSetter vTexCoord;
			vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, rightTexCoordVBO));

			glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);
		}
	}

	glPopAttrib();
}

void RavensHalfFacePaint::unload()
{
	glDeleteBuffers(1, &leftVBO);
	glDeleteBuffers(1, &rightVBO);
	glDeleteBuffers(1, &lineVBO);

	glDeleteBuffers(1, &leftTexCoordVBO);
	glDeleteBuffers(1, &rightTexCoordVBO);
	glDeleteBuffers(1, &lineTexCoordVBO);

	glDeleteBuffers(1, &textureid);
	textureid = 0;
}

//================================================================================
//================================================================================
//================================================================================

RavensFacePaintModel::RavensFacePaintModel()
{
}

void RavensFacePaintModel::onInputFrameResized()
{
	OpenGLModel::onInputFrameResized();

	for (auto &face : halfFacePaint.facePointsSmoothers)
	{
		for (auto &smoother : face)
		{
			smoother.SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		}
	}
}

void RavensFacePaintModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);
}

void RavensFacePaintModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately)
{
	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	if (!visible[faceIndex])
	{
		return;
	}

	auto renderParamsID = suit.get_optional<size_t>("renderParamsID");
	if (renderParamsID)
	{
		objectRenderParams[faceIndex].clear();
		objectRenderParams[faceIndex].push_back(commonRenderParams[renderParamsID.get()]);
	}

	halfFaceParams[faceIndex].applySuit(suit, loadTexturesImmediately);
}

bool RavensFacePaintModel::load()
{
	for (auto &params : halfFaceParams)
	{
		params.load();
		break;
	}

	halfFacePaint.load();

	return true;
}

void RavensFacePaintModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;

	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = 2 / width;
	projection(1, 1) = -2 / height;
	projection(2, 2) = 2 / (zNear - zFar);
	projection(3, 3) = 1;
	projection(0, 3) = -1;
	projection(1, 3) = 1;
	projection(2, 3) = (zFar + zNear) / (zNear - zFar);

	renderParams.projection = projection;

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;

	double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
	depth = -distance;

	Matrix4d modelMatrix(Affine3d(Translation3d(Vector3d(0, 0, -distance))).matrix());

	renderParams.modelView = modelMatrix.cast<float>();
	renderParams.rotationMatrix = Matrix3f::Identity();
	renderParams.yawMatrix = Matrix3f::Identity();

	halfFacePaint.transform(face, halfFaceParams[face.pointId]);
}

void RavensFacePaintModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	if (!halfFaceParams[face.pointId].facemaskTexturePath.empty())
	{
		ResourceManager::TextureLoadingResult result = resourceManager.loadTexture(std::string("Texture"), gtexture);

		textureid = result.ID;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureid);
		halfFacePaint.textureid = textureid;
	}

	SetUniformsForObject(*shader, 0, face.pointId);

	halfFacePaint.draw(face, halfFaceParams[face.pointId], shader);
}

void RavensFacePaintModel::unload()
{
	halfFacePaint.unload();
}

std::shared_ptr<ObjectRenderParams> RavensFacePaintModel::createDefaultObjectRenderParams()
{
	return std::make_shared<ObjectRenderParams>();
}

void RavensFacePaintModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
}