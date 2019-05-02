#pragma once

#include <QMainWindow>
#include <QListView>
#include "ui_LiveCamWindow.h"
#include <ui/cvimagewidget.h>

#include <ui\EditorWindow.h>
#include <ui\OutputWindow.h>
#include <ui/cvimagewidgetorg.h>
#include <opencv2/opencv.hpp>

#include "workers/cameraworker.h"

#include <QTimer>
#include <QMimeData>
#include <QDrag>
#include <QHash>
#include <QDropEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QFileInfo>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/iterator_range.hpp>

#include <ui/cvimagewidget.h>
#include <QCameraInfo>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <QThread>
#include <models\FilterUiModel.h>
#include <tracker\FaceTracker.h>

#include "interfaces/UiInterface.h"

#include "FilterManager.h"

#define MAX_NUM_CAM 8

QT_BEGIN_NAMESPACE
namespace Ui { class LiveCamWindow; }
QT_END_NAMESPACE

class QThread;
class QLabel;

class QSquareButton : public QPushButton
{
	void resizeEvent(QResizeEvent *event) override
	{
		setFixedHeight(width());
		setIconSize(QSize(width(), width()));
	}
};

class LiveCamWindow : public QMainWindow
{
    Q_OBJECT

public:
	FilterManager filterManager;

    LiveCamWindow(QWidget *parent = Q_NULLPTR);
	~LiveCamWindow();

	EditorWindow *editorWindow;
	OutputWindow *outputWindow;

	bool event(QEvent *e);

public slots:
	void exitCameraThread();
    void setRawLeftWidget(CVImageWidget* imageWidget);
    void setCompositeLeftWidget(CVImageWidget* imageWidget);
    void on_devices_comboBox_currentIndexChanged(int index);

    void on_btn_startEffectLeft_clicked();
    void on_btn_startFeedLeft_clicked(bool checked);
	void on_outputButton_clicked();
    void on_takeButton_clicked();
    void on_actionVideo_Output_Window_triggered();
    void closeEvent(QCloseEvent*);
    void on_actionExit_triggered();
    void timerHandler();
	void stopTimer();

	void on_create_filter_clicked();
	void on_edit_filter_clicked();
	void on_delete_filter_clicked();
	void on_import_filter_clicked();

	void on_editor_window_closed();
	void on_editor_moduleIcon_changed();
	void on_editor_filterIcon_changed();

	void initCameraAndVideoFileList();
    void initCameraList();
	void initVideoFileList();

	void on_leftViewerFxComboBox_currentIndexChanged(int index);

private slots:
	void initCameraThread();

	void runTracker2();
	
    void displayFrame(cv::Mat3b frame, int index);

    void on_pushButton_filter_clicked();

private:
	const static size_t SQUARE_BUTTON_MIN_SIZE = 40;
	const static size_t BUTTONS_PER_ROW = 4;

	friend class EditorWindow;
	friend class OutputWindow;

    Ui::LiveCamWindow ui;
    CVImageWidget *rawLeftWidget;
    CVImageWidget *rawRightWidget;
    CVImageWidget *compositeLeftWidget;
    CVImageWidget *compositeRighttWidget;
    int numCams;
    QLabel *labels[MAX_NUM_CAM];
   // QThread* threads[MAX_NUM_CAM];
   // CameraWorker* workers[MAX_NUM_CAM];

	QString startVideoString;
	QString stopVideoString;

    QString startEffectString;
    QString stopEffectString;

	QPushButton *activeFilterButton;

	QString startOutputString;
	QString stopOutputString;

	QString showFilterString;
	QString noShowFilterString;


	bool isDraggingOver;

	QThread* cameraThread;
	CameraWorker* cameraWorker;

    bool eventFilter(QObject *obj, QEvent *event);
    void installEventFilters();
	void onCamEventAction(QString camFileName, int index);

	void animatedSubmenuReplacing(std::shared_ptr<FxCore>);
	void animatedMainMenuReplacing(bool swapToEditor);

	std::vector<FilterUiModel> filterUiModels;

	void setFilter(int value);
	void setFilter(std::shared_ptr<FxCore> filter);

	QPushButton* createFilterButton();
	void addFilterButton(QPushButton* newbutton);
	QPushButton* addModuleButton();

public:
	QTimer *timer;

	bool isVideoStarted;
	bool isFilterStarted;
	bool isVideoDisplaying;
	bool isFilterDisplaying;

	FaceTracker faceTracker;
	CVImageWidgetOrg* outputImageWidget;

	bool addFilter(FilterUiModel& externalInfo);

private:
	void addFilter(bool rewrite); // only friendly EditorWindow calls it
};
