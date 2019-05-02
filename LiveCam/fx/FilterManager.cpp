#include "FilterManager.h"

// now zero ID is wrong ID and is used as error flag !
const int RED_LEGS_EFFECT = 1;
const int BOUFFANT_EFFECT = 2;
const int LOGO_EFFECT = 3;
const int DOG_CAM_EFFECT = 4;
const int PINCH = 6;
const int FACEPAINT = 7;
const int PLAYER_EMOJI_EFFECT = 8;
const int HEART_EFFECT = 9;
const int FACE_SWAP_EFFECT = 10;
const int BOBBLE_HEAD_EFFECT = 11;
const int FOOTBALL_HELMET_EFFECT = 12;
const int DRAGON_EFFECT = 13;
const int BASEBALL_HAT_EFFECT = 14;
const int EMOJI_3D_EFFECT = 15;
const int HOCKEY_HELMET_EFFECT = 16;
const int KNIGHT_HELMET_EFFECT = 17;
const int EMOJI_FILTER_CAP_EFFECT = 19;
const int MARLINS_HAT_EFFECT = 21;
const int PHILLIES_HAT_EFFECT = 23;
const int FACEPAINT_YANKEES = 24;
const int YANKEES_HAT_EFFECT = 25;
const int YANKEES_JUDGE_EFFECT = 26;
const int CONFERENCE_LOGO_EFFECT = 27;
const int SOCCER_BALL_EFFECT = 28;
const int SCARF_HAT_EFFECT = 29;
const int PANTHERS_HELMET_EFFECT = 30;
const int SIR_PURR_EFFECT = 31;
const int FOOTBALL_PLAYER_EMOJIS_EFFECT = 32;
const int FOOTBALL_PLAYER_EMOJIS_FACE_EFFECT = 33;
const int EMOJIS_OPENGL_EFFECT = 35;
const int PANTHERS_PAINT_OPENGL_EFFECT = 36;
const int GIANT_PLAYERS_EFFECT = 41;
const int PANTHERS_WIG_EFFECT = 42;
const int PANTHERS_BEANIE_OPENGL_EFFECT = 45;
const int RIDERS_HELMET_EFFECT = 46;
const int RIDERS_WATERMELON_EFFECT = 47;
const int FACEPAINT_SIMPLE = 48;
const int REDS_PLAYER_EMOJI_EFFECT = 49;
const int PIGEON_MASK_EFFECT = 50;
const int PHILLIES_PHANATIC_EFFECT = 51;
const int PHILLIES_SMILE_EFFECT = 52;
const int EAGLES_SWOOP_HEAD_EFFECT = 54;
const int EAGLES_SWOOP_BEAK_EFFECT = 55;
const int EAGLES_MINI_BIRDS_EFFECT = 56;
const int EAGLES_BLACK_EYE_EFFECT = 57;
const int EAGLES_MILITARY_HAT_EFFECT = 58;
const int EAGLES_BASEBALL_HAT_EFFECT = 59;
const int EAGLES_CRAZY_HAIR_EFFECT = 60;
const int TESLA_HELMET_EFFECT = 61;
const int COWBOYS_HELMET_EFFECT = 62;
const int COWBOYS_ROWDY_EFFECT = 63;
const int COWBOYS_DOG_EFFECT = 64;
const int COWBOYS_FACE_PAINT_EFFECT = 65;
const int FALCONS_FACE_PAINT_CAMO_EFFECT = 67;
const int BUCS_FACE_PAINT_EFFECT = 68;
const int HIPPSTER_BEARD_EFFECT = 69;
const int JETS_MOHAWK_EFFECT = 70;
const int COWBOYS_ALL_EFFECT = 71;
const int BILLS_FACE_PAINT_EFFECT = 72;
const int CHARGERS_PAINT_EFFECT = 73;
const int YANKEES_BOBBLE_BOY = 74;
const int MIAMI_HEAT_FACE_SWAP_CELEBRITIES = 75;
const int MIAMI_HEAT_FACE_SWAP_PLAYERS = 76;
const int MIAMI_HEAT_FACE_SWAP_PAST_PLAYERS = 77;
const int MIAMI_HEAT_FACE_SWAP_SUPER_HEROES = 78;
const int GHOST_FILTER = 79;
const int RAVENS_FACEPAINT_FILTER = 80;
const int TESLA_HELMET_OKC_FILTER = 81;
const int CHICAGO_BULLS_FACE_SWAP_FILTER = 82;
const int RIDERS_PIRATE_EFFECT = 83;
const int RIDERS_GOPHER_EFFECT = 84;
const int SJ_SHARKS_FACE_SWAP_EFFECT = 85;
const int MIAMI_HEAT_FACE_SWAP_ENTERTAINERS = 86;
const int FACE_PAINT_2 = 87;
const int RAVENS_FACEPAINT_NEW_FILTER = 88;
const int BIG_EYES_EFFECT = 89;
const int LIGHTNING_BEE_EFFECT = 90;
const int LIGHTNING_HAIR_EFFECT = 91;
const int ANIMATED_EMOJI_FILTER = 92;
const int TESLA_HELMET_LIGHTNING_FILTER = 93;
const int GUMBO_MASCOT_TONGUE = 94;
const int FIRE_HEAD_FILTER = 95;
const int REDS_FACE_SWAP_FILTER = 96;
const int CELTICS_BOBBLE_HEAD_FILTER = 97;
const int BRAVES_TOMAHAWK_CHOP = 98;
const int YANKEES_BOBBLE_HEAD = 99;
const int DEBUG_FILTER = 100;
const int DALMATION_FILTER = 101;
const int VIKINGS_FACE_SWAP_V2 = 102;
const int CROWS_FOOTBALL = 103;

FilterManager::FilterManager()
{
 	associations[DOG_CAM_EFFECT] = &FxCore::create<FxDog3d>;
 	associations[RED_LEGS_EFFECT] = &FxCore::create<FxMrRedLegs>;
 	associations[BOUFFANT_EFFECT] = &FxCore::create<FxBigBoyHair3d_2>;
 	associations[LOGO_EFFECT] = &FxCore::create<FxRedsCheekLogo>;
 	associations[PINCH] = &FxCore::create<FxFacePinch>;
 	associations[FACEPAINT] = &FxCore::create<FxFacePaint>;
 	associations[PLAYER_EMOJI_EFFECT] = &FxCore::create<FxPlayerEmojis>;
 	associations[HEART_EFFECT] = &FxCore::create<Fx3dHeart>;
 	associations[FACE_SWAP_EFFECT] = &FxCore::create<FxFaceSwap2>;
 	associations[BOBBLE_HEAD_EFFECT] = &FxCore::create<FxBobbleHead>;
 	associations[FOOTBALL_HELMET_EFFECT] = &FxCore::create<FxFootballHelmet>;
 	associations[DRAGON_EFFECT] = &FxCore::create<FxDragon>;
 	associations[BASEBALL_HAT_EFFECT] = &FxCore::create<FxBaseballHat>;
 	associations[EMOJI_3D_EFFECT] = &FxCore::create<FxEmojiFace3d>;
 	associations[HOCKEY_HELMET_EFFECT] = &FxCore::create<FxHockeyHelmet>;
 	associations[KNIGHT_HELMET_EFFECT] = &FxCore::create<FxKnightsHelmet>;
 	associations[EMOJI_FILTER_CAP_EFFECT] = &FxCore::create<FxEmojisCap>;
 	associations[MARLINS_HAT_EFFECT] = &FxCore::create<FxMarlinsHat>;
 	associations[PHILLIES_HAT_EFFECT] = &FxCore::create<FxPhilliesHat>;
 	associations[FACEPAINT_YANKEES] = &FxCore::create<FxFacePaintYankees>;
 	associations[YANKEES_HAT_EFFECT] = &FxCore::create<FxYankeesHat>;
 	associations[YANKEES_JUDGE_EFFECT] = &FxCore::create<FxYankeesJudge>;
 	associations[CONFERENCE_LOGO_EFFECT] = &FxCore::create<FxConferenceLogo>;
 	associations[SOCCER_BALL_EFFECT] = &FxCore::create<FxSoccerBall>;
 	associations[SCARF_HAT_EFFECT] = &FxCore::create<FxScarfHat>;
 	associations[FOOTBALL_PLAYER_EMOJIS_EFFECT] = &FxCore::create<FxFootballPlayerEmojis>;
 	associations[FOOTBALL_PLAYER_EMOJIS_FACE_EFFECT] = &FxCore::create<FxFootballPlayerEmojisFace>;
 	associations[EMOJIS_OPENGL_EFFECT] = &FxCore::create<FxEmojisOpenGL>;
 	associations[RIDERS_WATERMELON_EFFECT] = &FxCore::create<FxRidersWatermelon>;
 	associations[RIDERS_HELMET_EFFECT] = &FxCore::create<FxRidersHelmet>;
 	associations[PANTHERS_HELMET_EFFECT] = &FxCore::create<FxPanthersHelmet>;
 	associations[SIR_PURR_EFFECT] = &FxCore::create<FxSirPurr>;
 	associations[FOOTBALL_PLAYER_EMOJIS_EFFECT] = &FxCore::create<FxFootballPlayerEmojis>;
 	associations[FOOTBALL_PLAYER_EMOJIS_FACE_EFFECT] = &FxCore::create<FxFootballPlayerEmojisFace>;
 	associations[EMOJIS_OPENGL_EFFECT] = &FxCore::create<FxEmojisOpenGL>;
 	associations[PANTHERS_PAINT_OPENGL_EFFECT] = &FxCore::create<FxPanthersPaintOpenGL>;
 	associations[PANTHERS_WIG_EFFECT] = &FxCore::create<FxPanthersWig>;
 	associations[SIR_PURR_EFFECT] = &FxCore::create<FxSirPurr>;
 	associations[PANTHERS_BEANIE_OPENGL_EFFECT] = &FxCore::create<FxPanthersBeanieOpenGL>;
 	associations[GIANT_PLAYERS_EFFECT] = &FxCore::create<FxGiantPlayers>;
 	associations[REDS_PLAYER_EMOJI_EFFECT] = &FxCore::create<FxRedsPlayerEmojis>;
 	associations[PIGEON_MASK_EFFECT] = &FxCore::create<FxPigeonMask>;
 	associations[FACEPAINT_SIMPLE] = &FxCore::create<FxFacePaintSimple>;
	associations[TESLA_HELMET_EFFECT] = &FxCore::create<FxTeslaHelmet>;
	associations[PHILLIES_PHANATIC_EFFECT] = &FxCore::create<FxPhilliesPhanatic>;
 	associations[PHILLIES_SMILE_EFFECT] = &FxCore::create<FxPhilliesSmile>;
 	associations[EAGLES_SWOOP_HEAD_EFFECT] = &FxCore::create<FxEaglesSwoopHead>;
 	associations[EAGLES_SWOOP_BEAK_EFFECT] = &FxCore::create<FxEaglesSwoopBeak>;
 	associations[EAGLES_MINI_BIRDS_EFFECT] = &FxCore::create<FxEaglesMiniBirds>;
 	associations[EAGLES_BLACK_EYE_EFFECT] = &FxCore::create<FxEaglesBlackEye>;
 	associations[EAGLES_MILITARY_HAT_EFFECT] = &FxCore::create<FxEaglesMilitaryHat>;
 	associations[EAGLES_BASEBALL_HAT_EFFECT] = &FxCore::create<FxEaglesBaseballHat>;
 	associations[EAGLES_CRAZY_HAIR_EFFECT] = &FxCore::create<FxEaglesCrazyHair>;
 	associations[COWBOYS_HELMET_EFFECT] = &FxCore::create<FxCowboysHelmet>;
 	associations[COWBOYS_ROWDY_EFFECT] = &FxCore::create<FxCowboysRowdy>;
 	associations[COWBOYS_DOG_EFFECT] = &FxCore::create<FxCowboysDog3d>;
 	associations[COWBOYS_FACE_PAINT_EFFECT] = &FxCore::create<FxCowboysFacePaint>;
 	associations[FALCONS_FACE_PAINT_CAMO_EFFECT] = &FxCore::create<FxFalconsCamoFacePaint>;
 	associations[BUCS_FACE_PAINT_EFFECT] = &FxCore::create<FxFacePaintBucs>;
 	associations[HIPPSTER_BEARD_EFFECT] = &FxCore::create<FxHipsterBeard>;
 	associations[JETS_MOHAWK_EFFECT] = &FxCore::create<FxJetsMohawk>;
 	associations[COWBOYS_ALL_EFFECT] = &FxCore::create<FxCowboysAll>;
 	associations[BILLS_FACE_PAINT_EFFECT] = &FxCore::create<FxBillsFacePaint>;
 	associations[CHARGERS_PAINT_EFFECT] = &FxCore::create<FxChargersFP>;
 	associations[YANKEES_BOBBLE_BOY] = &FxCore::create<FxYankeesBobbleBoy>;
 	associations[MIAMI_HEAT_FACE_SWAP_CELEBRITIES] = &FxCore::create<FxMiamiHeatCelebritiesFaceSwap>;
 	associations[MIAMI_HEAT_FACE_SWAP_PLAYERS] = &FxCore::create<FxMiamiHeatPlayersFaceSwap>;
 	associations[MIAMI_HEAT_FACE_SWAP_PAST_PLAYERS] = &FxCore::create<FxMiamiHeatPastPlayersFaceSwap>;
	associations[MIAMI_HEAT_FACE_SWAP_SUPER_HEROES] = &FxCore::create<FxMiamiHeatSuperHeroesFaceSwap>;
	associations[MIAMI_HEAT_FACE_SWAP_ENTERTAINERS] = &FxCore::create<FxMiamiHeatArenaEntertainersFaceSwap>;
	associations[GHOST_FILTER] = &FxCore::create<FxGhost>;
	associations[TESLA_HELMET_OKC_FILTER] = &FxCore::create<FxTeslaHelmetOKC>;
	associations[CHICAGO_BULLS_FACE_SWAP_FILTER] = &FxCore::create<FxChicagoBullsFaceSwap>;
	associations[RIDERS_PIRATE_EFFECT] = &FxCore::create<FxRidersPirate>;
	associations[RIDERS_GOPHER_EFFECT] = &FxCore::create<FxRidersGopher>;
	associations[SJ_SHARKS_FACE_SWAP_EFFECT] = &FxCore::create<FxSjSharksFaceSwap>;
	associations[RAVENS_FACEPAINT_FILTER] = &FxCore::create<FxFacePaintRavens>;
	associations[RAVENS_FACEPAINT_NEW_FILTER] = &FxCore::create<FxFacePaintRavensNew>;
	associations[BIG_EYES_EFFECT] = &FxCore::create<FxBigEyes>;
	associations[FACE_PAINT_2] = &FxCore::create<FxFacePaintNewTest>;
	associations[LIGHTNING_BEE_EFFECT] = &FxCore::create<FxLightningBee>;
	associations[LIGHTNING_HAIR_EFFECT] = &FxCore::create<FxLightningHair>;
	associations[ANIMATED_EMOJI_FILTER] = &FxCore::create<FxThanksgivingEmojis>;
	associations[TESLA_HELMET_LIGHTNING_FILTER] = &FxCore::create<FxTeslaHelmetLightning>;
	associations[GUMBO_MASCOT_TONGUE] = &FxCore::create<FxGumboMascotTongue>;
	associations[FIRE_HEAD_FILTER] = &FxCore::create<FxFireHead>;
	associations[REDS_FACE_SWAP_FILTER] = &FxCore::create<FxRedsFaceSwap>;
	associations[CELTICS_BOBBLE_HEAD_FILTER] = &FxCore::create<FxCelticsBobbleHead>;
	associations[BRAVES_TOMAHAWK_CHOP] = &FxCore::create<FxBravesTomahawkChop>;
	associations[YANKEES_BOBBLE_HEAD] = &FxCore::create<FxYankeesBobbleHead>;
	associations[DEBUG_FILTER] = &FxCore::create<FxDebug>;
	associations[DALMATION_FILTER] = &FxCore::create<FxDalmation3d>;
	associations[VIKINGS_FACE_SWAP_V2] = &FxCore::create<FxVikingsFaceSwap>;
	associations[CROWS_FOOTBALL] = &FxCore::create<FxCrowsFootball>;
}

FilterManager::~FilterManager()
{
}

// never returns nullptr for today, if any filter was loaded successfully from json
std::shared_ptr<FxCore> FilterManager::getCurrentFilter()
{
	return currentFilter;
}

int FilterManager::getCurrentFilterID()
{
	return currentFilterID;
}

void FilterManager::setCurrentFilterWithID(int filterID)
{
	auto newFilter = filters[filterID];
	if (newFilter != nullptr)
	{
		currentFilter = newFilter;
		currentFilterID = filterID;
	}
}

std::shared_ptr<FxCore> FilterManager::findFilterById(int filterID)
{
	return filters[filterID];
}

void FilterManager::addFilter(int filterID)
{
	if (filterID < 1)
	{
		return;
	}

	auto creator = associations[filterID];
	if (creator != nullptr)
	{
		auto filter = filters[filterID] = creator();
		if (currentFilter == nullptr)
		{
			currentFilter = filter;
			currentFilterID = filterID;
		}
	}
}

int FilterManager::addFilter(FilterUiModel* externalInfo)
{
	std::shared_ptr<FxCore> ptr;
	
	try
	{
		ptr = make_shared<FxTemplate>(externalInfo);
	}
	catch (std::exception &e)
	{
		return 0;
	}

	filters[newTemplatedFilterID] = ptr;
	if (currentFilter == nullptr)
	{
		currentFilter = filters[newTemplatedFilterID];
		currentFilterID = newTemplatedFilterID;
	}

	externalInfo->setId(newTemplatedFilterID);
	return newTemplatedFilterID--;
}

int FilterManager::addFilter(std::shared_ptr<FxCore> filter)
{
	filters[newTemplatedFilterID] = filter;
	if (currentFilter == nullptr)
	{
		currentFilter = filters[newTemplatedFilterID];
		currentFilterID = newTemplatedFilterID;
	}
	return newTemplatedFilterID--;
}

void FilterManager::loadExternFilters(FilterUiModel& externalInfo, std::vector<FilterUiModel>& otherInfo)
{
	if (externalInfo.getJSONsource().empty())
	{
		return;
	}

	auto filter = findFilterById(externalInfo.getId());

	boost::property_tree::ptree source;
	boost::property_tree::json_parser::read_json(externalInfo.getJSONsource(), source);

	auto externList = source.get_child_optional("extern");
	if (externList)
	{
		for (auto &externFilter : externList.get())
		{
			auto value = externFilter.second.get_value<std::string>();

			int id = 0;
			try
			{
				id = std::stoi(value);
				filter->externFilters.push_back(associations[id]());
				continue;
			}
			catch (...) {}

			for (auto other : otherInfo)
			{
				if (other.getJSONsource() == value)
				{
					auto externFilter = filters[other.getId()];
					if (externFilter->externFilters.size() > 0)
					{
						throw new std::exception("Included filter can not include any other filter!");
						// for now don't know what to do in this case, let's throw an exception
					}
					filter->externFilters.push_back(filters[other.getId()]);
					continue;
				}
			}

			FilterUiModel externalInfo;
			externalInfo.setJSONsource(value);
			auto externFilter = std::make_shared<FxCore>(FxTemplate(&externalInfo));

			filter->externFilters.push_back(externFilter);
		}
	}

	for (auto &module : filter->filterModules)
	{
		if (module.externFilterID == -1)
		{
			continue;
		}

		if (module.externModuleID > -1)
		{
			auto& externModule = filter->externFilters[module.externFilterID]->filterModules[module.externModuleID];
			module.icon = externModule.icon;
			module.iconPath = externModule.iconPath;
			module.suits = externModule.suits;
		}
	}
}