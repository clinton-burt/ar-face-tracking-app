#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxRamHorns : public FxCore
{
public:
	FxRamHorns();
	~FxRamHorns();

	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};  