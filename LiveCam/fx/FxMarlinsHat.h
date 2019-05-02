#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxMarlinsHat : public FxCore
{
public:
	FxMarlinsHat();
	~FxMarlinsHat();

	std::shared_ptr<OpenGL3DModel> model;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};