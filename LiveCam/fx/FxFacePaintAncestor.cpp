#include "fx/FxFacePaintAncestor.h"

extern GLuint cameraTextureId;

static const cv::Point2f topPointTexCoord = { 0.5f, 0.2f }; // 500, 200
static const cv::Point2f headTexMapTopPointTexCoord = { 0.503125f, 0.279688f };

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
	cv::Point2f{ 171, 215 }, // above 17
	cv::Point2f{ 237, 155 }, // above 18
	cv::Point2f{ 375, 127 }, // above 20
	cv::Point2f{ 490, 95 }, // above TOP
	cv::Point2f{ 623, 127 }, // above 23
	cv::Point2f{ 765, 155 }, // above 25
	cv::Point2f{ 829, 215 } // above 26
};

static const std::array<cv::Point2f, 66 + 7> headTexMapCoords =
{
	cv::Point2f{ 0.33125f, 0.299219f }, // 0
	cv::Point2f{ 0.336719f, 0.327344f }, // 1
	cv::Point2f{ 0.34375f, 0.354688f }, // 2
	cv::Point2f{ 0.353125f, 0.382813f }, // 3
	cv::Point2f{ 0.360156f, 0.409375f }, // 4
	cv::Point2f{ 0.430469f, 0.452344f }, // 5
	cv::Point2f{ 0.45f, 0.4625f }, // 6
	cv::Point2f{ 0.474219f, 0.469531f }, // 7
	cv::Point2f{ 0.501563f, 0.475f }, // 8
	cv::Point2f{ 0.528906f, 0.469531f }, // 9
	cv::Point2f{ 0.546875f, 0.4625f }, // 10
	cv::Point2f{ 0.567188f, 0.452344f }, // 11
	cv::Point2f{ 0.63125f, 0.409375f }, // 12
	cv::Point2f{ 0.65f, 0.382813f }, // 13
	cv::Point2f{ 0.659375f, 0.354688f }, // 14
	cv::Point2f{ 0.666406f, 0.327344f }, // 15
	cv::Point2f{ 0.671094f, 0.299219f }, // 16
	cv::Point2f{ 0.410156f, 0.273438f }, // 17
	cv::Point2f{ 0.427344f, 0.264063f }, // 18
	cv::Point2f{ 0.446875f, 0.264063f }, // 19
	cv::Point2f{ 0.465625f, 0.269531f }, // 20
	cv::Point2f{ 0.482813f, 0.279688f }, // 21
	cv::Point2f{ 0.519531f, 0.279688f }, // 22
	cv::Point2f{ 0.536719f, 0.269531f }, // 23
	cv::Point2f{ 0.554688f, 0.264063f }, // 24
	cv::Point2f{ 0.574219f, 0.264063f }, // 25
	cv::Point2f{ 0.590625f, 0.273438f }, // 26
	cv::Point2f{ 0.501563f, 0.305469f }, // 27
	cv::Point2f{ 0.501563f, 0.322656f }, // 28
	cv::Point2f{ 0.501563f, 0.341406f }, // 29
	cv::Point2f{ 0.501563f, 0.358594f }, // 30
	cv::Point2f{ 0.484375f, 0.372656f }, // 31
	cv::Point2f{ 0.492188f, 0.375781f }, // 32
	cv::Point2f{ 0.501563f, 0.377344f }, // 33
	cv::Point2f{ 0.509375f, 0.375781f }, // 34
	cv::Point2f{ 0.517188f, 0.372656f }, // 35
	cv::Point2f{ 0.421094f, 0.300781f }, // 36
	cv::Point2f{ 0.438281f, 0.292969f }, // 37
	cv::Point2f{ 0.457031f, 0.294531f }, // 38
	cv::Point2f{ 0.475781f, 0.307031f }, // 39
	cv::Point2f{ 0.455469f, 0.311719f }, // 40
	cv::Point2f{ 0.435938f, 0.311719f }, // 41
	cv::Point2f{ 0.527344f, 0.307031f }, // 42
	cv::Point2f{ 0.54375f, 0.294531f }, // 43
	cv::Point2f{ 0.564063f, 0.292969f }, // 44
	cv::Point2f{ 0.580469f, 0.300781f }, // 45
	cv::Point2f{ 0.565625f, 0.311719f }, // 46
	cv::Point2f{ 0.546875f, 0.311719f }, // 47
	cv::Point2f{ 0.460156f, 0.407813f }, // 48
	cv::Point2f{ 0.472656f, 0.400781f }, // 49
	cv::Point2f{ 0.485938f, 0.396094f }, // 50
	cv::Point2f{ 0.501563f, 0.396094f }, // 51
	cv::Point2f{ 0.515625f, 0.396094f }, // 52
	cv::Point2f{ 0.528906f, 0.400781f }, // 53
	cv::Point2f{ 0.540625f, 0.407813f }, // 54
	cv::Point2f{ 0.528906f, 0.421875f }, // 55
	cv::Point2f{ 0.515625f, 0.428125f }, // 56
	cv::Point2f{ 0.501563f, 0.429688f }, // 57
	cv::Point2f{ 0.485938f, 0.428125f }, // 58
	cv::Point2f{ 0.472656f, 0.421875f }, // 59
	cv::Point2f{ 0.485938f, 0.40625f }, // 60
	cv::Point2f{ 0.501563f, 0.40625f }, // 61
	cv::Point2f{ 0.517188f, 0.40625f }, // 62
	cv::Point2f{ 0.517188f, 0.4125f }, // 63
	cv::Point2f{ 0.501563f, 0.4125f }, // 64
	cv::Point2f{ 0.485938f, 0.4125f }, // 65
	cv::Point2f{ 0.410156f, 0.255469f }, // 66
	cv::Point2f{ 0.427344f, 0.244531f }, // 67
	cv::Point2f{ 0.465625f, 0.241406f }, // 68
	cv::Point2f{ 0.501563f, 0.239844f }, // 69
	cv::Point2f{ 0.536719f, 0.241406f }, // 70
	cv::Point2f{ 0.574219f, 0.244531f }, // 71
	cv::Point2f{ 0.590625f, 0.255469f } // 72
};

const static cv::Point icon_position(296, 538);

//================================================================================
//================================================================================
//================================================================================

HalfFaceParams::HalfFaceParams() { }

HalfFaceParams::HalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
	Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
	std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth,
	std::string alphaMask, float alphaMaskPower)
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
	, alphaMask(alphaMask)
	, alphaMaskPower(alphaMaskPower)
{ }


void HalfFaceParams::applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately)
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
	alphaMask = suit.get<std::string>("alphaMask", "");

	alphaMaskPower = suit.get<float>("alphaMaskPower", 1);

	lineWidth = suit.get<float>("lineWidth", 0);

	verticalOffset = suit.get<float>("verticalOffset", 0);

	if (loadTexturesImmediately)
	{
		leftTexId = resourceManager.loadTexture(leftTexture).ID;
		rightTexId = resourceManager.loadTexture(rightTexture).ID;
		lineTexId = resourceManager.loadTexture(lineTexture).ID;
		alphaMaskId = resourceManager.loadTexture(alphaMask).ID;
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

void HalfFaceParams::load()
{
	leftTexId = resourceManager.loadTexture(leftTexture).ID;
	auto i = resourceManager.loadTexture(rightTexture);
	GLuint id = i.ID;
	rightTexId = resourceManager.loadTexture(rightTexture).ID;
	lineTexId = resourceManager.loadTexture(lineTexture).ID;
	alphaMaskId = resourceManager.loadTexture(alphaMask).ID;
}

const std::vector<std::vector<std::vector<int>>> HalfFacePaint::indexes =
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


std::array<cv::Point2f, 66 + 7> texCoordTable;
void HalfFacePaint::load(bool headTextureMap)
{
	glGenBuffers(1, &leftVBO);
	glGenBuffers(1, &rightVBO);
	glGenBuffers(1, &lineVBO);

	glGenBuffers(1, &leftTexCoordVBO);
	glGenBuffers(1, &rightTexCoordVBO);
	glGenBuffers(1, &lineTexCoordVBO);

	if (headTextureMap)
	{
		for (size_t i = 0; i < 66 + 7; i++)
		{
			texCoordTable[i] = headTexMapCoords[i];
		}
	}
	else
	{
		for (size_t i = 0; i < 66 + 7; i++)
		{
			texCoordTable[i] = { texCoordInitTable[i].x / 1000.f, texCoordInitTable[i].y / 1000.f };
		}
	}

	const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

	for (int i = 0; i < 2; ++i)
	{
		std::array<float, total * 2> texCoordBuffer;

		auto iter = texCoordBuffer.begin();

		for (auto &faceBone : indexes[i])
		{
			for (auto index : faceBone)
			{
				if (index > -1)
				{
					*iter++ = texCoordTable[index].x;
					*iter++ = texCoordTable[index].y;
				}
				else
				{
					*iter++ = headTextureMap ? headTexMapTopPointTexCoord.x : topPointTexCoord.x;
					*iter++ = headTextureMap ? headTexMapTopPointTexCoord.y : topPointTexCoord.y;
				}
			}
		}

		if (i == 0)
		{
			std::array<cv::Point2f, FOREHEAD_STRIP_COUNT> leftForehead =
			{
				texCoordTable[0], texCoordTable[66], texCoordTable[17], texCoordTable[67], texCoordTable[18],
				texCoordTable[68], texCoordTable[20], texCoordTable[69], headTextureMap ? headTexMapTopPointTexCoord : topPointTexCoord
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
				texCoordTable[70], texCoordTable[23], texCoordTable[69], headTextureMap ? headTexMapTopPointTexCoord : topPointTexCoord
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

void HalfFacePaint::transform(FXModel& face, HalfFaceParams &params)
{
	//verticalOffsetFromJson = 0.0;

	std::array<float, 66 * 2> smoothedPoints;

	for (int i = 0; i < 66; ++i)
	{
		smoothedPoints[i * 2] = facePointsSmoothers[face.pointId][i * 2].responsiveAnalogReadSimple(face.pts[i * 2]);
		smoothedPoints[i * 2 + 1] = facePointsSmoothers[face.pointId][i * 2 + 1].responsiveAnalogReadSimple(face.pts[i * 2 + 1]);
	}

	ImagePoints points(smoothedPoints);

	cv::Point2f chinPoint = points.at(9);
	cv::Point2f tmp = PointCalcUtil::crossPointTwoLines(chinPoint, points.at(28), points.at(21), points.at(24));
	Eigen::Vector2f topPoint = { tmp.x, tmp.y };

	Eigen::Vector2f verticalOffset = points.getEigenAt(28) - Eigen::Vector2f(chinPoint.x, chinPoint.y);
	//verticalOffset *= 0.5;
	verticalOffset *= ((params.verticalOffset == 0.0) ? 0.2 : params.verticalOffset);


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

	int index = 0;
	for (int i = 9; i > 0; --i)
	{
		faceLeftContour[index++] = { points.getEigenAt(i) };
	}

	index = 0;
	for (int i = 9; i < 18; ++i)
	{
		faceRightContour[index++] = { points.getEigenAt(i) };
	}

	index = 0;
	for (int i = 58; i < 61; ++i)
	{
		mouthLeftContour[index++] = { points.getEigenAt(i) };
	}
	for (int i = 49; i < 53; ++i)
	{
		mouthLeftContour[index++] = { points.getEigenAt(i) };
	}

	index = 0;
	for (int i = 52; i < 59; ++i)
	{
		mouthRightContour[index++] = { points.getEigenAt(i) };
	}

	index = 0;
	for (int i = 37; i < 43; ++i)
	{
		eyeLeftContour[index++] = { points.getEigenAt(i) };
	}
	eyeLeftContour[index] = { points.getEigenAt(37) };

	index = 0;
	for (int i = 43; i < 49; ++i)
	{
		eyeRightContour[index++] = { points.getEigenAt(i) };
	}
	eyeRightContour[index] = { points.getEigenAt(43) };

	index = 0;
	middleTopLines[index++] = topPoint + verticalOffset;
	middleTopLines[index++] = topPoint;
	for (int i = 28; i < 32; ++i)
	{
		middleTopLines[index++] = { points.getEigenAt(i) };
	}
	middleTopLines[index++] = { points.getEigenAt(34) };
	middleTopLines[index++] = { points.getEigenAt(52) };

	middleBottomLines[0] = { points.getEigenAt(58) };
	middleBottomLines[1] = { points.getEigenAt(9) };

	const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

	for (int i = 0; i < 2; ++i)
	{
		//if (i == 0 && (params.leftAmbient[3] == 0 && params.leftTexId == 0))
		//{
		//	continue;
		//}

		//if (i == 1 && (params.rightAmbient[3] == 0 && params.rightTexId == 0))
		//{
		//	continue;
		//}

		std::array<float, (total) * 3> buffer;

		auto iter = buffer.begin();

		for (auto &faceBone : indexes[i])
		{
			for (auto index : faceBone)
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
				points.getEigenAt(1), points.getEigenAt(18) + verticalOffset * 0.5, points.getEigenAt(18),
				points.getEigenAt(19) + verticalOffset * 0.75, points.getEigenAt(19), points.getEigenAt(21) + verticalOffset * 0.95,
				points.getEigenAt(21), topPoint + verticalOffset, topPoint
			};

			faceLeftContour[9] = leftForehead[1];
			faceLeftContour[10] = leftForehead[3];
			faceLeftContour[11] = leftForehead[5];
			faceLeftContour[12] = leftForehead[7];

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
				points.getEigenAt(17), points.getEigenAt(27) + verticalOffset * 0.5, points.getEigenAt(27),
				points.getEigenAt(26) + verticalOffset * 0.75, points.getEigenAt(26), points.getEigenAt(24) + verticalOffset * 0.95,
				points.getEigenAt(24), topPoint + verticalOffset, topPoint
			};

			faceRightContour[9] = rightForehead[1];
			faceRightContour[10] = rightForehead[3];
			faceRightContour[11] = rightForehead[5];
			faceRightContour[12] = rightForehead[7];

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

	if ((params.lineAmbient[3] != 0 || params.lineTexId != 0) && params.lineWidth != 0)
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

#define LoadLinesToUniform(linesArray, uniformName) \
for (int i = 0; i < linesArray.size() - 1; ++i, ++index) \
{ \
	float length = (linesArray[i + 1] - linesArray[i]).norm(); \
 \
	shader->setUniform2f(StructUniformField1(uniformName, index, "A"), linesArray[i][0], linesArray[i][1]); \
	shader->setUniform2f(StructUniformField1(uniformName, index, "B"), linesArray[i + 1][0], linesArray[i + 1][1]); \
	shader->setUniform1f(StructUniformField1(uniformName, index, "length"), length); \
}

void HalfFacePaint::draw(FXModel& face, HalfFaceParams &params, std::shared_ptr<cwc::glShader> shader)
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

	GLint edgeFadingDistanceLoc = shader->GetUniformLocation("edgeFadingDistance");

	if (edgeFadingDistanceLoc > -1)
	{
		float edgeFadingDistance = face.faceWidth * 0.05f * face.fWidth;
		shader->setUniform1f("edgeFadingDistance", edgeFadingDistance);

		int index = 0;
		LoadLinesToUniform(middleTopLines, "edgesMiddleTop");
		index = 0;
		LoadLinesToUniform(middleBottomLines, "edgesMiddleBottom");
	}

	int texture = shader->GetUniformLocation("Texture");
	glUniform1i(texture, 0);

	int alphaMask = shader->GetUniformLocation("AlphaMaskTexture");
	glUniform1i(alphaMask, 1);

	if (params.leftAmbient[3] != 0)
	{
		shader->setUniform4f("leftColor", params.leftAmbient[0], params.leftAmbient[1], params.leftAmbient[2], params.leftAmbient[3]);
	}
	else
	{
		shader->setUniform4f("leftColor", 0, 0, 0, 0);
	}

	if (params.rightAmbient[3] != 0)
	{
		shader->setUniform4f("rightColor", params.rightAmbient[0], params.rightAmbient[1], params.rightAmbient[2], params.rightAmbient[3]);
	}
	else
	{
		shader->setUniform4f("rightColor", 0, 0, 0, 0);
	}

	//if (params.leftAmbient[3] != 0 || params.leftTexId != 0)
	{
		if (edgeFadingDistanceLoc > -1)
		{
			int index = 0;
			LoadLinesToUniform(faceLeftContour, "edgesLeft");
			LoadLinesToUniform(eyeLeftContour, "edgesLeft");
			LoadLinesToUniform(mouthLeftContour, "edgesLeft");
		}

		shader->setUniform1i("isLeft", 1);

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, leftVBO));
		VertexAttribSetter vTexCoord;

		//if (params.leftTexId != 0 || params.alphaMaskId != 0)
		{
			vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, leftTexCoordVBO));
		}

		if (params.leftTexId != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, params.leftTexId);

			shader->setUniform1i("hasTexture", 1);
		}
		else
		{
			shader->setUniform1i("hasTexture", 0);
		}

		if (params.alphaMaskId != 0 && params.alphaMaskPower > 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, params.alphaMaskId);

			shader->setUniform1i("hasAlphaMask", 1);
			shader->setUniform1f("alphaMaskPower", params.alphaMaskPower);
		}
		else
		{
			shader->setUniform1i("hasAlphaMask", 0);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);

	}

	//if (params.rightAmbient[3] != 0 || params.rightTexId != 0)
	{
		if (edgeFadingDistanceLoc > -1)
		{
			int index = 0;
			LoadLinesToUniform(faceRightContour, "edgesRight");
			LoadLinesToUniform(eyeRightContour, "edgesRight");
			LoadLinesToUniform(mouthRightContour, "edgesRight");
		}

		shader->setUniform1i("isLeft", 0);

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, rightVBO));
		VertexAttribSetter vTexCoord;

		//if (params.rightTexId != 0 || params.alphaMaskId != 0)
		{
			vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, rightTexCoordVBO));
		}

		if (params.rightTexId != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, params.rightTexId);

			shader->setUniform1i("hasTexture", 1);
		}
		else
		{
			shader->setUniform1i("hasTexture", 0);
		}

		if (params.alphaMaskId != 0 && params.alphaMaskPower > 0)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, params.alphaMaskId);

			shader->setUniform1i("hasAlphaMask", 1);
			shader->setUniform1f("alphaMaskPower", params.alphaMaskPower);
		}
		else
		{
			shader->setUniform1i("hasAlphaMask", 0);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
		glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);
	}

	if (params.lineAmbient[3] != 0 || params.lineTexId != 0 && params.lineDiffuse[3] != 0 && false)
	{
		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, lineVBO));
		VertexAttribSetter vTexCoord;

		bool textureExist = (false && params.lineTexId != 0);

		if (textureExist)	//	false
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, params.lineTexId);

			vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, lineTexCoordVBO));
			shader->setUniform1i("hasTexture", 1);
		}
		else
		{
			shader->setUniform1i("hasTexture", 0);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, HalfFacePaint::LINE_STRIP_COUNT);
		glDrawArrays(GL_TRIANGLE_STRIP, HalfFacePaint::LINE_STRIP_COUNT, 4);
	}

	glPopAttrib();
}

void HalfFacePaint::unload()
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

OpenGLFacePaintModel::OpenGLFacePaintModel()
{
	headTextureMap = false;
	zOffset.fill(0);
	match3D.fill(false);
}

void OpenGLFacePaintModel::onInputFrameResized()
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

void OpenGLFacePaintModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);

	headTextureMap = modelRecord.get<bool>("headTextureMap", false);
}

void OpenGLFacePaintModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
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

	match3D[faceIndex] = suit.get<bool>("match3D", false);
	zOffset[faceIndex] = suit.get<float>("zOffset", 0);

	halfFaceParams[faceIndex].applySuit(suit, loadTexturesImmediately);
}

bool OpenGLFacePaintModel::load()
{
	for (auto &params : halfFaceParams)
	{
		params.load();
	}

	halfFacePaint.load(headTextureMap);

	return true;
}

void OpenGLFacePaintModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;

	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = 2 / width;
	projection(1, 1) = -2 / height;
	projection(3, 3) = 1;
	projection(0, 3) = -1;
	projection(1, 3) = 1;

	if (match3D[face.pointId])
	{
		projection(2, 2) = (zFar + zNear) / (zNear - zFar);
		projection(2, 3) = 2 * zFar * zNear / (zNear - zFar);
	}
	else
	{
		projection(2, 2) = 2 / (zNear - zFar);
		projection(2, 3) = (zFar + zNear) / (zNear - zFar);
	}

	renderParams.projection = projection;

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;

	double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
	depth = -distance;

	Matrix4d modelMatrix(Affine3d(Translation3d(Vector3d(0, 0, -distance + zOffset[face.pointId]))).matrix());

	renderParams.modelView = modelMatrix.cast<float>();
	renderParams.rotationMatrix = Matrix3f::Identity();
	renderParams.yawMatrix = Matrix3f::Identity();

	halfFacePaint.transform(face, halfFaceParams[face.pointId]);
}

void OpenGLFacePaintModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	SetUniformsForObject(*shader, 0, face.pointId);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, cameraTextureId);

	int alphaMask = shader->GetUniformLocation("OriginFrame");
	glUniform1i(alphaMask, 10);

	shader->setUniform1i("frameWidth", Resolutions::INPUT_ACTUAL_WIDTH);
	shader->setUniform1i("frameHeight", Resolutions::INPUT_ACTUAL_HEIGHT);

	shader->setUniform1f("faceBrightness", externalRenderParams.faceParams.brightness);

	float frameBrightness = max(externalRenderParams.frameParams.brightness, externalRenderParams.faceParams.brightness * 1.5f);
	shader->setUniform1f("frameBrightness", frameBrightness);
	shader->setUniform3f("frameLight", externalRenderParams.frameParams.light[2], externalRenderParams.frameParams.light[1], externalRenderParams.frameParams.light[0]);

	halfFacePaint.draw(face, halfFaceParams[face.pointId], shader);
}

void OpenGLFacePaintModel::unload()
{
	halfFacePaint.unload();
}

std::shared_ptr<ObjectRenderParams> OpenGLFacePaintModel::createDefaultObjectRenderParams()
{
	return std::make_shared<ObjectRenderParams>();
}

void OpenGLFacePaintModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
}


namespace old
{


	//================================================================================
	//================================================================================
	//================================================================================

	HalfFaceParams::HalfFaceParams() { }

	HalfFaceParams::HalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
		Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
		std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth, std::string alphaMask)
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
		, alphaMask(alphaMask)
	{ }


	void HalfFaceParams::applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately)
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
		alphaMask = suit.get<std::string>("alphaMask", "");

		lineWidth = suit.get<float>("lineWidth", 0);

		if (loadTexturesImmediately)
		{
			leftTexId = resourceManager.loadTexture(leftTexture).ID;
			rightTexId = resourceManager.loadTexture(rightTexture).ID;
			lineTexId = resourceManager.loadTexture(lineTexture).ID;
			alphaMaskId = resourceManager.loadTexture(alphaMask).ID;
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

	void HalfFaceParams::load()
	{
		leftTexId = resourceManager.loadTexture(leftTexture).ID;
		auto i = resourceManager.loadTexture(rightTexture);
		GLuint id = i.ID;
		rightTexId = resourceManager.loadTexture(rightTexture).ID;
		lineTexId = resourceManager.loadTexture(lineTexture).ID;
		alphaMaskId = resourceManager.loadTexture(alphaMask).ID;
	}

	const std::vector<std::vector<std::vector<int>>> HalfFacePaint::indexes =
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


	std::array<cv::Point2f, 66 + 7> texCoordTable;
	void HalfFacePaint::load(bool headTextureMap)
	{
		glGenBuffers(1, &leftVBO);
		glGenBuffers(1, &rightVBO);
		glGenBuffers(1, &lineVBO);

		glGenBuffers(1, &leftTexCoordVBO);
		glGenBuffers(1, &rightTexCoordVBO);
		glGenBuffers(1, &lineTexCoordVBO);

		if (headTextureMap)
		{
			for (size_t i = 0; i < 66 + 7; i++)
			{
				texCoordTable[i] = headTexMapCoords[i];
			}
		}
		else
		{
			for (size_t i = 0; i < 66 + 7; i++)
			{
				texCoordTable[i] = { texCoordInitTable[i].x / 1000.f, texCoordInitTable[i].y / 1000.f };
			}
		}

		const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

		for (int i = 0; i < 2; ++i)
		{
			std::array<float, total * 2> texCoordBuffer;

			auto iter = texCoordBuffer.begin();

			for (auto &faceBone : indexes[i])
			{
				for (auto index : faceBone)
				{
					if (index > -1)
					{
						*iter++ = texCoordTable[index].x;
						*iter++ = texCoordTable[index].y;
					}
					else
					{
						*iter++ = headTextureMap ? headTexMapTopPointTexCoord.x : topPointTexCoord.x;
						*iter++ = headTextureMap ? headTexMapTopPointTexCoord.y : topPointTexCoord.y;
					}
				}
			}

			if (i == 0)
			{
				std::array<cv::Point2f, FOREHEAD_STRIP_COUNT> leftForehead =
				{
					texCoordTable[0], texCoordTable[66], texCoordTable[17], texCoordTable[67], texCoordTable[18],
					texCoordTable[68], texCoordTable[20], texCoordTable[69], headTextureMap ? headTexMapTopPointTexCoord : topPointTexCoord
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
					texCoordTable[70], texCoordTable[23], texCoordTable[69], headTextureMap ? headTexMapTopPointTexCoord : topPointTexCoord
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

	void HalfFacePaint::transform(FXModel& face, HalfFaceParams &params)
	{
		std::array<float, 66 * 2> smoothedPoints;

		for (int i = 0; i < 66; ++i)
		{
			smoothedPoints[i * 2] = facePointsSmoothers[face.pointId][i * 2].responsiveAnalogReadSimple(face.pts[i * 2]);
			smoothedPoints[i * 2 + 1] = facePointsSmoothers[face.pointId][i * 2 + 1].responsiveAnalogReadSimple(face.pts[i * 2 + 1]);
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

		int index = 0;
		for (int i = 9; i > 0; --i)
		{
			faceLeftContour[index++] = { points.getEigenAt(i) };
		}

		index = 0;
		for (int i = 9; i < 18; ++i)
		{
			faceRightContour[index++] = { points.getEigenAt(i) };
		}

		index = 0;
		for (int i = 58; i < 61; ++i)
		{
			mouthLeftContour[index++] = { points.getEigenAt(i) };
		}
		for (int i = 49; i < 53; ++i)
		{
			mouthLeftContour[index++] = { points.getEigenAt(i) };
		}

		index = 0;
		for (int i = 52; i < 59; ++i)
		{
			mouthRightContour[index++] = { points.getEigenAt(i) };
		}

		index = 0;
		for (int i = 37; i < 43; ++i)
		{
			eyeLeftContour[index++] = { points.getEigenAt(i) };
		}
		eyeLeftContour[index] = { points.getEigenAt(37) };

		index = 0;
		for (int i = 43; i < 49; ++i)
		{
			eyeRightContour[index++] = { points.getEigenAt(i) };
		}
		eyeRightContour[index] = { points.getEigenAt(43) };

		index = 0;
		middleTopLines[index++] = topPoint + verticalOffset;
		middleTopLines[index++] = topPoint;
		for (int i = 28; i < 32; ++i)
		{
			middleTopLines[index++] = { points.getEigenAt(i) };
		}
		middleTopLines[index++] = { points.getEigenAt(34) };
		middleTopLines[index++] = { points.getEigenAt(52) };

		middleBottomLines[0] = { points.getEigenAt(58) };
		middleBottomLines[1] = { points.getEigenAt(9) };

		const size_t total = CHIN_STRIP_COUNT + CHEEK_FAN_COUNT + BROW_STRIP_COUNT + NOSE_FAN_COUNT + UNDER_NOSE_FAN_COUNT + FOREHEAD_STRIP_COUNT;

		for (int i = 0; i < 2; ++i)
		{
			if (i == 0 && (params.leftAmbient[3] == 0 && params.leftTexId == 0))
			{
				continue;
			}

			if (i == 1 && (params.rightAmbient[3] == 0 && params.rightTexId == 0))
			{
				continue;
			}

			std::array<float, (total) * 3> buffer;

			auto iter = buffer.begin();

			for (auto &faceBone : indexes[i])
			{
				for (auto index : faceBone)
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

				faceLeftContour[9] = leftForehead[1];
				faceLeftContour[10] = leftForehead[3];
				faceLeftContour[11] = leftForehead[5];
				faceLeftContour[12] = leftForehead[7];

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

				faceRightContour[9] = rightForehead[1];
				faceRightContour[10] = rightForehead[3];
				faceRightContour[11] = rightForehead[5];
				faceRightContour[12] = rightForehead[7];

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

		if ((params.lineAmbient[3] != 0 || params.lineTexId != 0) && params.lineWidth != 0)
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

#define LoadLinesToUniform(linesArray, uniformName) \
for (int i = 0; i < linesArray.size() - 1; ++i, ++index) \
{ \
	float length = (linesArray[i + 1] - linesArray[i]).norm(); \
 \
	shader->setUniform2f(StructUniformField1(uniformName, index, "A"), linesArray[i][0], linesArray[i][1]); \
	shader->setUniform2f(StructUniformField1(uniformName, index, "B"), linesArray[i + 1][0], linesArray[i + 1][1]); \
	shader->setUniform1f(StructUniformField1(uniformName, index, "length"), length); \
}

	void HalfFacePaint::draw(FXModel& face, HalfFaceParams &params, std::shared_ptr<cwc::glShader> shader)
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

		GLint edgeFadingDistanceLoc = shader->GetUniformLocation("edgeFadingDistance");

		if (edgeFadingDistanceLoc > -1)
		{
			float edgeFadingDistance = face.faceWidth * 0.05f * face.fWidth;
			shader->setUniform1f("edgeFadingDistance", edgeFadingDistance);

			int index = 0;
			LoadLinesToUniform(middleTopLines, "edgesMiddleTop");
			index = 0;
			LoadLinesToUniform(middleBottomLines, "edgesMiddleBottom");
		}

		int texture = shader->GetUniformLocation("Texture");
		glUniform1i(texture, 0);

		int alphaMask = shader->GetUniformLocation("AlphaMaskTexture");
		glUniform1i(alphaMask, 1);

		if (params.leftAmbient[3] != 0)
		{
			shader->setUniform4f("leftColor", params.leftAmbient[0], params.leftAmbient[1], params.leftAmbient[2], params.leftAmbient[3]);
		}
		else
		{
			shader->setUniform4f("leftColor", 0, 0, 0, 0);
		}

		if (params.rightAmbient[3] != 0)
		{
			shader->setUniform4f("rightColor", params.rightAmbient[0], params.rightAmbient[1], params.rightAmbient[2], params.rightAmbient[3]);
		}
		else
		{
			shader->setUniform4f("rightColor", 0, 0, 0, 0);
		}

		if (params.leftAmbient[3] != 0 || params.leftTexId != 0)
		{
			if (edgeFadingDistanceLoc > -1)
			{
				int index = 0;
				LoadLinesToUniform(faceLeftContour, "edgesLeft");
				LoadLinesToUniform(eyeLeftContour, "edgesLeft");
				LoadLinesToUniform(mouthLeftContour, "edgesLeft");
			}

			shader->setUniform1i("isLeft", 1);

			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, leftVBO));
			VertexAttribSetter vTexCoord;

			if (params.leftTexId != 0 || params.alphaMaskId != 0)
			{
				vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, leftTexCoordVBO));
			}

			if (params.leftTexId != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, params.leftTexId);

				shader->setUniform1i("hasTexture", 1);
			}
			else
			{
				shader->setUniform1i("hasTexture", 0);
			}

			if (params.alphaMaskId != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, params.alphaMaskId);

				shader->setUniform1i("hasAlphaMask", 1);
			}
			else
			{
				shader->setUniform1i("hasAlphaMask", 0);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);

		}

		if (params.rightAmbient[3] != 0 || params.rightTexId != 0)
		{
			if (edgeFadingDistanceLoc > -1)
			{
				int index = 0;
				LoadLinesToUniform(faceRightContour, "edgesRight");
				LoadLinesToUniform(eyeRightContour, "edgesRight");
				LoadLinesToUniform(mouthRightContour, "edgesRight");
			}

			shader->setUniform1i("isLeft", 0);

			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, rightVBO));
			VertexAttribSetter vTexCoord;

			if (params.rightTexId != 0 || params.alphaMaskId != 0)
			{
				vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, rightTexCoordVBO));
			}

			if (params.rightTexId != 0)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, params.rightTexId);

				shader->setUniform1i("hasTexture", 1);
			}
			else
			{
				shader->setUniform1i("hasTexture", 0);
			}

			if (params.alphaMaskId != 0)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, params.alphaMaskId);

				shader->setUniform1i("hasAlphaMask", 1);
			}
			else
			{
				shader->setUniform1i("hasAlphaMask", 0);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, CHIN_STRIP_START, CHIN_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, CHEEK_FAN_START, CHEEK_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, BROW_STRIP_START, BROW_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, NOSE_FAN_START, NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_FAN, UNDER_NOSE_FAN_START, UNDER_NOSE_FAN_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, FOREHEAD_STRIP_START, FOREHEAD_STRIP_COUNT);
		}

		if (params.lineAmbient[3] != 0 || params.lineTexId != 0 && params.lineDiffuse[3] != 0 && false)
		{
			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, lineVBO));
			VertexAttribSetter vTexCoord;

			bool textureExist = (false && params.lineTexId != 0);

			if (textureExist)	//	false
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, params.lineTexId);

				vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, lineTexCoordVBO));
				shader->setUniform1i("hasTexture", 1);
			}
			else
			{
				shader->setUniform1i("hasTexture", 0);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, HalfFacePaint::LINE_STRIP_COUNT);
			glDrawArrays(GL_TRIANGLE_STRIP, HalfFacePaint::LINE_STRIP_COUNT, 4);
		}

		glPopAttrib();
	}

	void HalfFacePaint::unload()
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

	OpenGLFacePaintModel::OpenGLFacePaintModel()
	{
		headTextureMap = false;
	}

	void OpenGLFacePaintModel::onInputFrameResized()
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

	void OpenGLFacePaintModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
	{
		OpenGLModel::loadFromJSON(modelRecord);

		headTextureMap = modelRecord.get<bool>("headTextureMap", false);
	}

	void OpenGLFacePaintModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
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

	bool OpenGLFacePaintModel::load()
	{
		for (auto &params : halfFaceParams)
		{
			params.load();
		}

		halfFacePaint.load(headTextureMap);

		return true;
	}

	void OpenGLFacePaintModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
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

	void OpenGLFacePaintModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
	{
		auto shader = objectRenderParams[face.pointId][0]->shader;

		ShaderSetter shaderSetter(shader, *this);

		SetUniformsForObject(*shader, 0, face.pointId);

		halfFacePaint.draw(face, halfFaceParams[face.pointId], shader);
	}

	void OpenGLFacePaintModel::unload()
	{
		halfFacePaint.unload();
	}

	std::shared_ptr<ObjectRenderParams> OpenGLFacePaintModel::createDefaultObjectRenderParams()
	{
		return std::make_shared<ObjectRenderParams>();
	}

	void OpenGLFacePaintModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
	{
	}








}