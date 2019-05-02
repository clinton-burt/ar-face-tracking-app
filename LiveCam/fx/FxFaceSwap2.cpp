#include <fx/FxFaceSwap2.h>
#include <utils/DrawUtil.h>
#include <iostream>
#include "utils/Resolutions.h"

extern FaceTracker faceTracker;

FxFaceSwap2::FxFaceSwap2()
{
	//Do this in subclass, not in FxFaceSwap2 class constructor
	//loadFromJSON("./assets/fx/reds/reds_face_swap_modules.json");
	//loadFromJSON("./assets/fx/vikings/vikings-face-swap-modules.json");
	//loadFromJSON("./assets/fx/chargers/face-swap-modules.json");
	

}

FxFaceSwap2::~FxFaceSwap2()
{
}

cv::Point2i FxFaceSwap2::getPoint(int i, const std::array<float, 66 * 2>& pts)
{
	return cv::Point2i(pts[(i - 1) * 2], pts[(i - 1) * 2 + 1]);
}

void FxFaceSwap2::applyModule(FilterModule *module, size_t faceIndex, bool loadTExturesImmediately)
{
	QString path = QString::fromStdString(module->suits[0].get<std::string>("face", ""));
	QString path2 = QString::fromStdString(module->suits[0].get<std::string>("left_display", ""));
	//QString path2 = "./assets/fx/miamiHeat/faceswap/noframe.png";

	leftDisplay.release();

	//leftDisplay = cv::imread(path2.toStdString());
    cv::Mat src = cv::imread(path2.toStdString());
	//cv::Mat src = cv::Mat(cv::Size(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT), CV_8UC3, {0, 255, 0});
	cv::Size size(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);
	resize(src, leftDisplay, size);

	selection(path, faceIndex);
}

void FxFaceSwap2::selection(QString absolutePath, int index)
{
	fs::path imagePath = absolutePath.toStdString();
	fs::path jsonPath = imagePath.parent_path() / (imagePath.stem().string() + ".json");

	if (!fs::exists(imagePath) || !fs::exists(jsonPath))
	{
		return;
	}

	currentPath = absolutePath;

	img1 = cv::imread(imagePath.string());


	boost::property_tree::ptree face;
	boost::property_tree::json_parser::read_json(jsonPath.string(), face);

	Eigen::Vector2f tmp;
	
	tmp = JSONVectorReader::readVector2f(face.get_child("0"));
	points_ann[0] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("3"));
	points_ann[1] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("6"));
	points_ann[2] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("8"));
	points_ann[3] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("11"));
	points_ann[4] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("14"));
	points_ann[5] = { (int)tmp[0], (int)tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("16"));
	points_ann[6] = { (int)tmp[0], (int)tmp[1] };

	Eigen::Vector2f nose_length = JSONVectorReader::readVector2f(face.get_child("27")) - JSONVectorReader::readVector2f(face.get_child("28"));

	tmp = JSONVectorReader::readVector2f(face.get_child("26"));
	points_ann[7] = cv::Point2i((int)tmp[0] + nose_length[0], (int)tmp[1] + nose_length[1]);
	tmp = JSONVectorReader::readVector2f(face.get_child("17"));
	points_ann[8] = cv::Point2i((int)tmp[0] + nose_length[0], (int)tmp[1] + nose_length[1]);

	affine_transform_keypoints_ann[0] = points_ann[3];

	tmp = JSONVectorReader::readVector2f(face.get_child("36"));
	affine_transform_keypoints_ann[1] = { tmp[0], tmp[1] };
	tmp = JSONVectorReader::readVector2f(face.get_child("45"));
	affine_transform_keypoints_ann[2] = { tmp[0], tmp[1] };

	player_size = cv::Size(img1.cols, img1.rows);

	feather_amount.width = feather_amount.height = (int)cv::norm(points_ann[0] - points_ann[6]) / 8;
	mask_ann.create(player_size, CV_8UC1);
	mask_ann.setTo(cv::Scalar::all(0));
	cv::fillConvexPoly(mask_ann, points_ann, 9, cv::Scalar(255));
	img1.copyTo(face_ann, mask_ann);

	playerFaceWidth = PointCalcUtil::distanceBetween(points_ann[0], points_ann[6]);
}

void FxFaceSwap2::drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) {
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
	
	float percentToAdjust = (float) ((float)faceWidth / (float)playerFaceWidth);

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

			if (bgrPixel[0] == 0 && bgrPixel[1] > 250 && bgrPixel[2] == 0){
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

void FxFaceSwap2::featherMask(cv::Mat &refined_masks)
{
	cv::erode(refined_masks, refined_masks, getStructuringElement(cv::MORPH_RECT, feather_amount), cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	cv::blur(refined_masks, refined_masks, feather_amount, cv::Point(-1, -1), cv::BORDER_CONSTANT);
}

inline void FxFaceSwap2::pasteFacesOnFrame(cv::Mat frameVal)
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

void FxFaceSwap2::specifiyHistogram(const cv::Mat source_image, cv::Mat target_image, cv::Mat mask)
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