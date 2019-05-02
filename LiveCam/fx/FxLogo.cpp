#include <fx/FxLogo.h>

FxLogo::FxLogo()
{
	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->emojisTextures.fill( { ".assets/fx/logo/images/logo.png" } );

	emoji->emojiRotation.fill(true);

	emoji->objectRenderParams.fill({ emoji->createDefaultObjectRenderParams() } );

	models.push_back(emoji);
}

FxLogo::~FxLogo()
{
}