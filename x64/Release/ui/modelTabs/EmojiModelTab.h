#pragma once

#include "boost/property_tree/ptree.hpp"
#include "boost/filesystem.hpp"

#include <QWidget>

#include "ui_EmojiModelTab.h"

namespace fs = boost::filesystem;

class EditorWindow;
class FxCore;
class OpenGLEmojiModel;

QT_BEGIN_NAMESPACE
namespace Ui { class EmojiModelTab; }
QT_END_NAMESPACE

class EmojiModelTab : public QWidget
{
	Q_OBJECT

public:
	EmojiModelTab(EditorWindow *editorWindow, QWidget *parent = Q_NULLPTR);
	~EmojiModelTab();

	void readEmojiModelProperties(FxCore* sourceFilter, OpenGLEmojiModel* model, boost::property_tree::ptree& suit,
		fs::path& resourcesRoot);

	void setEmojiImage(QString &imagePath);

public slots:
	void on_emojiImage_button_clicked();
	void on_emojiImage_delete_clicked();
	void on_modelName_edit_textEdited(const QString &text);

	void on_shiftX_slider_valueChanged(int value);
	void on_shiftY_slider_valueChanged(int value);
	void on_shiftZ_slider_valueChanged(int value);
	void on_scaleX_slider_valueChanged(int value);
	void on_scaleY_slider_valueChanged(int value);

	void on_shiftX_spinBox_valueChanged(double value);
	void on_shiftY_spinBox_valueChanged(double value);
	void on_shiftZ_spinBox_valueChanged(double value);
	void on_scaleX_spinBox_valueChanged(double value);
	void on_scaleY_spinBox_valueChanged(double value);

	void on_imageDirection_comboBox_currentIndexChanged(int value);
	void on_rotation_checkBox_toggled(bool checked);

protected:

private:
	Ui::EmojiModelTab ui;
	EditorWindow *editorWindow;
};


#include "ui/EditorWindow.h"