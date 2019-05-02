#include "fx/FxFaceSwapModel.h"

const std::string FaceSwapModel::TYPE_NAME = "FaceSwapModel";

std::string FaceSwapModel::getTypeName() {
	return TYPE_NAME;
}

cv::Point2i FaceSwapModel::getPoint(int i, const std::array<float, 66 * 2>& pts) {
	return cv::Point2i(pts[(i - 1) * 2], pts[(i - 1) * 2 + 1]);
}

FaceSwapModel::FaceSwapModel() {

}


void FaceSwapModel::loadFromJSON(boost::property_tree::ptree& modelRecord) {
	
	OpenGLModel::loadFromJSON(modelRecord);
	
}


void FaceSwapModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams, bool loadTexturesImmediately) {

	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	if (!visible[faceIndex])
	{
		return;
	}


	QString path = QString::fromStdString(suit.get<std::string>("face", ""));
	QString path2 = QString::fromStdString(suit.get<std::string>("left_display", ""));

	leftDisplay.release();

	cv::Mat src = cv::imread(path2.toStdString());

	cv::Size size(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);
	resize(src, leftDisplay, size);

	selection(path, faceIndex);

	
}

void FaceSwapModel::selection(QString absolutePath, int index) {
	int tryCounter = 0;

	img1.release();

	if (index > -1) {
		if (absolutePath.contains("_icon")) {
			absolutePath = absolutePath.replace("_icon", "");
		}
	}

	img1 = cv::imread(absolutePath.toStdString());

	faceTracker.reset();
	//faceTracker.resetHard();

	float trackerValues3[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES];
	const float* rawPoints3[FaceTracker::MAX_TO_TRACK];

	const float* confidence[FaceTracker::MAX_TO_TRACK];

	bool pointsAreReal3 = false;
	while (!pointsAreReal3) {
		faceTracker.trackerValuesFromMat(img1, rawPoints3, trackerValues3, confidence);


		if (rawPoints3[0] != NULL) {
			//pointsAreReal3 = true;
		}
		tryCounter++;

		//if (tryCounter > 200) {
		if (tryCounter > 40) {
			pointsAreReal3 = true;
			//selection(currentPath, index);
			if (rawPoints3[0] == NULL) {
				return;
			}
		}
	}

	//faceTracker.resetHard();
	faceTracker.reset();

	currentPath = absolutePath;

	std::array<float, 66 * 2> pts3;

	std::copy(&rawPoints3[0][0], &rawPoints3[0][66 * 2], &pts3[0]);

	points_ann[0] = getPoint(1, pts3);
	points_ann[1] = getPoint(4, pts3);
	points_ann[2] = getPoint(7, pts3);
	points_ann[3] = getPoint(9, pts3);
	points_ann[4] = getPoint(12, pts3);
	points_ann[5] = getPoint(15, pts3);
	points_ann[6] = getPoint(17, pts3);

	cv::Point2i nose_length = getPoint(28, pts3) - getPoint(29, pts3);
	points_ann[7] = getPoint(27, pts3) + nose_length;
	points_ann[8] = getPoint(18, pts3) + nose_length;

	affine_transform_keypoints_ann[0] = points_ann[3];
	affine_transform_keypoints_ann[1] = getPoint(37, pts3);
	affine_transform_keypoints_ann[2] = getPoint(46, pts3);

	player_size = cv::Size(img1.cols, img1.rows);

	feather_amount.width = feather_amount.height = (int)cv::norm(points_ann[0] - points_ann[6]) / 8;
	mask_ann.create(player_size, CV_8UC1);
	mask_ann.setTo(cv::Scalar::all(0));
	cv::fillConvexPoly(mask_ann, points_ann, 9, cv::Scalar(255));
	img1.copyTo(face_ann, mask_ann);

	playerFaceWidth = PointCalcUtil::distanceBetween(points_ann[0], points_ann[6]);

	faceTracker.reset();
}

bool FaceSwapModel::load() {
	objects.push_back(ModelLoader::CreateQuadModel());

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		//auto loadingResult = resourceManager.loadTexture(emojisTextures[i]);
		auto loadingResult = resourceManager.loadTexture(faceSwapTextures[i]);
		//emojisTexturesIDs[i] = loadingResult.ID;
		faceSwapTexturesIDs[i] = loadingResult.ID;
		//textureSize[i] = { loadingResult.textureWidth, loadingResult.textureHeight };
		textureSize[i] = { loadingResult.textureWidth, loadingResult.textureHeight };
	}

	return true;
}

void FaceSwapModel::drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) {
	const std::array<float, 66 * 2>&  pts = fxModel.pts;

	//cv::Mat playerClone;

	// Getting Frame Size
	frame_size = cv::Size(frame.cols, frame.rows);

	//separate Out Important Points
	points_faces[fxModel.pointId][0] = getPoint(1, pts);
	points_faces[fxModel.pointId][1] = getPoint(4, pts);
	points_faces[fxModel.pointId][2] = getPoint(7, pts);
	points_faces[fxModel.pointId][3] = getPoint(9, pts);
	points_faces[fxModel.pointId][4] = getPoint(12, pts);
	points_faces[fxModel.pointId][5] = getPoint(15, pts);
	points_faces[fxModel.pointId][6] = getPoint(17, pts);
	cv::Point2i nose_length = getPoint(28, pts) - getPoint(29, pts);
	points_faces[fxModel.pointId][7] = getPoint(27, pts) + nose_length;
	points_faces[fxModel.pointId][8] = getPoint(18, pts) + nose_length;
	affine_transform_keypoints_faces[fxModel.pointId][0] = points_faces[fxModel.pointId][3];
	affine_transform_keypoints_faces[fxModel.pointId][1] = getPoint(37, pts);
	affine_transform_keypoints_faces[fxModel.pointId][2] = getPoint(46, pts);

	cv::Point2i box_left_corner = points_faces[fxModel.pointId][8];
	int faceWidth = PointCalcUtil::distanceBetween(points_faces[fxModel.pointId][0], points_faces[fxModel.pointId][6]);

	float percentToAdjust = (float)((float)faceWidth / (float)playerFaceWidth);

	int faceHeight = PointCalcUtil::distanceBetween(box_left_corner, points_faces[fxModel.pointId][3]);// ((int)fxModel.faceWidthRaw * 1.5) + extraSpace;

	static const int halfDim = 30;
	cv::Rect bounding_rect(box_left_corner.x - halfDim, box_left_corner.y - halfDim, faceWidth + halfDim, faceHeight + halfDim);

	int sampleWidth = 200;//;
	cv::Rect samplingRect_rect(fxModel.xFaceCenterRaw - (sampleWidth / 2), fxModel.yFaceCenterRaw - (sampleWidth / 2), sampleWidth, sampleWidth);

	// Calculates transformation matrices based on points extracted by getFacePoints
	trans_ann_to_face[fxModel.pointId] = cv::getAffineTransform(affine_transform_keypoints_ann, affine_transform_keypoints_faces[fxModel.pointId]);

	// Creates masks for faces based on the points extracted in getFacePoints
	mask_face[fxModel.pointId].create(frame_size, CV_8UC1);

	mask_face[fxModel.pointId].setTo(cv::Scalar::all(0));
	cv::fillConvexPoly(mask_face[fxModel.pointId], points_faces[fxModel.pointId], 9, cv::Scalar(255));

	try
	{
		// Creates warpped masks out of masks created in getMasks to switch places
		cv::warpAffine(mask_ann, warpped_mask_ann, trans_ann_to_face[fxModel.pointId], frame_size, cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));

		cv::bitwise_and(mask_face[fxModel.pointId], warpped_mask_ann, refined_face_and_ann_warpped[fxModel.pointId]);

		// Returns Mat of refined mask such that warpped mask isn't bigger than original mask
		cv::Mat refined_masks_temp(frame_size, CV_8UC1, cv::Scalar(0));
		refined_face_and_ann_warpped[fxModel.pointId].copyTo(refined_masks_temp, refined_face_and_ann_warpped[fxModel.pointId]);
		refined_masks = refined_masks_temp;

		// Creates warpped faces out of faces extracted in extractFaces
		cv::Mat warpped_facesTemp(frame_size, CV_8UC3, cv::Scalar::all(0));
		cv::warpAffine(face_ann, warpped_face_ann, trans_ann_to_face[fxModel.pointId], frame_size, cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
		warpped_face_ann.copyTo(warpped_facesTemp, warpped_mask_ann);
		warpped_faces = warpped_facesTemp;
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		std::cout << "exception caught: " << err_msg << std::endl;
	}
	try
	{
		cv::Mat one = frame(bounding_rect);
		cv::Mat two = warpped_faces(bounding_rect);
		cv::Mat three = warpped_mask_ann(bounding_rect);
		specifiyHistogram(one, two, three);

		// Blurs edges of mask
		auto refined_mask_face = refined_masks(bounding_rect);
		featherMask(refined_mask_face);

		// Pastes faces on original frame
		pasteFacesOnFrame(frame);
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		std::cout << "exception caught: " << err_msg << std::endl;
	}

	cv::Mat imageKissInside = leftDisplay;

	for (int r = 0; r < imageKissInside.rows; r++)
	{
		for (int c = 0; c < imageKissInside.cols; c++)
		{
			int yPos = c;
			int xPos = r;

			if (xPos <= 0 || (xPos) >= frame.rows - 0) {
				continue;
			}

			if (yPos <= 0 || yPos >= frame.cols - 0) {
				continue;
			}

			cv::Vec3b bgrPixel = imageKissInside.at<cv::Vec3b>(r, c);

			if (bgrPixel[0] == 0 && bgrPixel[1] > 250 && bgrPixel[2] == 0) {
				continue;
			}

			try
			{
				frame.at<cv::Vec3b>(xPos, yPos) = bgrPixel;
			}
			catch (cv::Exception& e)
			{
				continue;
			}
		}
	}
}


void FaceSwapModel::unload() {
	OpenGLModel::unload();

	for (auto &IDs : texturesIDs)
	{
		IDs.clear();
	}
}

void FaceSwapModel::featherMask(cv::Mat &refined_masks)
{
	cv::erode(refined_masks, refined_masks, getStructuringElement(cv::MORPH_RECT, feather_amount), cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	cv::blur(refined_masks, refined_masks, feather_amount, cv::Point(-1, -1), cv::BORDER_CONSTANT);
}

inline void FaceSwapModel::pasteFacesOnFrame(cv::Mat frameVal)
{
	for (size_t i = 0; i < frameVal.rows; i++)
	{
		auto frame_pixel = frameVal.row(i).data;
		auto faces_pixel = warpped_faces.row(i).data;
		auto masks_pixel = refined_masks.row(i).data;

		for (size_t j = 0; j < frameVal.cols; j++)
		{
			if (*masks_pixel != 0)
			{
				*frame_pixel = ((255 - *masks_pixel) * (*frame_pixel) + (*masks_pixel) * (*faces_pixel)) >> 8; // divide by 256
				*(frame_pixel + 1) = ((255 - *(masks_pixel + 1)) * (*(frame_pixel + 1)) + (*(masks_pixel + 1)) * (*(faces_pixel + 1))) >> 8;
				*(frame_pixel + 2) = ((255 - *(masks_pixel + 2)) * (*(frame_pixel + 2)) + (*(masks_pixel + 2)) * (*(faces_pixel + 2))) >> 8;
			}

			frame_pixel += 3;
			faces_pixel += 3;
			masks_pixel++;
		}
	}
}

void FaceSwapModel::specifiyHistogram(const cv::Mat source_image, cv::Mat target_image, cv::Mat mask)
{
	uint8_t LUT[3][256];
	int source_hist_int[3][256];
	int target_hist_int[3][256];
	float source_histogram[3][256];
	float target_histogram[3][256];

	std::memset(source_hist_int, 0, sizeof(int) * 3 * 256);
	std::memset(target_hist_int, 0, sizeof(int) * 3 * 256);

	for (size_t i = 0; i < mask.rows; i++)
	{
		auto current_mask_pixel = mask.row(i).data;
		auto current_source_pixel = source_image.row(i).data;
		auto current_target_pixel = target_image.row(i).data;

		for (size_t j = 0; j < mask.cols; j++)
		{
			if (*current_mask_pixel != 0) {
				source_hist_int[0][*current_source_pixel]++;
				source_hist_int[1][*(current_source_pixel + 1)]++;
				source_hist_int[2][*(current_source_pixel + 2)]++;

				target_hist_int[0][*current_target_pixel]++;
				target_hist_int[1][*(current_target_pixel + 1)]++;
				target_hist_int[2][*(current_target_pixel + 2)]++;
			}

			// Advance to next pixel
			current_source_pixel += 3;
			current_target_pixel += 3;
			current_mask_pixel++;
		}
	}

	// Calc CDF
	for (size_t i = 1; i < 256; i++)
	{
		source_hist_int[0][i] += source_hist_int[0][i - 1];
		source_hist_int[1][i] += source_hist_int[1][i - 1];
		source_hist_int[2][i] += source_hist_int[2][i - 1];

		target_hist_int[0][i] += target_hist_int[0][i - 1];
		target_hist_int[1][i] += target_hist_int[1][i - 1];
		target_hist_int[2][i] += target_hist_int[2][i - 1];
	}

	// Normalize CDF
	for (size_t i = 0; i < 256; i++)
	{
		source_histogram[0][i] = (source_hist_int[0][255] ? (float)source_hist_int[0][i] / source_hist_int[0][255] : 0);
		source_histogram[1][i] = (source_hist_int[1][255] ? (float)source_hist_int[1][i] / source_hist_int[1][255] : 0);
		source_histogram[2][i] = (source_hist_int[2][255] ? (float)source_hist_int[2][i] / source_hist_int[2][255] : 0);

		target_histogram[0][i] = (target_hist_int[0][255] ? (float)target_hist_int[0][i] / target_hist_int[0][255] : 0);
		target_histogram[1][i] = (target_hist_int[1][255] ? (float)target_hist_int[1][i] / target_hist_int[1][255] : 0);
		target_histogram[2][i] = (target_hist_int[2][255] ? (float)target_hist_int[2][i] / target_hist_int[2][255] : 0);
	}

	// Create lookup table

	auto binary_search = [&](const float needle, const float haystack[]) -> uint8_t
	{
		uint8_t l = 0, r = 255, m;
		while (l < r)
		{
			m = (l + r) / 2;
			if (needle > haystack[m])
				l = m + 1;
			else
				r = m - 1;
		}
		// TODO check closest value
		return m;
	};

	for (size_t i = 0; i < 256; i++)
	{
		LUT[0][i] = binary_search(target_histogram[0][i], source_histogram[0]);
		LUT[1][i] = binary_search(target_histogram[1][i], source_histogram[1]);
		LUT[2][i] = binary_search(target_histogram[2][i], source_histogram[2]);
	}

	// repaint pixels
	for (size_t i = 0; i < mask.rows; i++)
	{
		auto current_mask_pixel = mask.row(i).data;
		auto current_target_pixel = target_image.row(i).data;
		for (size_t j = 0; j < mask.cols; j++)
		{
			if (*current_mask_pixel != 0)
			{
				*current_target_pixel = LUT[0][*current_target_pixel];
				*(current_target_pixel + 1) = LUT[1][*(current_target_pixel + 1)];
				*(current_target_pixel + 2) = LUT[2][*(current_target_pixel + 2)];
			}

			// Advance to next pixel
			current_target_pixel += 3;
			current_mask_pixel++;
		}
	}
}

void FaceSwapModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams) {

}

void FaceSwapModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams){

}