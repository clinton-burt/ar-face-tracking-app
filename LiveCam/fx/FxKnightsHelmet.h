#pragma once

#include <fx\Fx3dModelAncestor.h>

extern GLuint cubemapTexId; //find in main.cpp

class FxKnightsHelmet : public FxCore
{
public:
	FxKnightsHelmet();
	~FxKnightsHelmet();

	std::shared_ptr<OpenGL3DModel> helmet;

	void load() override;
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
}; 