#pragma once

#include <fx\Fx3dModelAncestor.h>
#include "models/SpriteAnimated.h"

class FxTeslaHelmet : public FxCore
{
public:
	std::shared_ptr<ObjectRenderParams> helmetRenderParams;
	std::shared_ptr<OpenGL3DModel> helmet;

	FxTeslaHelmet();
	~FxTeslaHelmet();

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;
	void unload() override;

	SpriteAnimated boltSprite;

	static const int PHI_COUNT = 3;
	static const int ALPHA_COUNT = 12;
	static const int MAX_RELOADING_TIME = 50;

	std::array<std::array<int, PHI_COUNT>, ALPHA_COUNT> animationStart;
	std::array<std::array<int, PHI_COUNT>, ALPHA_COUNT> animationIndex;
};