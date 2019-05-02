#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxEaglesMilitaryHat : public FxCore
{
public:
	FxEaglesMilitaryHat();
	~FxEaglesMilitaryHat();

	std::shared_ptr<OpenGL3DModel> model;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};