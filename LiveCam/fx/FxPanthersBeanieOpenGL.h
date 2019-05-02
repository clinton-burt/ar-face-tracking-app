#pragma once

#include <fx\FxAnimatedEmojiAncestor.h>
#include <fx\Fx3dModelAncestor.h>

class FxPanthersBeanieOpenGL : public FxCore
{
public:
	FxPanthersBeanieOpenGL();
	~FxPanthersBeanieOpenGL();

	std::shared_ptr<OpenGL3DModel> hat;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};