#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxEaglesBaseballHat : public FxCore
{
public:
	FxEaglesBaseballHat();
	~FxEaglesBaseballHat();

	std::shared_ptr<OpenGL3DModel> model;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};