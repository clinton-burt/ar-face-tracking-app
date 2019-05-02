#include "fx/FxGiantPlayers.h"

FxGiantPlayers::FxGiantPlayers()
{
	loadFromJSON("./assets/fx_config/filter-modules/giants-modules.json");

	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->name = "giants";

	emoji->applySuit(filterModules[0].suits[0], 0, commonRenderParams, false);
	emoji->applySuit(filterModules[1].suits[0], 1, commonRenderParams, false);
	emoji->applySuit(filterModules[2].suits[0], 2, commonRenderParams, false);
	emoji->applySuit(filterModules[3].suits[0], 3, commonRenderParams, false);
	emoji->applySuit(filterModules[4].suits[0], 4, commonRenderParams, false);

	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxGiantPlayers::~FxGiantPlayers()
{
}