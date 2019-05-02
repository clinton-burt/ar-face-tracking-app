#include "PointCalcUtil.h"

#include "models/FXModel.h"
#include <models/OpenGLModel.h>

#pragma once

struct FacePaintParams
{
	friend class HalfFacePaint2;
	friend class OpenGLFacePaintModel2;

	std::string texture;

	std::string iconImagePath;
	std::string facemaskTexturePath;

	FacePaintParams(); // is needed for compilation

	FacePaintParams(std::string texture);

	void applySuit(boost::property_tree::ptree& suit, bool loadTexturesImmediately);
	void load();

	bool makeTexture();

protected:
	GLuint textureId = 0;
};

class FacePaint
{
	friend class OpenGLFacePaintModel2;

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

	std::array<std::array<ValueSmoother, 66 * 2>, FaceTracker::MAX_TO_TRACK> facePointsSmoothers;

	GLuint leftVBO;

	GLuint leftTexCoordVBO = 0;

public:
	GLuint textureid = 0;

public:
	void load();
	void transform(FXModel& face, FacePaintParams &params);
	void draw(FacePaintParams &params, std::shared_ptr<cwc::glShader> shader);
	void unload();
};

class OpenGLFacePaintModel2 : public OpenGLModel
{
public:
	FacePaint facepaint;
	std::array<FacePaintParams, FaceTracker::MAX_TO_TRACK> faceParams;

	OpenGLFacePaintModel2();

	void onInputFrameResized() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

protected:
	GLuint textureid = 0;
};