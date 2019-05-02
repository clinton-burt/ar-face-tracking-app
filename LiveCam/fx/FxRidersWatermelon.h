#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxRidersWatermelon : public FxCore
{
public:
	FxRidersWatermelon();
	~FxRidersWatermelon();

	std::shared_ptr<OpenGL3DModel> model;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
}; 