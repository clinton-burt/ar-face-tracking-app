#pragma once

#include "PointCalcUtil.h"

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

struct HalfFaceParams
{
	friend class HalfFacePaint;
	friend class OpenGLFacePaintModel;

	Eigen::Vector4f leftAmbient;
	Eigen::Vector4f rightAmbient;
	Eigen::Vector4f lineAmbient;

	Eigen::Vector4f leftDiffuse;
	Eigen::Vector4f rightDiffuse;
	Eigen::Vector4f lineDiffuse;

	std::string leftTexture;
	std::string rightTexture;
	std::string lineTexture;
	std::string alphaMask;

	float alphaMaskPower; // resultColor = pixelColor * (1 - (1 - alphaFromMap) * alphaMaskFactor), thus if 0, then no alphaMap applied

	std::string iconImagePath;

	float lineWidth = 0;

	float verticalOffset = 0;

	HalfFaceParams(); // is needed for compilation

	HalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
		Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
		std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth,
		std::string alphaMask, float alphaMaskPower = 1);

	void applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately);
	void load();

protected:
	GLuint leftTexId = 0;
	GLuint rightTexId = 0;
	GLuint lineTexId = 0;
	GLuint alphaMaskId = 0;
};

class HalfFacePaint
{
	friend class OpenGLFacePaintModel;

protected:
	const static size_t LINE_STRIP_COUNT = 16;

	const static size_t CHIN_STRIP_COUNT = 9;
	const static size_t CHEEK_FAN_COUNT = 10;
	const static size_t BROW_STRIP_COUNT = 10;
	const static size_t NOSE_FAN_COUNT = 8;
	const static size_t UNDER_NOSE_FAN_COUNT = 6;
	const static size_t FOREHEAD_STRIP_COUNT = 9;

	// point indexes start with 0
	static const std::vector<std::vector<std::vector<int>>> indexes;

	GLuint leftVBO, rightVBO, lineVBO;

	GLuint leftTexCoordVBO = 0, rightTexCoordVBO = 0, lineTexCoordVBO = 0;

	std::array<std::array<ValueSmoother, 66 * 2>, FaceTracker::MAX_TO_TRACK> facePointsSmoothers;

	std::array<Eigen::Vector2f, 13> faceLeftContour;
	std::array<Eigen::Vector2f, 13> faceRightContour;

	std::array<Eigen::Vector2f, 7> eyeLeftContour;
	std::array<Eigen::Vector2f, 7> eyeRightContour;

	std::array<Eigen::Vector2f, 7> mouthLeftContour;
	std::array<Eigen::Vector2f, 7> mouthRightContour;

	std::array<Eigen::Vector2f, 8> middleTopLines;
	std::array<Eigen::Vector2f, 2> middleBottomLines;

public:
	GLuint textureid = 0;

public:
	void load(bool headTextureMap);
	void transform(FXModel& face, HalfFaceParams &params);
	void draw(FXModel& face, HalfFaceParams &params, std::shared_ptr<cwc::glShader> shader);
	void unload();
};

class OpenGLFacePaintModel : public OpenGLModel
{
public:

	HalfFacePaint halfFacePaint;
	std::array<HalfFaceParams, FaceTracker::MAX_TO_TRACK> halfFaceParams;

	bool headTextureMap;
	std::array<float, FaceTracker::MAX_TO_TRACK> zOffset;
	std::array<bool, FaceTracker::MAX_TO_TRACK> match3D;

	OpenGLFacePaintModel();

	void onInputFrameResized() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	// for LiveCam Editor
	std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
	void setDefaultObjectRenderParams(ObjectRenderParams& params) override;

protected:
	GLuint textureid = 0;
};


namespace old
{
	class HalfFacePaint;
	class OpenGLFacePaintModel;

	struct HalfFaceParams
	{
		friend class HalfFacePaint;
		friend class OpenGLFacePaintModel;

		Eigen::Vector4f leftAmbient;
		Eigen::Vector4f rightAmbient;
		Eigen::Vector4f lineAmbient;

		Eigen::Vector4f leftDiffuse;
		Eigen::Vector4f rightDiffuse;
		Eigen::Vector4f lineDiffuse;

		std::string leftTexture;
		std::string rightTexture;
		std::string lineTexture;
		std::string alphaMask;

		std::string iconImagePath;

		float lineWidth = 0;

		HalfFaceParams(); // is needed for compilation

		HalfFaceParams(Eigen::Vector4f leftAmbient, Eigen::Vector4f rightAmbient, Eigen::Vector4f lineAmbient,
			Eigen::Vector4f leftDiffuse, Eigen::Vector4f rightDiffuse, Eigen::Vector4f lineDiffuse,
			std::string leftTexture, std::string rightTexture, std::string lineTexture, float lineWidth, std::string alphaMask);

		void applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately);
		void load();

	protected:
		GLuint leftTexId = 0;
		GLuint rightTexId = 0;
		GLuint lineTexId = 0;
		GLuint alphaMaskId = 0;
	};

	class HalfFacePaint
	{
		friend class OpenGLFacePaintModel;

	protected:
		const static size_t LINE_STRIP_COUNT = 16;

		const static size_t CHIN_STRIP_COUNT = 9;
		const static size_t CHEEK_FAN_COUNT = 10;
		const static size_t BROW_STRIP_COUNT = 10;
		const static size_t NOSE_FAN_COUNT = 8;
		const static size_t UNDER_NOSE_FAN_COUNT = 6;
		const static size_t FOREHEAD_STRIP_COUNT = 9;

		// point indexes start with 0
		static const std::vector<std::vector<std::vector<int>>> indexes;

		GLuint leftVBO, rightVBO, lineVBO;

		GLuint leftTexCoordVBO = 0, rightTexCoordVBO = 0, lineTexCoordVBO = 0;

		std::array<std::array<ValueSmoother, 66 * 2>, FaceTracker::MAX_TO_TRACK> facePointsSmoothers;

		std::array<Eigen::Vector2f, 13> faceLeftContour;
		std::array<Eigen::Vector2f, 13> faceRightContour;

		std::array<Eigen::Vector2f, 7> eyeLeftContour;
		std::array<Eigen::Vector2f, 7> eyeRightContour;

		std::array<Eigen::Vector2f, 7> mouthLeftContour;
		std::array<Eigen::Vector2f, 7> mouthRightContour;

		std::array<Eigen::Vector2f, 8> middleTopLines;
		std::array<Eigen::Vector2f, 2> middleBottomLines;

	public:
		GLuint textureid = 0;

	public:
		void load(bool headTextureMap);
		void transform(FXModel& face, HalfFaceParams &params);
		void draw(FXModel& face, HalfFaceParams &params, std::shared_ptr<cwc::glShader> shader);
		void unload();
	};

	class OpenGLFacePaintModel : public OpenGLModel
	{
	public:

		bool headTextureMap;

		HalfFacePaint halfFacePaint;
		std::array<HalfFaceParams, FaceTracker::MAX_TO_TRACK> halfFaceParams;

		OpenGLFacePaintModel();

		void onInputFrameResized() override;

		void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
		void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
			bool loadTexturesImmediately) override;

		bool load() override;
		void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
		void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
		void unload() override;

		// for LiveCam Editor
		std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams() override;
		void setDefaultObjectRenderParams(ObjectRenderParams& params) override;

	protected:
		GLuint textureid = 0;
	};


}