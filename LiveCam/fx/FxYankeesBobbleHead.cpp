#include <fx/FxYankeesBobbleHead.h>
#include <iostream> 
#include <tracker/FaceTracker.h>

#define _USE_MATH_DEFINES
#include <math.h>

extern FaceTracker faceTracker;

const int pointTotal = 66;

extern GLuint cameraTextureId;


//#define USE_SMALL_POINTS

#ifdef USE_SMALL_POINTS
constexpr size_t facePointCount = 5;
#else
constexpr size_t facePointCount = 23;
#endif


std::array<float, facePointCount * 2> calculatePointPosArr2(const FXModel& face);



FxYankeesBobbleHead::FxYankeesBobbleHead()
{

	auto textureShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");


	//initialize shader
	//regularShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/extendedVertexShader.txt", "./assets/shaders/fragment/special/bobbleHeadHair_FragmentShader.txt"); // load (and compile, link) from file
	//regularShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/extendedVertexShader.txt", "./assets/shaders/fragment/special/bobbleHeadHair_yankees_FragmentShader.txt"); // load (and compile, link) from file
	regularShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/simpleVertexShader.txt", "./assets/shaders/fragment/phongFragmentShader.txt");

	faceMixShader = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/special/bobbleHeadFace_MixVertexShader.txt", "./assets/shaders/fragment/special/bobbleHeadFace_MixFragmentShader.txt"); // load (and compile, link) from file


	animationCounter.fill(0);
	animationDirection.fill(false);

	auto headModel = make_shared<OpenGL3DModel>();
	headModel->modelPath = "./assets/fx/yankees/bobble_head/models/Bobblehead_Head_Object_UVMap_002.obj";
	headModel->name = "head";
	headModel->modelScale = 1.55f;

	auto hairModel = make_shared<OpenGL3DModel>();

	//hairModel->modelPath = "./assets/fx/reds/bobble_head/models/Bobblehead_Hair_Object.obj";
	hairModel->modelPath = "./assets/fx/yankees/bobble_head_yankees/04-NYY_Bobblehead_v02-02-OBJ_nohead.obj";

	hairModel->name = "hair";
	hairModel->modelShift = { 0, 0.36f, 0 };
	hairModel->modelScale = 1.9f;


	auto renderParams = make_shared<ObjectRenderParams>();
	renderParams->ambientLight = { 0.2f, 0.2f, 0.2f };
	renderParams->specularLight = { 0.3f, 0.3f, 0.3f };
	renderParams->cameraPos = { 0.f, 1.f, 1.f, 0.f };
	renderParams->lightPos = { -50.f, 800.f, 400.f, 1.f };

	renderParams->specularPower = 15;

	renderParams->shader = textureShader;
	renderParams->cameraPos = { 0, 0, 0, 1 };
	renderParams->lightPos = { 0.2f, 0.4f, 1.0f, 0 };
	renderParams->ambientLight = { 0.6f, 0.6f, 0.6f };
	renderParams->diffuseLight = { 0.6f, 0.6f, 0.6f };
	renderParams->specularLight = { 0.15f, 0.15f, 0.15f };
	renderParams->specularPower = 15;
	renderParams->cullFace = false;

	auto renderParams2 = make_shared<ObjectRenderParams>();
	renderParams2->shader = regularShader;
	renderParams2->cameraPos = { 0, 0, 0, 1 };
	renderParams2->lightPos = { 0.2f, 1, 0.4f, 0 };
	renderParams2->ambientLight = { 0.6f, 0.6f, 0.6f };
	renderParams2->diffuseLight = { 0.6f, 0.6f, 0.6f };
	renderParams2->specularLight = { 0.0f, 0.0f, 0.0f };
	renderParams2->specularPower = 5;
	
	headModel->objectRenderParams.fill( { renderParams} );
	hairModel->objectRenderParams.fill({ renderParams2 });

	models.push_back(headModel);
	models.push_back(hairModel);


	calcNewFaceCenterAndStd();

}

FxYankeesBobbleHead::~FxYankeesBobbleHead()
{
	if (normalTextureId != 0)
	{
		glDeleteTextures(1, &normalTextureId);
		normalTextureId = 0;
	}
}

void FxYankeesBobbleHead::load()
{
	FxCore::load();


	//std::string hairTexture = "./assets/fx/reds/bobble_head/textures/hair.png";
	std::string hairTexture = "./assets/fx/yankees/bobble_head_yankees/NYY-04_Filter-Material_Bobblehead-v02-02.png";

	std::string normalTexture = "./assets/fx/yankees/bobble_head/textures/hair_normal.png";

	std::string headTexture = "./assets/fx/yankees/bobble_head/textures/BobbleHead_UVMap_Template.jpg";

	hairTextureID = resourceManager.loadTexture(hairTexture).ID;
	normalTextureId = resourceManager.loadTexture(normalTexture).ID;
	headTextureID = resourceManager.loadTexture(headTexture).ID;
}


void FxYankeesBobbleHead::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	animateNextStep(face.pointId);

	models[0]->renderParams.modelView = Affine3f(Translation3f(0, 0.0035*(animationCounter[face.pointId]), 0)).matrix() * models[0]->renderParams.modelView;
	models[1]->renderParams.modelView = Affine3f(Translation3f(0, 0.0035*(animationCounter[face.pointId]), 0)).matrix() * models[1]->renderParams.modelView;

}



void FxYankeesBobbleHead::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	auto faceStd = calcFaceStd(face.pts);

	faceStd.x = faceStd.x / face.fWidth;
	faceStd.y = faceStd.y / face.fHeight;

	cv::Point2f faceCenter;

	faceCenter.x = face.xFaceCenterRaw / face.fWidth;
	faceCenter.y = face.yFaceCenterRaw / face.fHeight;

	std::array<float, facePointCount * 2> pointPosArr = calculatePointPosArr2(face);

	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < 2; ++i)
	{

		const std::vector<DrawObjectsModel>& drawObjects = models[i]->objects;

		bool modelIsHead = (models[i]->name == "head");

		std::shared_ptr<cwc::glShader> shader = modelIsHead ? faceMixShader : regularShader;

		ShaderSetter shaderSetter(shader, *models[i]);

		Eigen::Matrix3f faceTextureRotateMatrix;

		if (modelIsHead)
		{

			float scaleX = faceStd.x / newFaceStd.x;
			float scaleY = faceStd.y / newFaceStd.y;

			faceTextureRotateMatrix = AngleAxisf(-face.roll * M_PI / 180.0, Vector3f(0, 0, 1));

			shader->setUniformMatrix3fv("FaceTextureTransformMatrix", 1, false, faceTextureRotateMatrix.data());

			shader->setUniform2f("FaceTextureScale", scaleX, scaleY);

			shader->setUniform2f("AppliedFaceTextureCenter", faceCenter.x, faceCenter.y);

			shader->setUniform2f("ModelFaceTextureCenter", newFaceCenter.x, newFaceCenter.y);

			shader->setUniform2fv("fp", pointPosArr.size(), &pointPosArr[0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cameraTextureId);

		}
		else
		{
			//glActiveTexture(GL_TEXTURE2);

			glBindTexture(GL_TEXTURE_2D, normalTextureId);

			
			shader->setUniform4fv("cameraPos", 1, &(models[i]->objectRenderParams[face.pointId][0]->cameraPos[0]));
			shader->setUniform4fv("lightPos", 1, &(models[i]->objectRenderParams[face.pointId][0]->lightPos[0]));

			shader->setUniform3fv("ambientLight", 1, &(models[i]->objectRenderParams[face.pointId][0]->ambientLight[0]));
			shader->setUniform3fv("diffuseLight", 1, &(models[i]->objectRenderParams[face.pointId][0]->diffuseLight[0]));
			shader->setUniform3fv("specularLight", 1, &(models[i]->objectRenderParams[face.pointId][0]->specularLight[0]));

			shader->setUniform1f("specularPower", models[i]->objectRenderParams[face.pointId][0]->specularPower);

			shader->setUniform1f("reflectionRatio", models[i]->objectRenderParams[face.pointId][0]->reflectionRatio);

			shader->setUniform4fv("materialColor", 1, &models[i]->objectRenderParams[face.pointId][0]->materialColor[0]);

			glActiveTexture(GL_TEXTURE0); //Always should end with GL_TEXTURE0 -- Vladislav Khorev vladislav.khorev@fishrungames.com

			if (drawObjects.size() > 0)
			{
				glBindTexture(GL_TEXTURE_2D, models[i]->name == "hair" ? hairTextureID : headTextureID);
			}


		}



		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		glDisable(GL_CULL_FACE);

		for (size_t i = 0; i < drawObjects.size(); i++) {
			DrawObjectsModel o = drawObjects[i];
			if (o.vb < 1) {
				continue;
			}

			VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, o.vb));

			VertexAttribSetter vNormal(VertexAttribSetter::NormalAttribSetter(*shader, o.nb));

			VertexAttribSetter vTangent(VertexAttribSetter::TangentAttribSetter(*shader, o.tangentb));

			VertexAttribSetter vBitangent(VertexAttribSetter::BitangentAttribSetter(*shader, o.bitangentb));

			VertexAttribSetter vTexCoord(VertexAttribSetter::TexCoordAttribSetter(*shader, o.tb));

			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);


		}

		glEnable(GL_CULL_FACE);
	}
}


void FxYankeesBobbleHead::animateNextStep(size_t i)
{
	if (animationDirection[i]) {

		animationCounter[i]++;

		if (animationCounter[i] == maxMovementYankees)
		{
			animationDirection[i] = false;
		}
	}
	else {

		animationCounter[i]--;

		if (animationCounter[i] == (-maxMovementYankees))
		{
			animationDirection[i] = true;
		}
	}
}

void FxYankeesBobbleHead::calcNewFaceCenterAndStd()
{
	cv::Mat imgNewPartialFace = cv::imread("./assets/fx/yankees/bobble_head/textures/face.jpg");

	bool isTracking;
	float trackerValues[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES];
	const float* rawPoints[FaceTracker::MAX_TO_TRACK];

	const float* confidence[FaceTracker::MAX_TO_TRACK];

	// Get tracker values from tracker..
	bool pointsAreReal = false;
	while (!pointsAreReal) {
		isTracking = faceTracker.trackerValuesFromMat(imgNewPartialFace, rawPoints, trackerValues, confidence);
		if (rawPoints[0] != NULL) {
			pointsAreReal = true;
		}
	}

	float imgWidth = imgNewPartialFace.cols;
	float imgHeight = imgNewPartialFace.rows;
	float widthDiff = 1.0f;
	float heightDiff = 1.0f;

	int frame_width = imgWidth;
	int frame_height = imgHeight;

	int width = imgWidth;
	int height = imgHeight;

	int id = 0;

	// calculate x and y face center values.
	float xValue = (float)-(1 - ((trackerValues[id][FaceTracker::FACE_CENTER_X] * widthDiff) / (frame_width / 2)));
	float yValue = (float)(1 - ((trackerValues[id][FaceTracker::FACE_CENTER_Y] * heightDiff) / (frame_height / 2)));

	//xValue = (float)counter;

	// calculate x and y face center values.
	float lipsXValue = (float)-(1 - ((trackerValues[id][FaceTracker::TOP_LIP_CENTER_X] * widthDiff) / (frame_width / 2)));
	float lipsYValue = (float)(1 - ((trackerValues[id][FaceTracker::TOP_LIP_CENTER_Y] * heightDiff) / (frame_height / 2)));

	float facewidth = trackerValues[id][FaceTracker::FACE_WIDTH] / frame_width;

	float viewerSize = (facewidth * 2) * widthDiff;

	int pitch = (int)trackerValues[id][FaceTracker::PITCH];
	int yaw = 0 - (int)trackerValues[id][FaceTracker::YAW];
	int roll = 0 - (int)trackerValues[id][FaceTracker::ROLL];

	FXModel newFaceFxModel;
	std::array<float, 66 * 2>  newFaceFxModelPts;

	std::copy(rawPoints[0], rawPoints[0] + pointTotal * 2, newFaceFxModelPts.begin());

	newFaceFxModel.pointId = id;
	newFaceFxModel.pointTotal = trackerValues[id][FaceTracker::POINT_TOTAL];

	//newFaceFxModel.pts = rawPoints[id];

	newFaceFxModel.faceWidth = facewidth;
	newFaceFxModel.widthDiff = widthDiff;
	newFaceFxModel.faceWidthRaw = trackerValues[id][FaceTracker::FACE_WIDTH];
	newFaceFxModel.xFaceCenter = xValue;
	newFaceFxModel.yFaceCenter = yValue;
	newFaceFxModel.xTopLipCenter = lipsXValue;
	newFaceFxModel.yTopLipCenter = lipsYValue;
	newFaceFxModel.xFaceCenterRaw = trackerValues[id][FaceTracker::FACE_CENTER_X];
	newFaceFxModel.yFaceCenterRaw = trackerValues[id][FaceTracker::FACE_CENTER_Y];
	newFaceFxModel.pitch = pitch;
	newFaceFxModel.roll = roll;
	newFaceFxModel.yaw = yaw;
	newFaceFxModel.viewerSize = viewerSize;
	newFaceFxModel.gazeLX = trackerValues[id][FaceTracker::LEFT_GAZE_X];
	newFaceFxModel.gazeLY = trackerValues[id][FaceTracker::LEFT_GAZE_Y];
	newFaceFxModel.gazeRX = trackerValues[id][FaceTracker::RIGHT_GAZE_X];
	newFaceFxModel.gazeRY = trackerValues[id][FaceTracker::RIGHT_GAZE_Y];
	newFaceFxModel.gazeRZ = trackerValues[id][FaceTracker::RIGHT_GAZE_Z];
	newFaceFxModel.gazeLZ = trackerValues[id][FaceTracker::LEFT_GAZE_Z];
	newFaceFxModel.pupilLX = trackerValues[id][FaceTracker::LEFT_PUPIL_X];
	newFaceFxModel.pupilLY = trackerValues[id][FaceTracker::LEFT_PUPIL_Y];
	newFaceFxModel.pupilRX = trackerValues[id][FaceTracker::RIGHT_PUPIL_X];
	newFaceFxModel.pupilRY = trackerValues[id][FaceTracker::RIGHT_PUPIL_Y];
	newFaceFxModel.fWidth = imgNewPartialFace.cols;
	newFaceFxModel.fHeight = imgNewPartialFace.rows;
	newFaceFxModel.width = width;
	newFaceFxModel.height = height;

	newFaceStd = calcFaceStd(newFaceFxModelPts);

	newFaceStd.x = newFaceStd.x / 1024.f;
	newFaceStd.y = newFaceStd.y / 1024.f;

	newFaceCenter.x = newFaceFxModel.xFaceCenterRaw / 1024.f;
	newFaceCenter.y = newFaceFxModel.yFaceCenterRaw / 1024.f;
}


cv::Point2f FxYankeesBobbleHead::calcFaceStd(const std::array<float, 66 * 2>& pts)
{
	float stdFaceX = 0;
	float stdFaceY = 0;

	float meanFaceX = 0;
	float meanFaceY = 0;


	for (int i = 0; i < pointTotal; i++)
	{
		meanFaceX += pts[2 * i];
		meanFaceY += pts[2 * i + 1];
	}

	meanFaceX /= pointTotal;
	meanFaceY /= pointTotal;

	for (int i = 0; i < pointTotal; i++)
	{
		stdFaceX += (pts[2 * i] - meanFaceX) * (pts[2 * i] - meanFaceX);
		stdFaceY += (pts[2 * i + 1] - meanFaceY) * (pts[2 * i + 1] - meanFaceY);

	}

	stdFaceX = std::sqrt(stdFaceX / pointTotal);
	stdFaceY = std::sqrt(stdFaceY / pointTotal);


	cv::Point2f result;

	result.x = stdFaceX;
	result.y = stdFaceY;

	return result;
}




std::array<float, facePointCount * 2> calculatePointPosArr2(const FXModel& face)
{
	cv::Point2f faceCenter;
	faceCenter.x = face.xFaceCenterRaw / face.fWidth;
	faceCenter.y = face.yFaceCenterRaw / face.fHeight;


	constexpr float specialUpperScaleX = 1.05f;
	constexpr float specialUpperScaleY = 1.15f;


#ifdef USE_SMALL_POINTS

	std::array<float, facePointCount * 2> smallPointPosArr{
		face.pts[0], face.pts[1],
		face.pts[8 * 2], face.pts[8 * 2 + 1],
		face.pts[16 * 2], face.pts[16 * 2 + 1],
		face.pts[24 * 2], face.pts[24 * 2 + 1],
		face.pts[19 * 2], face.pts[19 * 2 + 1]
	};

	for (size_t i = 0; i < smallPointPosArr.size() / 2; i++)
	{
		smallPointPosArr[2 * i] = smallPointPosArr[2 * i] / face.fWidth - faceCenter.x;
		smallPointPosArr[2 * i + 1] = smallPointPosArr[2 * i + 1] / face.fHeight - faceCenter.y;
	}

	smallPointPosArr[6] = smallPointPosArr[6] * specialUpperScaleX;
	smallPointPosArr[7] = smallPointPosArr[7] * specialUpperScaleY;
	smallPointPosArr[8] = smallPointPosArr[8] * specialUpperScaleX;
	smallPointPosArr[9] = smallPointPosArr[9] * specialUpperScaleY;

	return smallPointPosArr;

#else

	std::array<float, facePointCount * 2> pointPosArr{

		face.pts[0], face.pts[1],
		face.pts[2], face.pts[3],
		face.pts[4], face.pts[5],
		face.pts[6], face.pts[7],
		face.pts[8], face.pts[9],
		face.pts[10], face.pts[11],
		face.pts[12], face.pts[13],
		face.pts[14], face.pts[15],
		face.pts[16], face.pts[17],
		face.pts[18], face.pts[19],
		face.pts[20], face.pts[21],
		face.pts[22], face.pts[23],
		face.pts[24], face.pts[25],
		face.pts[26], face.pts[27],
		face.pts[28], face.pts[29],
		face.pts[30], face.pts[31],
		face.pts[32], face.pts[33],

		face.pts[26 * 2], face.pts[26 * 2 + 1],
		face.pts[25 * 2], face.pts[25 * 2 + 1],
		face.pts[24 * 2], face.pts[24 * 2 + 1] ,

		face.pts[19 * 2], face.pts[19 * 2 + 1] ,
		face.pts[18 * 2] , face.pts[18 * 2 + 1],
		face.pts[17 * 2], face.pts[17 * 2 + 1]
	};

	for (size_t i = 0; i < pointPosArr.size() / 2; i++)
	{
		pointPosArr[2 * i] = pointPosArr[2 * i] / face.fWidth - faceCenter.x;
		pointPosArr[2 * i + 1] = pointPosArr[2 * i + 1] / face.fHeight - faceCenter.y;
	}

	pointPosArr[pointPosArr.size() - 1 - 11] = pointPosArr[pointPosArr.size() - 1 - 11] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1 - 10] = pointPosArr[pointPosArr.size() - 1 - 10] * specialUpperScaleY;

	pointPosArr[pointPosArr.size() - 1 - 9] = pointPosArr[pointPosArr.size() - 1 - 9] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1 - 8] = pointPosArr[pointPosArr.size() - 1 - 8] * specialUpperScaleY;

	pointPosArr[pointPosArr.size() - 1 - 7] = pointPosArr[pointPosArr.size() - 1 - 7] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1 - 6] = pointPosArr[pointPosArr.size() - 1 - 6] * specialUpperScaleY;

	pointPosArr[pointPosArr.size() - 1 - 5] = pointPosArr[pointPosArr.size() - 1 - 5] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1 - 4] = pointPosArr[pointPosArr.size() - 1 - 4] * specialUpperScaleY;

	pointPosArr[pointPosArr.size() - 1 - 3] = pointPosArr[pointPosArr.size() - 1 - 3] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1 - 2] = pointPosArr[pointPosArr.size() - 1 - 2] * specialUpperScaleY;

	pointPosArr[pointPosArr.size() - 1 - 1] = pointPosArr[pointPosArr.size() - 1 - 1] * specialUpperScaleX;
	pointPosArr[pointPosArr.size() - 1] = pointPosArr[pointPosArr.size() - 1] * specialUpperScaleY;

	return pointPosArr;
#endif


}