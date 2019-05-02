#pragma once

#include <QMainWindow>
#include "ui_EditorWindow.h"

#include "ui/modelTabs/EmojiModelTab.h"
#include "ui/modelTabs/Model3DTab.h"

#include <QMouseEvent>
#include <QEvent>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QFileDialog>

#include "boost/filesystem.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

class LiveCamWindow;

#include <models/OpenGLModel.h>
#include <FxCore.h>

#include "Fx3dModelAncestor.h"
#include "FxEmojiAncestor.h"
#include "FxAnimatedEmojiAncestor.h"
#include "FxFacePaintAncestor.h"
#include "DepthMask.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EditorWindow; }
QT_END_NAMESPACE

class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
	EditorWindow(LiveCamWindow *liveCamWindow, QWidget *parent = Q_NULLPTR);
    ~EditorWindow();

	void resetEditableFilter();

	std::shared_ptr<FxCore> getEditableFilter();
	ExtraFilterData getEditableFilterExtraData();

	void readFilterProperties(std::shared_ptr<FxCore> filter, FilterUiModel& externalInfo);

	EmojiModelTab* createEmojiModelTab();
	Model3DTab* EditorWindow::createModel3DTab(bool lipsJoint);

public: signals:
	void editor_window_closed();

public slots:

	void on_tab_deleted();
	void on_filterName_edit_textEdited(const QString &text);
	void on_filterIcon_button_clicked();
	void on_filterIcon_delete_clicked();

	void on_new_emoji_clicked();
	void on_new_3D_clicked();
	void on_new_LipsJoint_clicked();

	void on_models_tabWidget_currentChanged(int index);

	void on_hide_button_clicked();
	void on_save_button_clicked();
	void on_export_button_clicked();
	void on_import_button_clicked();

private:

	friend class LiveCamWindow;
	friend class EmojiModelTab;
	friend class Model3DTab;

	QIcon chooseImageIcon;

	QString fileDialogStart;
	QString filterFolderStart;

    Ui::EditorWindow ui;
	LiveCamWindow* liveCamWindow;

	ExtraFilterData data;
	std::shared_ptr<FxCore> editableFilter;

	bool EditorWindow::event(QEvent * e) override;

	QPushButton* createIconButton();
	void saveFilter(QString& filterFolder);

	void setFilterIcon(QString& iconPath);
	void setElementIcon(QString& iconPath);

protected:
	void closeEvent(QCloseEvent *event) override;
};

#include <ui/LiveCamWindow.h>