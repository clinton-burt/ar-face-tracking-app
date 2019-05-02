#include "fx/FxEmojisCap.h"

FxEmojisCap::FxEmojisCap()
{
	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->emojisTextures.fill({ "./assets/images/emojifilters/GPE019_MarlinsCap_.png" });
	emoji->emojiRotation.fill(true);
	emoji->emojisScale.fill({ 2, 2 });
	emoji->emojisOffset.fill({ 0, 0, -0.3f });
	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxEmojisCap::~FxEmojisCap()
{

}