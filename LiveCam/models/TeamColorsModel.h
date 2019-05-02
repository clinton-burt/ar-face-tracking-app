#ifndef NFL_COLOR_OBJECT
#define NFL_COLOR_OBJECT

#include <iostream>

class TeamColorsModel {
public:
	void setIndex(int ind);
	int getIndex(void);

	void setId(int i);
	int getId(void);

	std::string name;

	double ltR;
	double ltG;
	double ltB;
	double rtR;
	double rtG;
	double rtB;

	TeamColorsModel();

	TeamColorsModel(std::string nameString, int ltRVal, int ltGVal, int ltBVal, int rtRVal, int rtGVal, int rtBVal);

private:
	int index = -1;
	int id;
};

#endif 