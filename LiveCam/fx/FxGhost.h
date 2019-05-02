#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxGhost : public FxCore
{
public:
	FxGhost();
	~FxGhost();

	std::shared_ptr<cwc::glShader> ghostShader;

	std::shared_ptr<OpenGL3DModel> ghost;
	float wavesOffset;
	GLuint wavesTex;

	float shineFactor;
	bool shineIncreasing;

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
}; 