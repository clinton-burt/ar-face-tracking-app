#pragma once

#include <fx\Fx3dModelAncestor.h>
#include "models/SpriteAnimated.h"

class FxLightningBee : public FxCore
{
public:
	std::shared_ptr<ObjectRenderParams> helmetRenderParams;
	std::shared_ptr<OpenGL3DModel> helmet;

	FxLightningBee();
	~FxLightningBee();

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	SpriteAnimated boltSprite;

	int animationIndex;
};