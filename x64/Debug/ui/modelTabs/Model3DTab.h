#pragma once

#include "boost/property_tree/ptree.hpp"
#include "boost/filesystem.hpp"

#include <QWidget>
#include <QPushButton>

#include "ui_Model3DTab.h"
#include "ui/modelTabs/RenderParamsTab.h"

namespace fs = boost::filesystem;

class EditorWindow;
class FxCore;
class OpenGL3DModel;

QT_BEGIN_NAMESPACE
namespace Ui { class Model3DTab; }
QT_END_NAMESPACE

class Model3DTab : public QWidget
{
	Q_OBJECT

public:
	Model3DTab(EditorWindow *editorWindow, QWidget *parent = Q_NULLPTR);
	~Model3DTab();

	void read3DModelProperties(FxCore* sourceFilter, OpenGL3DModel* sourceModel, OpenGL3DModel* destinyModel,
		boost::property_tree::ptree& suit, fs::path& resourcesRoot);

private slots:

	void on_modelName_edit_textEdited(const QString &text);
	void on_elementIcon_button_clicked();
	void on_elementIcon_delete_clicked();

	void on_objectButton_clicked();
	void on_shader_list_currentIndexChanged(int value);

	void on_OBJfile_choose_clicked();
	void on_MTLfile_choose_clicked();
	void on_OBJfile_delete_clicked();
	void on_MTLfile_delete_clicked();

	void on_shiftX_slider_valueChanged(int value);
	void on_shiftY_slider_valueChanged(int value);
	void on_shiftZ_slider_valueChanged(int value);
	void on_rotateX_slider_valueChanged(int value);
	void on_rotateY_slider_valueChanged(int value);
	void on_rotateZ_slider_valueChanged(int value);
	void on_scaleX_slider_valueChanged(int value);
	void on_scaleY_slider_valueChanged(int value);
	void on_scaleZ_slider_valueChanged(int value);

	void on_shiftX_spinBox_valueChanged(double value);
	void on_shiftY_spinBox_valueChanged(double value);
	void on_shiftZ_spinBox_valueChanged(double value);
	void on_rotateX_spinBox_valueChanged(double value);
	void on_rotateY_spinBox_valueChanged(double value);
	void on_rotateZ_spinBox_valueChanged(double value);
	void on_scaleX_spinBox_valueChanged(double value);
	void on_scaleY_spinBox_valueChanged(double value);
	void on_scaleZ_spinBox_valueChanged(double value);

private:
	Ui::Model3DTab ui;
	EditorWindow *editorWindow;

	int objIndex;

	// quickLoading is only loads count of meshes for filling arrays
	void setOBJ(std::vector<std::string> &fileNames, bool quickLoading = false);
	void setMTL(std::vector<std::string> &fileNames, bool quickLoading = false);
	void setShader(size_t objectIndex, std::string name);
};

#include "ui/EditorWindow.h"