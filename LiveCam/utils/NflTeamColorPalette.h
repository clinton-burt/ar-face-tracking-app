#ifndef TEAM_COLOR_MODEL_OBJECT
#define TEAM_COLOR_MODEL_OBJECT

#include <models\TeamColorsModel.h>

class NflTeamColorPalette
{

public:
	NflTeamColorPalette();
	~NflTeamColorPalette();

	static TeamColorsModel colors[4];

	static void init()
	{
		colors[0] = TeamColorsModel("cowboys.png", 0, 87, 120, 244, 130, 33);
		colors[1] = TeamColorsModel("redskins.png", 0, 83, 149, 255, 255, 255);
		colors[2] = TeamColorsModel("ny.png", 218, 33, 39, 0, 42, 92);
		colors[3] = TeamColorsModel("eagles.png", 41, 67, 58, 255, 255, 255);
	}

	
};

#endif 