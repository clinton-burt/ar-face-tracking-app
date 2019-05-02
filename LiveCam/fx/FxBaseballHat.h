#include <fx\Fx3dModelAncestor.h>


#pragma once

class FxBaseballHat : public FxCore
{
public:
	FxBaseballHat();
	~FxBaseballHat();

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;

private:
	std::shared_ptr<OpenGL3DModel> headModel;

	std::shared_ptr<cwc::glShader> shader = NULL;

	GLuint hatTexture;
	GLuint extraTexture;
	GLuint normalMap;
};