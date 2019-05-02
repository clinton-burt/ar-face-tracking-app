#include "fx/FxFacePaintAncestor2.h"
#include "delaunay.h"

static const float new_texCoordInitTable[60*2] = {
	0.3385, -0.7020,
	0.3376, -0.6670,
	0.3347, -0.6331,
	0.3472, -0.5924,
	0.3806, -0.5639,
	0.4096, -0.5413,
	0.4424, -0.5236,
	0.4773, -0.5182,
	0.5000, -0.5158,
	0.5227, -0.5182,
	0.5576, -0.5236,
	0.5904, -0.5413,
	0.6194, -0.5639,
	0.6528, -0.5924,
	0.6653, -0.6331,
	0.6624, -0.6670,
	0.6615, -0.7020,
	0.4240, -0.7236,
	0.4367, -0.7282,
	0.4503, -0.7310,
	0.4641, -0.7324,
	0.4774, -0.7269,
	0.5226, -0.7269,
	0.5359, -0.7324,
	0.5497, -0.7310,
	0.5633, -0.7282,
	0.5760, -0.7236,
	0.5000, -0.6927,
	0.5000, -0.6763,
	0.5000, -0.6562,
	0.5000, -0.6410,
	0.4786, -0.6338,
	0.4913, -0.6278,
	0.5000, -0.6268,
	0.5087, -0.6278,
	0.5214, -0.6338,
	0.4344, -0.6940,
	0.4477, -0.7007,
	0.4566, -0.6989,
	0.4644, -0.6904,
	0.4537, -0.6853,
	0.4462, -0.6850,
	0.5356, -0.6904,
	0.5434, -0.6989,
	0.5523, -0.7007,
	0.5651, -0.6955,
	0.5538, -0.6850,
	0.5462, -0.6855,
	0.4649, -0.5906,
	0.4796, -0.6045,
	0.4925, -0.6051,
	0.5000, -0.6034,
	0.5076, -0.6048,
	0.5202, -0.6055,
	0.5351, -0.5905,
	0.5260, -0.5776,
	0.5132, -0.5700,
	0.5000, -0.5689,
	0.4868, -0.5700,
	0.4741, -0.5776
};


//================================================================================
//================================================================================
//================================================================================

FacePaintParams::FacePaintParams() { }

FacePaintParams::FacePaintParams(std::string texture)
	:texture(texture)
{ }

void FacePaintParams::applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately)
{
	texture = suit.get<std::string>("Texture", "");
	if (loadTexturesImmediately)
	{
		textureId = resourceManager.loadTexture(texture).ID;
	}
}

static cv::Mat gtexture;
static std::vector<int> triangles;
static std::vector<cv::Point> texture_points;

static void drawline(cv::Mat image, const int* idxs, int count)
{
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < count; i++)
	{
		cv::Point p1 = texture_points[idxs[i]];
		cv::Point p2 = texture_points[idxs[i + 1]];
		cv::line(image, p1, p2, cv::Scalar(255, 255, 255));
	}
}

bool FacePaintParams::makeTexture()
{
	//if (textureId == 0)
	//{
		cv::Mat orgtexture = cv::imread(texture);
		int rows = orgtexture.rows;
		int cols = orgtexture.cols;
	
		// make delaunay
		texture_points.clear();
		triangles.clear();

		const int nbPoints = 60;
		for (int i = 0;i < nbPoints;i++)
		{
			float rx = new_texCoordInitTable[i * 2];
			float ry = new_texCoordInitTable[i * 2 + 1];
			cv::Point p(rx*cols, (ry + 1.f)*rows);
			texture_points.push_back(p);
		}

		// extent top portion above eyebrow
		{
			cv::Point2f chinPoint = texture_points[8];
			cv::Point2f topPoint = PointCalcUtil::crossPointTwoLines(chinPoint, texture_points[27], texture_points[20], texture_points[23]);

			cv::Point2f verticalOffset = cv::Point2f(texture_points[27]) - chinPoint;
			verticalOffset *= 0.2;
			for (int i = 0; i < nbPoints; i++)
			{
				cv::Point2f p = texture_points[i];
				if (i == 17 || i == 26)
					p = p + verticalOffset * 0.75;
				else if (i == 18 || i == 19 || i == 24 || i == 25)
					p = p + verticalOffset * 0.75;
				else if (i == 20 || i == 21 || i == 22 || i == 23)
					p = p + verticalOffset * 0.75;
				else
					continue;
				texture_points[i] = p;
			}
		}

		{
			// make blurred mask
			cv::Mat bluringmask = cv::Mat::zeros(rows, cols, CV_8UC1);
			const int boundary_idx[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 24, 23, 20, 19, 0 };
			const int lefteye_boundary_idx[] = { 36, 37, 38, 39, 40, 41, 36 };
			const int righteye_boundary_idx[] = { 42, 43, 44, 45, 46, 47, 42 };
			const int mouth_boundary_idx[] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 48 };
			drawline(bluringmask, boundary_idx, sizeof(boundary_idx) / sizeof(boundary_idx[0]) - 1);
			drawline(bluringmask, lefteye_boundary_idx, sizeof(lefteye_boundary_idx) / sizeof(lefteye_boundary_idx[0]) - 1);
			drawline(bluringmask, righteye_boundary_idx, sizeof(righteye_boundary_idx) / sizeof(righteye_boundary_idx[0]) - 1);
			drawline(bluringmask, mouth_boundary_idx, sizeof(mouth_boundary_idx) / sizeof(mouth_boundary_idx[0]) - 1);

			cv::Point seed = texture_points[30];

			cv::floodFill(bluringmask, seed, cv::Scalar(255, 255, 255, 255));
			cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(21, 21), cv::Point(11, 11));
			cv::erode(bluringmask, bluringmask, element);
			cv::erode(bluringmask, bluringmask, element);
			cv::GaussianBlur(bluringmask, bluringmask, cv::Size(47, 47), 10., 10.);

			gtexture.create(rows, cols, CV_8UC4);
			for (int y = 0; y < bluringmask.rows; y++) {
				for (int x = 0; x < bluringmask.cols; x++) {
					cv::Vec3b org_color = orgtexture.at<cv::Vec3b>(y, x);
					cv::Vec4b texture_color;
					texture_color.val[0] = org_color.val[0];
					texture_color.val[1] = org_color.val[1];
					texture_color.val[2] = org_color.val[2];
					texture_color.val[3] = bluringmask.data[y*rows + x];
					gtexture.at<cv::Vec4b>(y, x) = texture_color;
				}
			}

			del_point2d_t* delpoints = new del_point2d_t[nbPoints];
			int i;

			for (i = 0;i < nbPoints;i++)
			{
				delpoints[i].x = texture_points[i].x;
				delpoints[i].y = texture_points[i].y;
			}

			// make face triangles using delaunay 
			delaunay2d_t* pdelaunay = delaunay2d_from(delpoints, nbPoints);
			tri_delaunay2d_t* ptri = tri_delaunay2d_from(pdelaunay);

			for (i = 0; i < (int)ptri->num_triangles; i++)
			{
				int idx1 = ptri->tris[i * 3];
				int idx2 = ptri->tris[i * 3 + 1];
				int idx3 = ptri->tris[i * 3 + 2];
				bool flefteye = (idx1 >= 36 && idx1 < 42) && (idx2 >= 36 && idx2 < 42) && (idx3 >= 36 && idx3 < 42);
				bool frighteye = (idx1 >= 42 && idx1 < 48) && (idx2 >= 42 && idx2 < 48) && (idx3 >= 42 && idx3 < 48);
				bool fmouth = (idx1 >= 48 && idx1 < 60) && (idx2 >= 48 && idx2 < 60) && (idx3 >= 48 && idx3 < 60);
				if (flefteye || frighteye || fmouth)
					continue;

				triangles.push_back(idx1);
				triangles.push_back(idx2);
				triangles.push_back(idx3);
			}

			tri_delaunay2d_release(ptri);
			delaunay2d_release(pdelaunay);
			delete[]delpoints;
		}

		textureId = resourceManager.loadTexture(texture).ID;
	//}

	return true;
}

void FacePaintParams::load()
{
	makeTexture();
}

const std::vector<std::vector<std::vector<int>>> FacePaint::indexes =
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

void FacePaint::load()
{
	glGenBuffers(1, &leftVBO);
	glGenBuffers(1, &leftTexCoordVBO);

	const int total = 60;
	static std::array<cv::Point2f, total> texCoordTable;

	int rows = gtexture.rows;
	int cols = gtexture.cols;
	for (int i = 0; i < total; i++) {
		cv::Point2f p((float)texture_points[i].x / (float)cols, texture_points[i].y / (float)rows);
		texCoordTable[i] = p;
	}

	glBindBuffer(GL_ARRAY_BUFFER, leftTexCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 2, texCoordTable.data(), GL_STATIC_DRAW);
}

void FacePaint::transform(FXModel& face, FacePaintParams &params)
{
	std::array<float, 66 * 2> smoothedPoints;

	for (int i = 0; i < 66; ++i)
	{
		smoothedPoints[i * 2] = facePointsSmoothers[face.pointId][i * 2].responsiveAnalogReadSimple(face.pts[i * 2]);
		smoothedPoints[i * 2 + 1] = facePointsSmoothers[face.pointId][i * 2 + 1].responsiveAnalogReadSimple(face.pts[i * 2 + 1]);
	}

	ImagePoints points(smoothedPoints);

	cv::Point2f chinPoint = points.at(9);
	cv::Point2f topPoint = PointCalcUtil::crossPointTwoLines(chinPoint, points.at(28), points.at(21), points.at(24));

	cv::Point2f verticalOffset = points.at(28) - chinPoint;
	verticalOffset *= 0.2;

	//points.at(1), points.at(18) + verticalOffset * 0.25, points.at(18), points.at(19) + verticalOffset * 0.5, points.at(19),
	//	points.at(21) + verticalOffset * 0.75, points.at(21), topPoint + verticalOffset, topPoint

	const int total = 60;
	std::vector<cv::Point3f> facial_points;
	for (int i = 0; i < 60; i++)
	{
		cv::Point2f p = points.at(i + 1);
		if (i == 17 || i == 26)
			p = p + verticalOffset * 0.25;
		else if (i == 18 || i == 19 || i == 24 || i == 25)
			p = p + verticalOffset * 0.5;
		else if (i == 20 || i == 21 || i == 22 || i == 23)
			p = p + verticalOffset * 0.75;
		facial_points.push_back(cv::Point3f(p.x, p.y, 0));
	}

	glBindBuffer(GL_ARRAY_BUFFER, leftVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 3, facial_points.data(), GL_STREAM_DRAW);

}

void FacePaint::draw(FacePaintParams &params, std::shared_ptr<cwc::glShader> shader)
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

	{
		shader->setUniform1i("hasTexture", 1);

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, leftVBO));
		VertexAttribSetter vTexCoord;
		vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, leftTexCoordVBO));

		glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
		assert(glGetError() == GL_NO_ERROR);
	}
	
	glPopAttrib();
}

void FacePaint::unload()
{
	glDeleteBuffers(1, &leftVBO);

	glDeleteBuffers(1, &leftTexCoordVBO);

	glDeleteBuffers(1, &textureid);
	textureid = 0;
}

//================================================================================
//================================================================================
//================================================================================

OpenGLFacePaintModel2::OpenGLFacePaintModel2()
{
}

void OpenGLFacePaintModel2::onInputFrameResized()
{
	OpenGLModel::onInputFrameResized();

	for (auto &face : facepaint.facePointsSmoothers)
	{
		for (auto &smoother : face)
		{
			smoother.SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		}
	}
}

void OpenGLFacePaintModel2::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);
}

void OpenGLFacePaintModel2::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately)
{
	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	auto renderParamsID = suit.get_optional<size_t>("renderParamsID");
	if (renderParamsID)
	{
		objectRenderParams[faceIndex].clear();
		objectRenderParams[faceIndex].push_back(commonRenderParams[renderParamsID.get()]);
	}

	faceParams[faceIndex].applySuit(suit, loadTexturesImmediately);
}

bool OpenGLFacePaintModel2::load()
{
	for (auto &params : faceParams)
	{
		params.load();
		break;
	}

	facepaint.load();

	return true;
}

void OpenGLFacePaintModel2::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double width = externalRenderParams.ULSWidth;
	double height = externalRenderParams.ULSHeight;
	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	double h = 30;

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

	double planeH = 2.f * h * tan(vAngle / 2);
	double planeW = planeH * aspect;

	const double faceWidthToDepth = 1.0;
	double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
	double faceDepth = faceWidthToDepth * faceWidth;

	Matrix4d modelMatrix (Affine3d(Translation3d(Vector3d(0, 0, -h - faceDepth))).matrix());

	depth = -h - faceDepth;

	renderParams.modelView = modelMatrix.cast<float>();
	renderParams.rotationMatrix = Matrix3f::Identity();
	renderParams.yawMatrix = Matrix3f::Identity();

	facepaint.transform(face, faceParams[face.pointId]);
}

void OpenGLFacePaintModel2::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	//if (textureid == 0)
	//{
		ResourceManager::TextureLoadingResult result = resourceManager.loadTexture(std::string("Texture"), gtexture);

		textureid = result.ID;
	//}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid);
	facepaint.textureid = textureid;

	SetUniformsForObject(*shader, 0, face.pointId);

	facepaint.draw(faceParams[face.pointId], shader);
}

void OpenGLFacePaintModel2::unload()
{
	facepaint.unload();
}