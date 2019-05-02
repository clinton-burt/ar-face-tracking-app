#include "LiveCamWindow.h"

#include "Resolutions.h"

Q_DECLARE_METATYPE(QCameraInfo)

QPushButton* LiveCamWindow::createFilterButton()
{
	QSquareButton* newbutton = new QSquareButton();

	newbutton->setAutoExclusive(true);
	newbutton->setCheckable(true);
	newbutton->setMinimumSize(QSize(SQUARE_BUTTON_MIN_SIZE, SQUARE_BUTTON_MIN_SIZE));
	newbutton->setIconSize(QSize(SQUARE_BUTTON_MIN_SIZE, SQUARE_BUTTON_MIN_SIZE));

	connect(newbutton, SIGNAL(clicked()), this, SLOT(on_pushButton_filter_clicked()));

	return newbutton;
}

void LiveCamWindow::addFilterButton(QPushButton* newbutton)
{
	int number = activeFilterButton ? ui.filters_content->findChildren<QPushButton*>().count() : 0;

	int y = number / BUTTONS_PER_ROW;
	int x = number % BUTTONS_PER_ROW;

	ui.gridLayout_filters_content->addWidget(newbutton, y, x);
}

QPushButton* LiveCamWindow::addModuleButton()
{
	QSquareButton* newbutton = new QSquareButton();

	newbutton->setMinimumSize(QSize(SQUARE_BUTTON_MIN_SIZE, SQUARE_BUTTON_MIN_SIZE));
	newbutton->setIconSize(QSize(SQUARE_BUTTON_MIN_SIZE, SQUARE_BUTTON_MIN_SIZE));
	newbutton->setMouseTracking(true);
	newbutton->setVisible(false);

	newbutton->installEventFilter(this);

	int number = ui.modules_content->findChildren<QPushButton*>().count();

	int y = number / BUTTONS_PER_ROW;
	int x = number % BUTTONS_PER_ROW;

	ui.gridLayout_modules_content->addWidget(newbutton, y, x);

	return newbutton;
}

bool LiveCamWindow::event(QEvent * e) // overloading event(QEvent*) method of QMainWindow 
{
	switch (e->type())
	{
	case QEvent::WindowActivate:
		if (!ui.btn_startEffectLeft_Container->isEnabled())
		{
			editorWindow->raise();
		}
		break;
	};
	return QMainWindow::event(e);
}

LiveCamWindow::LiveCamWindow(QWidget *parent) : QMainWindow(parent)
{
	outputWindow = new OutputWindow(this);
	outputWindow->showMaximized();

	editorWindow = new EditorWindow(this);

	connect(editorWindow, SIGNAL(editor_window_closed()), this, SLOT(on_editor_window_closed()));
	connect(editorWindow->ui.filterIcon_button, SIGNAL(clicked()), this, SLOT(on_editor_filterIcon_changed()));
	connect(editorWindow->ui.filterIcon_delete, SIGNAL(clicked()), this, SLOT(on_editor_filterIcon_changed()));

	ui.setupUi(this);
    ui.pages->setCurrentWidget(ui.mainPage);

	startVideoString = "Start Video";
	stopVideoString = "Stop Video";

	startEffectString = "Track Faces";
	stopEffectString = "Stop Tracking";

	startOutputString = "Start Output";
	stopOutputString = "Stop Output";

	showFilterString = "Show Filter";
	noShowFilterString = "Hide Filter";

    ui.devices_comboBox->setView(new QListView());

	isDraggingOver = false;

    //initCameraList();
	initCameraAndVideoFileList();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerHandler()));
	//timer->start(0);
	//connect(timer, SIGNAL(timeout()), this, SLOT(processOneThing()));

#ifdef NDEBUG
	timer->start(15); // 60 FPS limit for release
#else
	timer->start(30); // 30 FPS limit for debug (to speed up UI)
#endif

	isVideoStarted = false;
	isFilterStarted = false;
	isFilterDisplaying = false;
	isVideoDisplaying = false;
	
	QGridLayout *filterTable = findChild<QGridLayout*>("gridLayout_filters_content");
	filterTable->setAlignment(Qt::AlignLeft);
	filterTable->setAlignment(Qt::AlignTop);

	QGridLayout *filterSubTable = findChild<QGridLayout*>("gridLayout_modules_content");
	filterSubTable->setAlignment(Qt::AlignLeft);
	filterSubTable->setAlignment(Qt::AlignTop);

	for (int i = 0; i < BUTTONS_PER_ROW; ++i)
	{
		filterTable->setColumnStretch(i, 1);
		filterSubTable->setColumnStretch(i, 1);
	}

	activeFilterButton = nullptr;

	bool loadCustom = true;

	try // SAFETY LOAD FROM THE config.json
	{
		boost::property_tree::ptree config;
		
		boost::property_tree::read_json("./assets/application_config.json", config);

		bool createMode = config.get<bool>("editor", false);

		loadCustom = !config.get<bool>("ignore custom", false);
		
		if (!createMode)
		{
			ui.create_filter->hide();
			ui.edit_filter->hide();
			ui.delete_filter->hide();
			ui.import_filter->hide();
		}
		
		auto JSONsource = config.get<std::string>("filters", "");
		
		if (!JSONsource.empty())
		{
			boost::property_tree::read_json(JSONsource, config);
		}
		else
		{
			throw std::exception();

			//boost::property_tree::read_json("./assets/fx_config/ui-config.json", config);

			//boost::property_tree::read_json("./assets/fx_config/ui-config-EDITOR.json", config);

			//boost::property_tree::read_json("./assets/fx_config/ui-config-TMP.json", config);

			//boost::property_tree::read_json("./assets/fx_config/ui-config-FULL-TEST.json", config);

		}

		auto filterListRecord = config.get_child_optional("filters");

		if (filterListRecord)
		{
			for (auto &filterRecord : filterListRecord.get())
			{
				auto externalInfo = FilterUiModel(filterRecord.second);
				if (addFilter(externalInfo))
				{
					filterUiModels.push_back(externalInfo);
				}
				else
				{
					qDebug().noquote() << "\n!!! FILTER LOADING ERROR: \"" + QString::fromStdString(externalInfo.getTitle()) + "\"\n";
				}
			}
		}
	}
	catch (const std::exception&)
	{
		qDebug().noquote() << "\n!!! CONFIG LOADING ERROR !!!\n";
	}

	if (loadCustom)
	{
		// autoscan the CUSTOM folder & load filters
		for (auto& entry : fs::directory_iterator("./assets/fx_config/custom-filters"))
		{
			if (fs::is_directory(entry))
			{
				auto JSONsource = fs::path(entry.path() / "filter-config.json");
				if (fs::exists(JSONsource))
				{
					FilterUiModel externalInfo;
					externalInfo.setJSONsource(JSONsource.string());

					if (addFilter(externalInfo))
					{
						filterUiModels.push_back(externalInfo);
					}
					else
					{
						qDebug().noquote() << "\n!!! FILTER LOADING ERROR: \"" + QString::fromStdString(externalInfo.getTitle()) + "\"\n";
					}
				}
			}
		}
	}

	if (ui.filters_content->findChildren<QPushButton*>().count() == 0) // need one button for the editor, if none filter was loaded
	{
		QPushButton *newbutton = createFilterButton();
		newbutton->setProperty("buttonNumber", 0);
		newbutton->setVisible(false);
		addFilterButton(newbutton);
	}

	auto filter = filterManager.getCurrentFilter();

	if (filter != nullptr) // if current filter has modules, load their icons
	{
		if (filter->editable)
		{
			ui.delete_filter->setEnabled(true);
		}

		int x = 0, y = 0;

		for (int i = 0; i < filter->filterModules.size(); ++i)
		{
			QPushButton *button = qobject_cast<QPushButton*>(filterSubTable->itemAt(i)->widget());
			button->setIcon(filter->filterModules[i].icon);
			button->setProperty("filterModule", (qulonglong)&filter->filterModules[i]);
			button->setVisible(true);

			if (++x == BUTTONS_PER_ROW)
			{
				x = 0;
				++y;
			}
		}

		ui.label_current_filter->setText(QString::fromStdString(filterUiModels.front().getTitle()));
	}

	installEventFilters();
}

LiveCamWindow::~LiveCamWindow(){
}

bool LiveCamWindow::addFilter(FilterUiModel& externalInfo)
{
	std::string JSONsource = externalInfo.getJSONsource();

	QPushButton *newbutton = createFilterButton();
	externalInfo.connectedButton = newbutton;

	try
	{
		if (JSONsource.empty()) // CHOICE BETWEEN HARDCODED AND TEMPLATED FILTER
		{
			if (externalInfo.getId() == 0)
			{
				throw std::exception();
			}
			else
			{
				filterManager.addFilter(externalInfo.getId());
			}
		}
		else
		{
			if (filterManager.addFilter(&externalInfo) == 0)
			{
				throw std::exception();
			}
		}
	}
	catch (std::exception &e)
	{
		return false;
	}

	int	number = ui.filters_content->findChildren<QPushButton*>().count();

	newbutton->setProperty("buttonNumber", number);
	addFilterButton(newbutton);
	// icon setting is below

	if (number == 0)
	{
		newbutton->setChecked(true);
		activeFilterButton = newbutton;
	}

	auto &filter = filterManager.findFilterById(externalInfo.getId());

	// ICON SETTING
	if (!externalInfo.getIcon().empty())
	{
		newbutton->setIcon(QIcon(QString::fromStdString(fs::path(filter->resourcesRoot / externalInfo.getIcon()).string())));
	}

	int count = ui.modules_content->findChildren<QPushButton*>().count();
	count = filter->filterModules.size() - count;

	while (count-- > 0)
	{
		addModuleButton();
	}

	return true;
}

void LiveCamWindow::addFilter(bool rewrite)
{
	auto editorData = editorWindow->getEditableFilterExtraData();

	QPushButton *newbutton;
	int number;

	if (activeFilterButton)
	{
		if (rewrite)
		{
			number = activeFilterButton->property("buttonNumber").toInt();
			newbutton = activeFilterButton;
		}
		else
		{
			number = ui.filters_content->findChildren<QPushButton*>().count();
			newbutton = createFilterButton();
			addFilterButton(newbutton);
		}
	}
	else
	{
		number = 0;
		newbutton = (QPushButton*)ui.gridLayout_filters_content->itemAt(0)->widget();
	}

	if (!rewrite)
	{
		newbutton->setProperty("buttonNumber", number);
	}

	if (number == 0)
	{
		newbutton->setChecked(true);
		activeFilterButton = newbutton;
	}

	FilterUiModel newExternalData;
	newExternalData.connectedButton = newbutton;
	newExternalData.setTitle(editorData.title);
	newExternalData.setIcon(fs::path(editorData.filterIconPath).filename().string());

	newbutton->setIcon(QIcon(editorData.filterIcon));

	if (!rewrite)
	{
		int templatedID = filterManager.addFilter(editorWindow->getEditableFilter());
		newExternalData.setId(templatedID);

		filterUiModels.push_back(newExternalData);
	}
	else
	{
		filterManager.filters[filterManager.getCurrentFilterID()] = editorWindow->editableFilter;

		newExternalData.setId(filterUiModels[number].getId());
		filterUiModels[number] = newExternalData;
	}
}


void LiveCamWindow::setRawLeftWidget(CVImageWidget* imageWidget){
	rawLeftWidget = imageWidget;
	QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(0);

	vbox->addWidget(imageWidget, 0, Qt::AlignCenter);
    //ui.rawLeftFrame->setLayout(vbox);
    ui.widget_raw_frame->setLayout(vbox);
}

void LiveCamWindow::setCompositeLeftWidget(CVImageWidget* imageWidget){
	compositeLeftWidget = imageWidget;
	QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setContentsMargins(0,0,0,0);
    vbox->setSpacing(0);

    vbox->addWidget(imageWidget, 0, Qt::AlignCenter);
    //ui.compositeLeftFrame->setLayout(vbox);
    ui.widget_composite_frame->setLayout(vbox);
}

void LiveCamWindow::closeEvent(QCloseEvent*)
{
	closeCameras();
}

void LiveCamWindow::on_devices_comboBox_currentIndexChanged(int index)
{
	qDebug() << "on_devices_comboBox_currentIndexChanged index change" << index;
	qDebug() << "- change" << ui.devices_comboBox->currentIndex();
}

void LiveCamWindow::on_btn_startEffectLeft_clicked()
{	
	if (!filterManager.getCurrentFilter())
	{
		return;
	}

	if (!isFilterStarted)
	{
		if (showComposite(LEFT_VIEWER, true, filterManager.getCurrentFilter()))
		{
			ui.btn_startEffectLeft->setText(stopEffectString);
		}

		if (isVideoDisplaying)
		{
			ui.takeButton->setEnabled(true);
		}

		isFilterStarted = true;
	}
	else
	{
		ui.btn_startEffectLeft->setText(startEffectString);
		showComposite(LEFT_VIEWER, false, nullptr);

		ui.takeButton->setChecked(false);
		ui.takeButton->setText(showFilterString);
		ui.takeButton->setEnabled(false);
		isFilterDisplaying = false;

		isFilterStarted = false;
	}
}

void LiveCamWindow::initVideoFileList()
{
	using namespace boost::filesystem;

	path p("./videos/");

	if (is_directory(p)) {
		std::cout << p << " is a directory containing:\n";

		for (auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
		{
			QString qstring = QString::fromStdString(entry.path().filename().generic_string());

			ui.devices_comboBox->addItem(qstring);
		}
			
	}
}

void LiveCamWindow::on_outputButton_clicked() {
	//outputToWindow();
	if (!isVideoDisplaying) {
		isVideoDisplaying = true;
 		ui.outputButton->setText(stopOutputString);

		if (isFilterStarted || !ui.btn_startEffectLeft_Container->isEnabled())
		{
			ui.takeButton->setEnabled(true);
		}
	}
	else
	{
		isVideoDisplaying = false;
		ui.outputButton->setText(startOutputString);

		ui.takeButton->setChecked(false);
		ui.takeButton->setText(showFilterString);
		ui.takeButton->setEnabled(false);
		isFilterDisplaying = false;
	}
}

void LiveCamWindow::on_takeButton_clicked()
{
	if (!isFilterDisplaying){
		isFilterDisplaying = true;
		ui.takeButton->setText(noShowFilterString);
	}
	else
	{
		isFilterDisplaying = false;
		ui.takeButton->setText(showFilterString);
	}

	qDebug() << "on_takeButton_clicked change" ;	
}

void LiveCamWindow::on_btn_startFeedLeft_clicked(bool checked){

	//initCameraThread();

	if (ui.devices_comboBox->currentIndex() < QCameraInfo::availableCameras().size())
	{
		startCamera(ui.devices_comboBox->currentIndex());
	}
	else
	{
		startVideoFile(ui.devices_comboBox->currentText().toStdString());
	}

	isVideoStarted = true;

	if (filterManager.getCurrentFilter())
	{
		ui.btn_startEffectLeft->setEnabled(true);
	}

	ui.outputButton->setEnabled(true);

	ui.create_filter->setEnabled(true);

	if (activeFilterButton && filterManager.getCurrentFilter()->editable)
	{
		ui.edit_filter->setEnabled(true);
	}
}

void LiveCamWindow::on_leftViewerFxComboBox_currentIndexChanged(int index){
	setFilter(index);
}

void LiveCamWindow::runTracker2(){

}

void LiveCamWindow::timerHandler(){
	//qDebug() << "timerHandler";
	runTracker();
}

void LiveCamWindow::stopTimer(){
    timer->stop();
}


void LiveCamWindow::initCameraAndVideoFileList() {
	initCameraList();
	initVideoFileList();
}

void LiveCamWindow::initCameraList(){
    // Getting devices list and adding to UI.
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(false);
    foreach(const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
        QAction *videoDeviceAction = new QAction(cameraInfo.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraInfo));
        if (cameraInfo == QCameraInfo::defaultCamera()){
            videoDeviceAction->setChecked(true);
        }
        ui.devices_comboBox->addItem(cameraInfo.description());
    }

    if (QCameraInfo::availableCameras().length()==0){
		cv::VideoCapture camera;
        int device_counts = 0;
        while (true) {
            if (!camera.open(device_counts++)) {
                break;
            }
            QString qstring;
            QTextStream(&qstring) << "Camera " << device_counts;
            ui.devices_comboBox->addItem(qstring);
        }
        camera.release();
    }
}

void LiveCamWindow::on_actionVideo_Output_Window_triggered(){
	// open OutputWindow just from here, now it is a member of LiveCamWindow
}

void LiveCamWindow::on_actionExit_triggered(){
	closeCameras();
}

void LiveCamWindow::exitCameraThread(){
	if (cameraThread->isRunning()){
		cameraThread->exit();
	}
}

void LiveCamWindow::initCameraThread(){
	cameraThread = new QThread;
	cameraWorker = new CameraWorker(0);
	cameraWorker->moveToThread(cameraThread);
	connect(cameraWorker, SIGNAL(frameFinished(cv::Mat3b, int)), this, SLOT(displayFrame(cv::Mat3b, int)));
	connect(cameraThread, SIGNAL(started()), cameraWorker, SLOT(readVideo()));

	connect(cameraWorker, SIGNAL(finished(int)), cameraThread, SLOT(quit()));
	connect(cameraWorker, SIGNAL(finished(int)), cameraWorker, SLOT(deleteLater()));
	connect(cameraThread, SIGNAL(finished()), cameraThread, SLOT(deleteLater()));

	cameraThread->start();
}

void LiveCamWindow::displayFrame(cv::Mat3b frame, int index){
	rawLeftWidget->showImage(frame);

	//cv::Mat outputImage;
	// create float array for storing tracker values
	//float trackerPresets[FaceTracker::MAX_TO_TRACK][FaceTracker::NUMBER_OF_PRESET_VALUES];
	//const float* trackerRawPoints[FaceTracker::MAX_TO_TRACK][2];
	// Get tracker values from tracker..
	//isTracking = faceTracker.trackerValuesFromMat(frame, trackerRawPoints, trackerPresets);

	// add the tracker values array to the opengl class and return the output image
	//outputImage = openGlMain.addTrackerValues(frame0, trackerRawPoints, trackerPresets, isTracking, isTakingOutput);

//	cv::flip(outputImage, outputImage, 0);
	outputImageWidget->showImage(frame);
}

void LiveCamWindow::setFilter(std::shared_ptr<FxCore> filter)
{
	sendEffectToOpenGL(filter);
}

void LiveCamWindow::setFilter(int value){
	if (filterUiModels.size() < value + 1) return;

	QString title = QString::fromUtf8(filterUiModels[value].getTitle().c_str());
	ui.label_current_filter->setText(title);

	loadFilter(filterUiModels[value].getId());

	ui.delete_filter->setEnabled(filterManager.getCurrentFilter()->editable);

	if (isVideoStarted)
	{
		ui.edit_filter->setEnabled(filterManager.getCurrentFilter()->editable);
	}

	animatedSubmenuReplacing(filterManager.getCurrentFilter());
}

void LiveCamWindow::on_pushButton_filter_clicked(){
	QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());
	qDebug() << "button " << buttonSender->property("buttonNumber").toString();
	if (activeFilterButton == buttonSender)
	{
		return;
	}
	activeFilterButton = buttonSender;
	setFilter(buttonSender->property("buttonNumber").toInt());
}

bool LiveCamWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress ) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
        if (mouse_event->button() != Qt::LeftButton) return false;
        if (qobject_cast<QPushButton *>(obj) == nullptr) return false;

        qDebug() << mouse_event;

		mouse_event->pos();

		QPushButton* button = qobject_cast<QPushButton*>(obj);

        QPixmap pixmap = QPixmap(button->icon().pixmap(QSize(60, 60)));

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

		mimeData->setImageData(pixmap);
		mimeData->setProperty("filterModule", button->property("filterModule"));

		drag->setMimeData(mimeData);
        drag->setHotSpot(QPoint(0,0));
        drag->setPixmap(pixmap);

        drag->exec();
    }

    if (event->type() == QEvent::MouseButtonDblClick ) {
        QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
        if (mouse_event->button() != Qt::LeftButton) return false;
        if (qobject_cast<QPushButton *>(obj) == nullptr) return false;

        qDebug() << mouse_event;

		doubleClickSelection((FilterModule*)obj->property("filterModule").toULongLong());
    }

    if (event->type() == QEvent::DragEnter) {
        qDebug() << "QEvent::DragEnter";
		isDraggingOver = true;
		usingDragDrop(isDraggingOver);
        QDragEnterEvent *drag_enter_event = static_cast<QDragEnterEvent *>(event);
        drag_enter_event->acceptProposedAction();
    }

	if (isDraggingOver){
		if (event->type() == QEvent::DragMove) {
			QDragMoveEvent *drag_move_event = static_cast<QDragMoveEvent *>(event);
			qDebug() << "QEvent::DragMove" << drag_move_event->pos();
			drag_move_event->acceptProposedAction();
			draggingOver(drag_move_event->pos());
		}
	}

	if (event->type() == QEvent::DragLeave) {
		qDebug() << "QEvent::DragLeave";
		isDraggingOver = false;
		usingDragDrop(false);
	}
	
    if (event->type() == QEvent::Drop) {

		int index = obj->property("moduleID").toInt();
	
        QDropEvent *drop_event = static_cast<QDropEvent *>(event);
        drop_event->acceptProposedAction();

		auto s = drop_event->mimeData()->property("filterModule").toULongLong();
		dropImage((FilterModule*)drop_event->mimeData()->property("filterModule").toULongLong());

		isDraggingOver = false;
		usingDragDrop(false);
    }

    return false;
}

void LiveCamWindow::installEventFilters()
{
    ui.widget_composite_frame->installEventFilter(this);

	for (QPushButton *pb : ui.modules_list->findChildren<QPushButton *>()) {
		pb->installEventFilter(this);
    }
}

void LiveCamWindow::onCamEventAction(QString camFileName, int index)
{
    //qDebug() << "Cam effect triggered: icon " << camFileName;
	//dropImage(camFileName, index);
}

void LiveCamWindow::animatedSubmenuReplacing(std::shared_ptr<FxCore> newSettedFilter)
{
	QGridLayout *filterModulesTableGrid = findChild<QGridLayout*>("gridLayout_modules_content");
	QWidget *filterModulesTable = filterModulesTableGrid->parentWidget();

	QGraphicsBlurEffect *blur1 = new QGraphicsBlurEffect();
	QPropertyAnimation *blurIncreasing = new QPropertyAnimation(blur1, "blurRadius");
	blurIncreasing->setStartValue(1);
	blurIncreasing->setEndValue(10);
	blurIncreasing->setDuration(100);

	QGraphicsBlurEffect *blur2 = new QGraphicsBlurEffect();
	QPropertyAnimation* blurDecreasing = new QPropertyAnimation(blur2, "blurRadius");
	blurDecreasing->setStartValue(10);
	blurDecreasing->setEndValue(1);
	blurDecreasing->setDuration(100);

	filterModulesTable->setGraphicsEffect(blur1);
	blurIncreasing->start();

    connect(blurIncreasing, &QPropertyAnimation::finished, [=]()
	{
		int i = 0;

		if (newSettedFilter)
		{
			for (; i < newSettedFilter->filterModules.size(); ++i)
			{
				QPushButton *button = qobject_cast<QPushButton*>(filterModulesTableGrid->itemAt(i)->widget());
				button->setIcon(newSettedFilter->filterModules[i].icon);
				button->setVisible(true);
				button->setProperty("filterModule", (qulonglong)&newSettedFilter->filterModules[i]);
			}
		}

		QLayoutItem *item;

		while ((item = filterModulesTableGrid->itemAt(i)) != nullptr)
		{
			item->widget()->setVisible(false);
			((QPushButton*)item->widget())->setIcon(QIcon());
			++i;
		}

		filterModulesTable->setGraphicsEffect(blur2);
		blurDecreasing->start();
	});
}

void LiveCamWindow::animatedMainMenuReplacing(bool swapToEditor)
{
	QGridLayout *filterTableGrid = findChild<QGridLayout*>("gridLayout_filters_content");
	QWidget *filterTable = filterTableGrid->parentWidget();

	QGraphicsBlurEffect *blur1 = new QGraphicsBlurEffect();
	QPropertyAnimation *blurIncreasing = new QPropertyAnimation(blur1, "blurRadius");
	blurIncreasing->setStartValue(1);
	blurIncreasing->setEndValue(10);
	blurIncreasing->setDuration(100);

	QGraphicsBlurEffect *blur2 = new QGraphicsBlurEffect();
	QPropertyAnimation* blurDecreasing = new QPropertyAnimation(blur2, "blurRadius");
	blurDecreasing->setStartValue(10);
	blurDecreasing->setEndValue(1);
	blurDecreasing->setDuration(100);

	filterTable->setGraphicsEffect(blur1);
	blurIncreasing->start();

	connect(blurIncreasing, &QPropertyAnimation::finished, [=]()
	{
		if (swapToEditor)
		{
			bool firstButton = true;
			for (int i = 0; i < filterUiModels.size() || firstButton; ++i)
			{
				QPushButton *button = qobject_cast<QPushButton*>(filterTableGrid->itemAt(i)->widget());
				if (firstButton)
				{
					QIcon icon;
					icon.addPixmap(QPixmap(QString::fromStdString(editorWindow->data.filterIconPath)), QIcon::Disabled);
					button->setIcon(icon);
					button->setVisible(true);
					firstButton = false;

					if (activeFilterButton)
					{
						activeFilterButton->setChecked(false);
					}
				}
				else
				{
					button->setVisible(false);
				}
			}
		}
		else
		{
			if (filterUiModels.size() == 0)
			{
				((QPushButton*)filterTableGrid->itemAt(0)->widget())->setVisible(false);
			}
			else
			{
				bool firstButton = true;
				for (int i = 0; i < filterUiModels.size() || firstButton; ++i)
				{
					QPushButton *button = qobject_cast<QPushButton*>(filterTableGrid->itemAt(i)->widget());
					if (firstButton)
					{
						std::string iconPath = boost::filesystem::path(
							filterManager.findFilterById(filterUiModels.front().getId())->resourcesRoot / filterUiModels.front().getIcon()
						).string();

						button->setIcon(QIcon(QString::fromStdString(iconPath)));
						
						if (activeFilterButton)
						{
							activeFilterButton->setChecked(true);
						}

						firstButton = false;
					}
					else
					{
						button->setVisible(true);
					}
				}
			}
		}

		filterTable->setGraphicsEffect(blur2);
		blurDecreasing->start();
	});
}

void LiveCamWindow::on_create_filter_clicked()
{
	ui.filters_list->setEnabled(false);
	ui.modules_list->setEnabled(false);
	ui.editor_buttons->setEnabled(false);
	ui.btn_startEffectLeft->setEnabled(false);
	ui.btn_startEffectLeft_Container->setEnabled(false);
	ui.devices_comboBox->setEnabled(false);

	if (isVideoDisplaying)
	{
		ui.takeButton->setEnabled(true);
	}

	editorWindow->show();

	auto newSettedFilter = editorWindow->getEditableFilter();

	ui.label_current_filter->setText(editorWindow->ui.filterName_edit->text());
	animatedSubmenuReplacing(newSettedFilter);
	animatedMainMenuReplacing(true);
	showComposite(LEFT_VIEWER, true, newSettedFilter);
}

void LiveCamWindow::on_editor_filterIcon_changed()
{
	animatedMainMenuReplacing(true);
}

void LiveCamWindow::on_editor_moduleIcon_changed()
{
	animatedSubmenuReplacing(editorWindow->getEditableFilter());
}

void LiveCamWindow::on_edit_filter_clicked()
{
	ui.filters_list->setEnabled(false);
	ui.modules_list->setEnabled(false);
	ui.editor_buttons->setEnabled(false);
	ui.btn_startEffectLeft->setEnabled(false);
	ui.btn_startEffectLeft_Container->setEnabled(false);
	ui.devices_comboBox->setEnabled(false);

	if (isVideoDisplaying)
	{
		ui.takeButton->setEnabled(true);
	}

	editorWindow->readFilterProperties(filterManager.getCurrentFilter(), 
		filterUiModels[activeFilterButton->property("buttonNumber").toInt()]);

	auto newSettedFilter = editorWindow->getEditableFilter();

	ui.label_current_filter->setText(editorWindow->ui.filterName_edit->text());
	animatedSubmenuReplacing(newSettedFilter);
	animatedMainMenuReplacing(true);
	showComposite(LEFT_VIEWER, true, newSettedFilter);

	editorWindow->show();
}

void LiveCamWindow::on_editor_window_closed()
{
	ui.filters_list->setEnabled(true);
	ui.modules_list->setEnabled(true);
	ui.editor_buttons->setEnabled(true);
	ui.btn_startEffectLeft->setEnabled(true);
	ui.btn_startEffectLeft_Container->setEnabled(true);
	ui.devices_comboBox->setEnabled(true);

	if (!isFilterStarted)
	{
		ui.takeButton->setEnabled(false);
	}

	if (ui.btn_startEffectLeft->text() == startEffectString)
	{
		showComposite(LEFT_VIEWER, false, nullptr);
	}

	animatedMainMenuReplacing(false);

	if (activeFilterButton)
	{
		setFilter(activeFilterButton->property("buttonNumber").toInt());
	}
	else
	{
		animatedSubmenuReplacing(nullptr);
	}
}

void LiveCamWindow::on_delete_filter_clicked()
{
	fs::path deletePath = fs::canonical(filterManager.getCurrentFilter()->resourcesRoot);

	auto choice = QMessageBox::question(nullptr, "Delete " + ui.label_current_filter->text(),
		"Are you sure to delete this filter?\n\n" + QString::fromStdString(deletePath.string()));

	if (choice == QMessageBox::No)
	{
		return;
	}

	int index = activeFilterButton->property("buttonNumber").toInt();

	ui.gridLayout_filters_content->removeItem(ui.gridLayout_filters_content->itemAt(index));
	activeFilterButton->hide();
	delete activeFilterButton;

	filterUiModels.erase(filterUiModels.begin() + index);

	fs::remove_all(deletePath);

	std::vector<QLayoutItem*> crutch; // temporary?

	for (int i = index; i < filterUiModels.size(); ++i)
	{
		auto item = ui.gridLayout_filters_content->itemAt(index);
		ui.gridLayout_filters_content->removeItem(item);
		crutch.push_back(item);
	}

	int i = index;
	for (auto item : crutch)
	{
		ui.gridLayout_filters_content->addItem(item, i / 4, i % 4);
		item->widget()->setProperty("buttonNumber", i++);
	}

	if (index >= filterUiModels.size() && filterUiModels.size() > 0)
	{
		index = filterUiModels.size() - 1;
	}

	if (index < filterUiModels.size())
	{
		QPushButton* buttonSender = (QPushButton*)ui.gridLayout_filters_content->itemAt(index)->widget();
		activeFilterButton = buttonSender;
		activeFilterButton->setChecked(true);
		setFilter(index);
	}
	else
	{
		QPushButton *newbutton = createFilterButton();
		newbutton->setProperty("buttonNumber", 0);
		newbutton->setVisible(false);
		addFilterButton(newbutton);

		showComposite(LEFT_VIEWER, false, nullptr);
		activeFilterButton = nullptr;
		ui.delete_filter->setEnabled(false);
		ui.edit_filter->setEnabled(false);
		ui.btn_startEffectLeft->setEnabled(false);
		ui.label_current_filter->setText("");
		animatedSubmenuReplacing(nullptr);
	}
}

void LiveCamWindow::on_import_filter_clicked()
{
	auto JSONsource = QFileDialog::getOpenFileName(nullptr, tr("Choose a Filter Config to Load"),
		editorWindow->filterFolderStart, "filter-config.json");

	if (JSONsource.isEmpty())
	{
		return;
	}

	editorWindow->filterFolderStart = QFileInfo(JSONsource).dir().path();

	FilterUiModel externalInfo;
	externalInfo.setJSONsource(JSONsource.toStdString());

	addFilter(externalInfo);
	filterUiModels.push_back(externalInfo);

	auto filterFolder = fs::path(JSONsource.toStdString()).parent_path();
	fs::copy_directory(filterFolder, "./assets/fx_config/custom-filters/" / filterFolder.filename());
}