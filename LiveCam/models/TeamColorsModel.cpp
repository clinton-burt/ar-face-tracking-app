
#include <models\TeamColorsModel.h>

TeamColorsModel::TeamColorsModel(void) {
}

TeamColorsModel::TeamColorsModel(std::string nameString, int ltRVal, int ltGVal, int ltBVal, int rtRVal, int rtGVal, int rtBVal){
	name = nameString;

	ltR = (double)ltRVal;
	ltG = (double)ltGVal;
	ltB = (double)ltBVal;

	rtR = (double)rtRVal;
	rtG = (double)rtGVal;
	rtB = (double)rtBVal;
}

void TeamColorsModel::setIndex(int ind) {
	index = ind;
}
int TeamColorsModel::getIndex(void) {
	return index;
}
void TeamColorsModel::setId(int i) {
	id = i;
}
int TeamColorsModel::getId(void) {
	return id;
}