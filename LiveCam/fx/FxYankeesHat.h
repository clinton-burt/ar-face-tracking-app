#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxYankeesHat : public FxCore
{
public:

	std::shared_ptr<OpenGL3DModel> model;

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;

	FxYankeesHat();
	~FxYankeesHat();
};