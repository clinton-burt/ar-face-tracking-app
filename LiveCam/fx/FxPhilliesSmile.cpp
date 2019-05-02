#include "fx/FxPhilliesSmile.h"

using namespace Eigen;

FxPhilliesSmile::FxPhilliesSmile()
{
	loadFromJSON("./assets/fx_config/filter-modules/phillies-smile-modules.json");

	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->name = "emoji";

	emoji->applySuit(filterModules[0].suits[0], 0, commonRenderParams, false);
	emoji->applySuit(filterModules[1].suits[0], 1, commonRenderParams, false);
	emoji->applySuit(filterModules[2].suits[0], 2, commonRenderParams, false);
	emoji->applySuit(filterModules[3].suits[0], 3, commonRenderParams, false);
	emoji->applySuit(filterModules[0].suits[0], 4, commonRenderParams, false);

	emoji->emojiRotation.fill(true);
	//emoji->emojisScale.fill({ 0.5f, 0.5f });
	//emoji->emojisOffset.fill({ 0, 0.06f, 0 });

	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxPhilliesSmile::~FxPhilliesSmile()                 
{
}