#include "fx/FxEaglesMiniBirds.h"
const int miniEagles_CLOSED = 0;
const int miniEagles_OPEN = 1;
const int miniEagles_CLOSING = 2;

FxEaglesMiniBirds::FxEaglesMiniBirds()
{
	
	auto shader2D = shaderManagerWrapper.LoadFromFile("./assets/shaders/vertex/orthoVertexShader.txt", "./assets/shaders/fragment/orthoFragmentShader.txt");
	
	auto miniEagles = make_shared<OpenGLAnimatedEmojiModel>();
	miniEagles->name = "miniEagles";
	miniEagles->animationRotation.fill(true);

	miniEagles->animationPaths.fill("./assets/fx/eagles/mini_eagles/animation/");

	miniEagles->animationOffset.fill({ 0, -0.95f, 0 });
	miniEagles->animationScale.fill({ 2.f, 2.f });
	miniEagles->animationOpened.fill(180);

	//miniEagles->animationLength.fill(191);

	auto miniEaglesRenderParams = make_shared<ObjectRenderParams>();
	miniEaglesRenderParams->shader = shader2D;
	miniEaglesRenderParams->blend = true;


	miniEagles->objectRenderParams.fill({ miniEaglesRenderParams });


	models.push_back(miniEagles);

}

FxEaglesMiniBirds::~FxEaglesMiniBirds()
{

}