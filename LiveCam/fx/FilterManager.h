#ifndef FILTER_MANAGER_H
#define FILTER_MANAGER_H

#include <map>
#include <string>


#include <models\FilterUiModel.h>

#include "FxCore.h"
#include <fx/FxTemplate.h>

 #include <fx\FxBigBoyHair3d_2.h>
 #include <fx\FxDog3d.h>
 #include <fx\FxMrRedLegs.h>
 #include <fx\FxBobbleHead.h>
 //#include <fx\FxPinch.h>
#include <fx\FxFacePinch.h>
 #include <fx\FxEmojisCap.h>
 #include <fx\FxFacePaint.h>
 #include <fx\FxPlayerEmojis.h>
 #include <fx\Fx3dHeart.h>
 #include <fx\FxFootballHelmet.h>
 #include <fx\FxBaseballHat.h>
 #include <fx\FxDragon.h>
 #include <fx/FxEmojiFace3d.h>
 #include <fx\FxFaceSwap2.h>
 #include <fx\FxRedsCheekLogo.h>
 #include <fx\FxHockeyHelmet.h>
 #include <fx\FxKnightsHelmet.h>
 #include <fx\FxMarlinsHat.h>
 #include <fx\FxPhilliesHat.h>
 #include <fx\FxFacePaintYankees.h>
 #include <fx\FxYankeesHat.h>
 #include <fx\FxYankeesJudge.h>
 #include <fx\FxConferenceLogo.h>
 #include <fx\FxSoccerBall.h>
 #include <fx\FxScarfHat.h>
 #include <fx\FxFootballPlayerEmojis.h>
 #include <fx\FxFootballPlayerEmojisFace.h>
 #include <fx\FxEmojisOpenGL.h>
 #include "fx/FxRidersWatermelon.h"
 #include "fx/FxRidersHelmet.h"
 #include <fx\FxPanthersHelmet.h>
 #include <fx\FxSirPurr.h>
 #include <fx\FxPanthersPaintOpenGL.h>
 #include <fx\FxPanthersWig.h>
 #include <fx\FxPanthersBeanieOpenGL.h>
 #include <fx\FxGiantPlayers.h>
 #include <fx\FxRedsPlayerEmojis.h>
 #include <fx/FxPigeonMask.h>
 #include <fx/FxTeslaHelmet.h>
 #include <fx/FxFacePaintSimple.h>
 #include <fx/FxPhilliesPhanatic.h>
 #include <fx/FxPhilliesSmile.h>
 #include <fx/FxEaglesSwoopHead.h>
 #include <fx/FxEaglesSwoopBeak.h>
 #include <fx/FxEaglesMiniBirds.h>
 #include <fx/FxEaglesBlackEye.h>
 #include <fx/FxEaglesMilitaryHat.h>
 #include <fx/FxEaglesBaseballHat.h>
 #include <fx/FxEaglesCrazyHair.h>
 #include <fx/FxCowboysHelmet.h>
 #include <fx/FxCowboysFacePaint.h>
 #include <fx/FxCowboysRowdy.h>
 #include <fx/FxCowboysDog3d.h>
 #include <fx/FxFalconsCamoFacePaint.h>
 #include <fx/FxFacePaintBucs.h>
 #include <fx/FxJetsMohawk.h>
 #include <fx/FxHipsterBeard.h>
 #include <fx/FxCowboysAll.h>
 #include "fx/FxBillsFacePaint.h"
 #include "fx/FxChargersFP.h"
 #include <fx/FxYankeesBobbleBoy.h>
 #include <fx/FxMiamiHeatArenaEntertainersFaceSwap.h>
 #include <fx/FxMiamiHeatCelebritiesFaceSwap.h>
 #include <fx/FxMiamiHeatPlayersFaceSwap.h>
 #include <fx/FxMiamiHeatPastPlayersFaceSwap.h>
 #include <fx/FxMiamiHeatSuperHeroesFaceSwap.h>
#include <fx/FxGhost.h>
#include <fx/FxTeslaHelmetOKC.h>
#include <fx/FxChicagoBullsFaceSwap.h>
#include <fx/FxRidersPirate.h>
#include <fx/FxRidersGopher.h>
#include <fx/FxSjSharksFaceSwap.h>
#include <fx/FxFacePaintRavens.h>
#include <fx/FxFacePaintRavensNew.h>
#include <fx/FxBigEyes.h>
#include <fx/FxFacePaintNewTest.h>
#include <fx/FxLightningBee.h>
#include <fx/FxLightningHair.h>
#include <fx/FxThanksgivingEmojis.h>
#include <fx/FxTeslaHelmetLightning.h>
#include <fx/FxGumboMascotTongue.h>
#include <fx/FxFireHead.h>
#include <fx/FxRedsFaceSwap.h>
#include <fx/FxCelticsBobbleHead.h>
#include <fx/FxBravesTomahawkChop.h>
#include <fx/FxYankeesBobbleHead.h>
#include <fx/FxDebug.h>
#include <fx/FxDalmation3d.h>
#include <fx/FxVikingsFaceSwap.h>
#include <fx/FxCrowsFootball.h>

class FilterManager
{
public:

	typedef std::function<std::shared_ptr<FxCore>()> StaticCreator;

	std::map<int, StaticCreator> associations; // all associations
	std::map<int, std::shared_ptr<FxCore>> filters; // loaded from config list

	std::shared_ptr<FxCore> currentFilter;
	int currentFilterID;

	int newTemplatedFilterID = -1;

	FilterManager();
	~FilterManager();

	std::shared_ptr<FxCore> getCurrentFilter();
	int getCurrentFilterID();

	std::shared_ptr<FxCore> findFilterById(int filterID);

	void setCurrentFilterWithID(int filterID);

	void addFilter(int filterID);

	int addFilter(FilterUiModel* externalInfo);

	int addFilter(std::shared_ptr<FxCore> filter);

	void loadExternFilters(FilterUiModel& externalInfo, std::vector<FilterUiModel>& otherInfo);
};

#endif FILTER_MANAGER_H