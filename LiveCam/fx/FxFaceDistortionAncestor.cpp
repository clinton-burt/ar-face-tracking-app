#include "fx/FxFaceDistortionAncestor.h"

//================================================================================
//================================================================================
//================================================================================

FaceDistortionParams::FaceDistortionParams() { }

void FaceDistortionParams::applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately)
{
	/*texture = suit.get<std::string>("Texture", "");
	if (loadTexturesImmediately)
	{
		textureId = resourceManager.loadTexture(texture).ID;
	}*/
}

static cv::Mat gtexture;
static std::vector<int> triangles;
static std::vector<cv::Point> texture_points;

void FaceDistortionParams::load()
{
}

void FaceDistortion::load()
{
/*	glGenBuffers(1, &vbo);
	glGenBuffers(1, &texCoordVBO);

	const int total = 60;
	static std::array<cv::Point2f, total> texCoordTable;

	int rows = gtexture.rows;
	int cols = gtexture.cols;
	for (int i = 0; i < total; i++) {
		cv::Point2f p((float)texture_points[i].x / (float)cols, texture_points[i].y / (float)rows);
		texCoordTable[i] = p;
	}

	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 2, texCoordTable.data(), GL_STATIC_DRAW);*/
}

void FaceDistortion::transform(ImagePoints &points, FaceDistortionParams &params)
{
/*	cv::Point2f chinPoint = points.at(9);
	cv::Point2f topPoint = PointCalcUtil::crossPointTwoLines(chinPoint, points.at(28), points.at(21), points.at(24));

	cv::Point2f verticalOffset = points.at(28) - chinPoint;
	verticalOffset *= 0.2;

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

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * total * 3, facial_points.data(), GL_STREAM_DRAW);*/

}

void FaceDistortion::draw(FaceDistortionParams &params, std::shared_ptr<cwc::glShader> shader)
{
/*	glPushAttrib(GL_ALL_ATTRIB_BITS);

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

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, vbo));
		VertexAttribSetter vTexCoord;
		vTexCoord = VertexAttribSetter(VertexAttribSetter::TexCoordAttribSetter(*shader, texCoordVBO));

		glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, &triangles[0]);
		assert(glGetError() == GL_NO_ERROR);
	}

	glPopAttrib();*/
}

void FaceDistortion::unload()
{
	glDeleteBuffers(1, &vbo);

	glDeleteBuffers(1, &texCoordVBO);

	glDeleteBuffers(1, &textureid);
	textureid = 0;
}

//================================================================================
//================================================================================
//================================================================================

OpenGLFaceDistortionModel::OpenGLFaceDistortionModel()
{
}

void OpenGLFaceDistortionModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);
}

void OpenGLFaceDistortionModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
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

	faceParams[faceIndex].applySuit(suit, loadTexturesImmediately);
}

bool OpenGLFaceDistortionModel::load()
{
	for (auto &params : faceParams)
	{
		params.load();
		break;
	}

	facedistortion.load();

	return true;
}

void OpenGLFaceDistortionModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
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

	Matrix4d modelMatrix(Affine3d(Translation3d(Vector3d(0, 0, -h - faceDepth))).matrix());

	depth = -h - faceDepth;

	renderParams.modelView = modelMatrix.cast<float>();
	renderParams.rotationMatrix = Matrix3f::Identity();
	renderParams.yawMatrix = Matrix3f::Identity();

	facedistortion.transform(ImagePoints(face.pts), faceParams[face.pointId]);
}

void OpenGLFaceDistortionModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	auto shader = objectRenderParams[face.pointId][0]->shader;

	ShaderSetter shaderSetter(shader, *this);

	if (textureid == 0)
	{
		ResourceManager::TextureLoadingResult result = resourceManager.loadTexture(std::string("Texture"), gtexture);

		textureid = result.ID;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid);
	facedistortion.textureid = textureid;

	SetUniformsForObject(*shader, 0, face.pointId);

	facedistortion.draw(faceParams[face.pointId], shader);
}

void OpenGLFaceDistortionModel::unload()
{
	facedistortion.unload();
}