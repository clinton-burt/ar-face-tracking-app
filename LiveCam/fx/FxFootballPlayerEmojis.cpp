#include "fx/FxFootballPlayerEmojis.h"

FxFootballPlayerEmojis::FxFootballPlayerEmojis()
{
	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->emojisTextures.fill({ "./assets/fx/nyc_football_club/footballPlayerEmojis/FPE_1.png" });
	emoji->emojisScale.fill({ 2, 2 });
	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxFootballPlayerEmojis::~FxFootballPlayerEmojis()                 
{
}