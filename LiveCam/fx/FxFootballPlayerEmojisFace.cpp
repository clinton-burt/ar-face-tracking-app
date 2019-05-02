#include "fx/FxFootballPlayerEmojisFace.h"

FxFootballPlayerEmojisFace::FxFootballPlayerEmojisFace()
{
	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->emojisTextures.fill({ "./assets/fx/nyc_football_club/footballPlayerEmojis/FPE_2.png" });
	emoji->emojisScale.fill({ 1.5f, 1.5f });
	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxFootballPlayerEmojisFace::~FxFootballPlayerEmojisFace()
{
}