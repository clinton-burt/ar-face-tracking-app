#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxScarfHat : public FxCore
{
public:
	FxScarfHat();
	~FxScarfHat();

	std::shared_ptr<OpenGL3DModel> hat;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams);
}; 