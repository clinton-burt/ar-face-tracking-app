#include "fx/FxPlayerEmojis.h"

FxPlayerEmojis::FxPlayerEmojis()
{
	loadFromJSON("./assets/fx_config/filter-modules/player-emoji-modules.json");
	
	auto shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoFragmentShader.txt");

	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->name = "playerEmojis";

	emoji->applySuit(filterModules[0].suits[0], 0, commonRenderParams, false);
	emoji->applySuit(filterModules[1].suits[0], 1, commonRenderParams, false);
	emoji->applySuit(filterModules[2].suits[0], 2, commonRenderParams, false);
	emoji->applySuit(filterModules[3].suits[0], 3, commonRenderParams, false);
	emoji->applySuit(filterModules[4].suits[0], 4, commonRenderParams, false);

	auto emojiRenderParams = make_shared<ObjectRenderParams>();
	emojiRenderParams->shader = shader2D;
	emojiRenderParams->blend = true;

	emoji->objectRenderParams.fill({ emojiRenderParams });

	models.push_back(emoji);
}

FxPlayerEmojis::~FxPlayerEmojis()                 
{
	
}