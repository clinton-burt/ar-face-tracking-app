#pragma once

#include <Fx3dModelAncestor.h>

#define _USE_MATH_DEFINES
#include <math.h>

class DepthMask : public OpenGL3DModel
{
public:
	static const std::string TYPE_NAME;
	std::string getTypeName() override;

	const static size_t FACE_FAN_COUNT = 23;;
	static const std::array<int, FACE_FAN_COUNT> face_fan;

	DepthMask();
	DepthMask(float zOffset, bool draw2Dmask, bool draw3Dmask, bool drawEars = false);
	~DepthMask();

	void onInputFrameResized() override;

	void loadFromJSON(boost::property_tree::ptree& modelRecord) override;
	void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately) override;

	bool load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	float zOffset;
	bool draw2Dmask;
	bool draw3Dmask;

protected:
	shared_ptr<cwc::glShader> shader2D;
	shared_ptr<cwc::glShader> shader3D;
	ModelRenderParams params2D;
	ModelRenderParams params3D;
	GLuint mask2dVBO;
};