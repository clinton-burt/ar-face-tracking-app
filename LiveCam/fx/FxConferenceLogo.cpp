#include "fx/FxConferenceLogo.h"

FxConferenceLogo::FxConferenceLogo()
{
	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->emojisTextures.fill({ "./assets/fx/conferenceLogo/conferenceLogo.png" });
	emoji->emojiRotation.fill(true);
	emoji->emojisScale.fill({ 2, 2 });
	emoji->objectRenderParams.fill( { emoji->createDefaultObjectRenderParams() });

	models.push_back(emoji);
}

FxConferenceLogo::~FxConferenceLogo()
{

}