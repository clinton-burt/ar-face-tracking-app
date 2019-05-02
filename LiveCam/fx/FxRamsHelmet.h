#pragma once

#include <fx\Fx3dModelAncestor.h>

class FxRamsHelmet : public FxCore
{
public:
	FxRamsHelmet();
	~FxRamsHelmet();

	void load() override;
	void draw(FXModel& face, ExternalRenderParams &externalRenderParams) override;

	std::array<std::string, 6> cubemap;

	GLuint cubemapID;
}; 