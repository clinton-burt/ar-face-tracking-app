#include <fx\FxAnimatedStandardEmojiAncestor.h>
#include <fx\FxFacePaintAncestor.h>

class FxFireHead : public FxCore
{
public:

	AtlasAnimatedEmojiModel* fire;

	FxFireHead();
	~FxFireHead();

	void transform(FXModel& face, ExternalRenderParams &externalRenderParams) override;
};