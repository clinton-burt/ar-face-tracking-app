#include "fx/FxThanksgivingEmojis.h"
const int TONGUE_CLOSED = 0;
const int TONGUE_OPEN = 1;
const int TONGUE_CLOSING = 2;

FxThanksgivingEmojis::FxThanksgivingEmojis()
{
	auto shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoFragmentShader.txt");
	
	auto tongue = make_shared<OpenGLAnimatedStandardEmojiModel>();
	tongue->name = "sirPurrTongue";
	tongue->animationRotation.fill(true);
	tongue->animationPaths.fill("./assets/fx/thanksgiving_bucket/emojis/eye-blink/");

	tongue->animationOffset.fill({ 0, 0, 0 });
	tongue->animationScale.fill({ 2.1f, 2.1f });	
	//tongue->animationLength.fill(64);
	
	auto tongueRenderParams = make_shared<ObjectRenderParams>();
	tongueRenderParams->shader = shader2D;
	tongueRenderParams->blend = true;

	tongue->objectRenderParams.fill({ tongueRenderParams });

	tongue->transformVersion = TransformVersion::V1;

	models.push_back(tongue);
}

FxThanksgivingEmojis::~FxThanksgivingEmojis()
{

}