#include <models\FilterUiModel.h>

FilterUiModel::FilterUiModel(void)
{
	id = 0;
	connectedButton = nullptr;
}

FilterUiModel::FilterUiModel(boost::property_tree::ptree &record)
{
	title = record.get<std::string>("title", "");
	icon = record.get<std::string>("icon", "");
	JSONsource = record.get<std::string>("JSONsource", "");
	id = record.get<int>("id", 0);
	watermark = record.get<std::string>("watermark", "");
	connectedButton = nullptr;
}

void FilterUiModel::setTitle(std::string value) {
	title = value;
}
std::string FilterUiModel::getTitle(void) {
	return title;
}
void FilterUiModel::setId(int i) {
	id = i;
}
int FilterUiModel::getId(void) {
	return id;
}
void FilterUiModel::setIcon(std::string value) {
	icon = value;
	
	if (connectedButton)
	{
		connectedButton->setIcon(QIcon(QString::fromStdString(icon)));
	}
}
std::string FilterUiModel::getIcon(void) {
	return icon;
}
void FilterUiModel::setJSONsource(std::string value) {
	JSONsource = value;
}
std::string FilterUiModel::getJSONsource(void) {
	return JSONsource;
}
void FilterUiModel::setWatermark(std::string value) {
	watermark = value;
}
std::string FilterUiModel::getWatermark(void) {
	return watermark;
}