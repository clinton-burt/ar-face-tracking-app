#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <utils/DrawUtil.h>
#include "models/FXModel.h"
#include <models/OpenGLModel.h>
#include "utils/Resolutions.h"

#include <fx\FxCore.h>

extern FaceTracker faceTracker;

class FaceSwapModel : public OpenGLModel
{
protected:
	std::array<Eigen::Vector2f, FaceTracker::MAX_TO_TRACK> textureSize;

public:
	FaceSwapModel();

	static const std::string TYPE_NAME;

	std::array<std::string, FaceTracker::MAX_TO_TRACK> faceSwapTextures;
	std::array<GLuint, FaceTracker::MAX_TO_TRACK> faceSwapTexturesIDs;
	std::array<std::vector<GLuint>, FaceTracker::MAX_TO_TRACK> texturesIDs;
	std::array<std::vector<std::string>, FaceTracker::MAX_TO_TRACK> texturesPaths;

	std::string getTypeName() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;

	//OpenGL3DModel();

	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams, bool loadTexturesImmediately) override;

	//void applyModule(FilterModule *module, size_t faceIndex, bool loadTExturesImmediately);// override;

	bool load() override;

	void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) override;

	void unload() override;

	void selection(QString absolutePath, int index);

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;

private:
	cv::Mat img1;

	cv::Mat leftDisplay;

//	cv::Mat leftDisplay;
	
	QString currentPath;

	// Blurs edges of mask
	void featherMask(cv::Mat &refined_masks);

	// Pastes faces on original frame
	void pasteFacesOnFrame(cv::Mat frameVal);

	// Calculates source image histogram and changes target_image to match source hist
	void specifiyHistogram(const cv::Mat source_image, cv::Mat target_image, cv::Mat mask);


	cv::Point2f  affine_transform_keypoints_player[3], affine_transform_keypoints_ann[3];

	cv::Mat warpped_face_ann, warpped_face_bob;

	cv::Point2i points_player[9], points_ann[9];

	cv::Point2i getPoint(int i, const std::array<float, 66 * 2>& pts);

	cv::Point2f affine_transform_keypoints_faces[FaceTracker::MAX_TO_TRACK][3];
	
	cv::Mat trans_ann_to_face[FaceTracker::MAX_TO_TRACK];
	cv::Mat mask_face[FaceTracker::MAX_TO_TRACK];
	cv::Mat refined_face_and_ann_warpped[FaceTracker::MAX_TO_TRACK];

	cv::Mat warpped_mask_ann, warpped_mask_bob;
	cv::Mat warpped_faces;

	cv::Mat refined_masks;
	cv::Point2i points_faces[FaceTracker::MAX_TO_TRACK][9];
	cv::Mat mask_ann, mask_bob, mask_player;
	cv::Mat face_ann, face_bob;

	cv::Size player_size;
	cv::Size frame_size;

	cv::Size feather_amount;

	int playerFaceWidth;

};