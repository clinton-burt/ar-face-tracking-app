#pragma once
#include <fx\FxCore.h>

class FxFaceSwap2 : public FxCore
{
public:
	FxFaceSwap2();
	~FxFaceSwap2();

	void applyModule(FilterModule *module, size_t faceIndex, bool loadTExturesImmediately) override;

	void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel) override;

	void selection(QString absolutePath, int index);
private:
	//cv::Mat_<int> connections;
	//std::vector<cv::Scalar> colors;
	cv::Mat img1;
	//cv::Mat img2;
	//cv::Mat img1Warped;

	cv::Mat leftDisplay;

	//cv::Mat imgHold; 
	QString currentPath;

	//int tryCounter = 0;

	// Blurs edges of mask
	void featherMask(cv::Mat &refined_masks);

	// Pastes faces on original frame
	void pasteFacesOnFrame(cv::Mat frameVal);


	// Calculates source image histogram and changes target_image to match source hist
	void specifiyHistogram(const cv::Mat source_image, cv::Mat target_image, cv::Mat mask);

	cv::Point2i getPoint(int i, const std::array<float, 66 * 2>& pts);

	//cv::Rect rect_ann, rect_bob;
	//cv::Rect big_rect_ann, big_rect_bob;

	cv::Point2f  affine_transform_keypoints_player[3], affine_transform_keypoints_ann[3]/*, affine_transform_keypoints_bob[3]*/;

	cv::Point2f affine_transform_keypoints_faces[FaceTracker::MAX_TO_TRACK][3];

	//cv::Mat refined_ann_and_bob_warpped, refined_bob_and_ann_warpped;
	cv::Mat warpped_face_ann, warpped_face_bob;

	cv::Point2i points_player[9], points_ann[9]/*, points_bob[9]*/;

	cv::Point2i points_faces[FaceTracker::MAX_TO_TRACK][9];
	cv::Mat trans_ann_to_face[FaceTracker::MAX_TO_TRACK];
	cv::Mat mask_face[FaceTracker::MAX_TO_TRACK];
	cv::Mat normal_face[FaceTracker::MAX_TO_TRACK];
	cv::Mat refined_face_and_ann_warpped[FaceTracker::MAX_TO_TRACK];

	cv::Mat trans_ann_to_bob, trans_bob_to_ann;
	cv::Mat mask_ann, mask_bob, mask_player;
	cv::Mat warpped_mask_ann, warpped_mask_bob;
	cv::Mat refined_masks;
	cv::Mat face_ann, face_bob;
	cv::Mat warpped_faces;

	cv::Mat small_frame;

	cv::Size player_size;
	cv::Size frame_size;

	cv::Size feather_amount;

	int playerFaceWidth;
};