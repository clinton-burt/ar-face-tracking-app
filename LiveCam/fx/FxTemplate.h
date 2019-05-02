#pragma once

#include "boost/filesystem.hpp"

#include <models\FilterUiModel.h>

#include "FxCore.h"
#include "Fx3dModelAncestor.h"

class FxTemplate : public FxCore
{
public:
	FxTemplate(FilterUiModel* externalInfo);
	FxTemplate::~FxTemplate();

	void load();
	void transform(FXModel& face, ExternalRenderParams &externalRenderParams);
};