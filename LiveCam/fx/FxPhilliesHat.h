#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxPhilliesHat : public FxCore
{
public:
	FxPhilliesHat();
	~FxPhilliesHat();

	std::shared_ptr<OpenGL3DModel> model;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};