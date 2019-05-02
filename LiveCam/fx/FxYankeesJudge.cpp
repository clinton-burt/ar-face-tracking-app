#include "fx/FxYankeesJudge.h"

FxYankeesJudge::FxYankeesJudge()
{
	auto judge = make_shared<OpenGLEmojiModel>();
	judge->name = "judge";
	judge->emojisTextures.fill("./assets/fx/yankeesJudge/yankeesJudge.png");
	judge->emojiRotation.fill(true);
	judge->symmetricState.fill(1);
	
	judge->emojisScale.fill({ 2 * 1.869f, 2 * 1.869f });
	judge->emojisOffset.fill({ -0.666f * 1.869f, -0.8f * 1.869f, -0.2f });

	judge->objectRenderParams.fill({ judge->createDefaultObjectRenderParams() });

	models.push_back(judge);
}

FxYankeesJudge::~FxYankeesJudge()
{

}