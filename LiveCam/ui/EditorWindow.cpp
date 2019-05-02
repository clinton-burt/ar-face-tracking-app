#include "EditorWindow.h"

QPushButton* EditorWindow::createIconButton()
{
	auto filterIcon_button = new QPushButton(ui.filterIcon_container);
	filterIcon_button->setObjectName("element_button");
	filterIcon_button->setMaximumSize(QSize(32, 32));
	filterIcon_button->setIcon(QIcon(":/images/FileButton.png"));
	filterIcon_button->setIconSize(QSize(32, 32));
	filterIcon_button->setFlat(true);

	return filterIcon_button;
}

EditorWindow::EditorWindow(LiveCamWindow *liveCamWindow, QWidget *parent) : QMainWindow(parent)
{
	fileDialogStart = "./assets/";
	filterFolderStart = "./assets/fx_config/custom-filters";

	this->liveCamWindow = liveCamWindow;

	ui.setupUi(this);

	ui.menubar->hide();
	ui.statusbar->hide();

	ui.filterIcon_delete->setVisible(false);

	chooseImageIcon = QIcon(":/images/FileButton.png");

	resetEditableFilter();

	//installEventFilters();
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::resetEditableFilter()
{
	editableFilter = make_shared<FxCore>();

	editableFilter->editable = true;

	data = ExtraFilterData();
	data.filter = editableFilter;

	ui.filterIcon_button->setIcon(chooseImageIcon);

	ui.filterName_edit->setText("New Filter");
	data.title = ui.filterName_edit->text().toStdString();

	int curModelIndex = ui.models_tabWidget->currentIndex();

	ui.models_tabWidget->blockSignals(true);
	while (ui.models_tabWidget->count() > 1)
	{
		ui.models_tabWidget->removeTab(0);
	}
	ui.models_tabWidget->blockSignals(false);

	ui.save_button->setEnabled(false);
	ui.export_button->setEnabled(false);
}

std::shared_ptr<FxCore> EditorWindow::getEditableFilter()
{
	return editableFilter;
}

ExtraFilterData EditorWindow::getEditableFilterExtraData()
{
	return data;
}

bool EditorWindow::event(QEvent * e)
{
	switch (e->type())
	{
	case QEvent::WindowActivate:
		break;
	};
	return QMainWindow::event(e);
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
	resetEditableFilter();

	QMainWindow::closeEvent(event);

	emit editor_window_closed();
}

void EditorWindow::readFilterProperties(std::shared_ptr<FxCore> filter, FilterUiModel& externalInfo)
{
	resetEditableFilter();

	data.resourcesRoot = filter->resourcesRoot.string();

	editableFilter->resourcesRoot = filter->resourcesRoot;

	resourceManager.resourceRoot = filter->resourcesRoot;

	int index = 0;
	for (auto model : filter->models)
	{
		std::string type = model->getTypeName();

		if (type == OpenGLEmojiModel::TYPE_NAME)
		{
			auto tab = createEmojiModelTab();
			tab->readEmojiModelProperties(filter.get(), (OpenGLEmojiModel*)model.get(), filter->filterModules[index].suits[index],
				filter->resourcesRoot);
		}
		else if (type == OpenGL3DModel::TYPE_NAME)
		{
			auto tab = createModel3DTab(false);
			tab->read3DModelProperties(filter.get(), (OpenGL3DModel*)model.get(), (OpenGL3DModel*)editableFilter->models.back().get(),
				filter->filterModules[index].suits[index], filter->resourcesRoot);
		}
		else if (type == LipsJoint3DModel::TYPE_NAME)
		{
			auto tab = createModel3DTab(true);
			tab->read3DModelProperties(filter.get(), (OpenGL3DModel*)model.get(), (OpenGL3DModel*)editableFilter->models.back().get(),
				filter->filterModules[index].suits[index], filter->resourcesRoot);
		}

		editableFilter->models.back()->name = model->name;

		if (!filter->filterModules[index].iconPath.empty())
		{
			setElementIcon(QString::fromStdString(
				fs::path(filter->resourcesRoot / filter->filterModules[index].iconPath).string()));
		}

		ui.models_tabWidget->setTabIcon(index, data.modules[index].moduleIcon);
		ui.models_tabWidget->setTabText(index, QString::fromStdString(model->name));

		++index;
	}

	if (editableFilter->models.size() > 0)
	{
		ui.save_button->setEnabled(true);
		ui.export_button->setEnabled(true);
	}

	ui.filterName_edit->setText(QString::fromStdString(externalInfo.getTitle()));
	data.title = externalInfo.getTitle();

	if (!externalInfo.getIcon().empty())
	{
		setFilterIcon(QString::fromStdString(fs::path(filter->resourcesRoot / externalInfo.getIcon()).string()));
	}

	ui.models_tabWidget->setCurrentIndex(0);
}

void EditorWindow::setFilterIcon(QString& iconPath)
{
	data.filterIcon = QPixmap(iconPath);
	data.filterIconPath = iconPath.toStdString();

	if (data.filterIcon.isNull())
	{
		ui.filterIcon_button->setIcon(QIcon(chooseImageIcon));
		ui.filterIcon_delete->setVisible(false);
	}
	else
	{
		ui.filterIcon_button->setIcon(QIcon(data.filterIcon));
		ui.filterIcon_delete->setVisible(true);
	}
}

void EditorWindow::setElementIcon(QString& iconPath)
{
	auto &moduleData = data.modules[ui.models_tabWidget->currentIndex()];

	moduleData.moduleIcon = QPixmap(iconPath);
	moduleData.moduleIconPath = iconPath.toStdString();

	QIcon icon;

	if (moduleData.moduleIcon.isNull())
	{
		icon = chooseImageIcon;

		editableFilter->filterModules[ui.models_tabWidget->currentIndex()].iconPath = "";
		editableFilter->filterModules[ui.models_tabWidget->currentIndex()].icon = QIcon();
	}
	else
	{
		QFileInfo fileInfo(iconPath);
		icon.addPixmap(moduleData.moduleIcon, QIcon::Normal);
		icon.addPixmap(moduleData.moduleIcon, QIcon::Disabled);

		editableFilter->filterModules[ui.models_tabWidget->currentIndex()].iconPath = fileInfo.fileName().toStdString();
		editableFilter->filterModules[ui.models_tabWidget->currentIndex()].icon = icon;
	}

	ui.models_tabWidget->setTabIcon(ui.models_tabWidget->currentIndex(), icon);
}

EmojiModelTab* EditorWindow::createEmojiModelTab()
{
	EmojiModelTab *emojiTab = new EmojiModelTab(this);

	ui.models_tabWidget->insertTab(ui.models_tabWidget->count() - 1, emojiTab, "New Emoji Model");
	ui.models_tabWidget->setTabIcon(ui.models_tabWidget->count() - 2, chooseImageIcon);

	// new model

	auto emoji = make_shared<OpenGLEmojiModel>();
	emoji->name = "New Emoji Model";
	emoji->useHardCodedUniforms = false;
	emoji->transformVersion = TransformVersion::V2;
	emoji->emojisTextures.fill("");

	// emoji model default objectRenderParams

	auto renderParams = emoji->createDefaultObjectRenderParams();
	emoji->objectRenderParams.fill({ renderParams });

	emoji->onInputFrameResized();

	ExtraRenderParamsData renderParamsData(&data);

	// new filter module & adding suits

	boost::property_tree::ptree disablingSuit;
	disablingSuit.put("visible", "false");

	FilterModule newModule;

	for (auto model : editableFilter->models) // disable the new model for other modules
	{
		newModule.suits.push_back(disablingSuit);
	}

	for (auto &module : editableFilter->filterModules) // disable other models for the new module
	{
		module.suits.push_back(disablingSuit);
	}

	// push & load for render at once

	editableFilter->filterModules.push_back(newModule);
	editableFilter->filterModules.back().suits.push_back(boost::property_tree::ptree());
	editableFilter->models.push_back(emoji);

	ExtraModelData modelData = ExtraModelData(&data);
	modelData.renderParams.push_back(renderParamsData);

	data.modules.push_back(ExtraModuleData(&data));
	data.modules.back().models.push_back(modelData);

	editableFilter->models.back()->load();

	// check enough count of filter modules buttons

	auto &list = liveCamWindow->ui.modules_content->findChildren<QPushButton*>();

	int total = list.count();

	int visible = std::count_if(list.begin(), list.end(), [](QPushButton* button) -> bool
	{
		return button->isVisible();
	});

	if (editableFilter->filterModules.size() > total)
	{
		auto newbutton = liveCamWindow->addModuleButton();
		newbutton->setVisible(true);
	}
	else if (editableFilter->filterModules.size() > visible)
	{
		liveCamWindow->ui.gridLayout_modules_content->itemAt(visible)->widget()->setVisible(true);
	}

	// call event
	ui.models_tabWidget->setCurrentWidget(emojiTab);

	return emojiTab;
}

Model3DTab* EditorWindow::createModel3DTab(bool lipsJoint)
{
	Model3DTab *modelTab = new Model3DTab(this);

	int curIndex = ui.models_tabWidget->count() - 1;

	ui.models_tabWidget->insertTab(curIndex, modelTab, "New 3D Model");
	ui.models_tabWidget->setTabIcon(curIndex, chooseImageIcon);

	// new model

	std::shared_ptr<OpenGL3DModel> model = lipsJoint ? make_shared<LipsJoint3DModel>() : make_shared<OpenGL3DModel>();
	model->useHardCodedUniforms = false;
	model->transformVersion = TransformVersion::V2;
	model->name = "New 3D Model";
	model->texturesPaths.fill( { "" } );

	model->onInputFrameResized();

	// new filter module & adding suits

	boost::property_tree::ptree disablingSuit;
	disablingSuit.put("visible", "false");

	FilterModule newModule;

	for (auto model : editableFilter->models) // disable the new model for other modules
	{
		newModule.suits.push_back(disablingSuit);
	}

	for (auto &module : editableFilter->filterModules) // disable other models for the new module
	{
		module.suits.push_back(disablingSuit);
	}

	// push & load for render at once

	editableFilter->filterModules.push_back(newModule);
	editableFilter->filterModules.back().suits.push_back(boost::property_tree::ptree());
	editableFilter->models.push_back(model);

	data.modules.push_back(ExtraModuleData(&data));
	data.modules.back().models.push_back(ExtraModelData(&data));

	editableFilter->models.back()->load();

	// check enough count of filter modules buttons

	auto &list = liveCamWindow->ui.modules_content->findChildren<QPushButton*>();

	int total = list.count();

	int visible = std::count_if(list.begin(), list.end(), [](QPushButton* button) -> bool
	{
		return button->isVisible();
	});

	if (editableFilter->filterModules.size() > total)
	{
		auto newbutton = liveCamWindow->addModuleButton();
		newbutton->setVisible(true);
	}
	else if (editableFilter->filterModules.size() > visible)
	{
		liveCamWindow->ui.gridLayout_modules_content->itemAt(visible)->widget()->setVisible(true);
	}

	// call event
	ui.models_tabWidget->setCurrentWidget(modelTab);

	return modelTab;
}

void EditorWindow::saveFilter(QString& filterFolder)
{
	auto relativePath = "." / fs::relative(filterFolder.toStdString());

	auto existingJSON = relativePath / "filter-config.json";
	if (fs::exists(existingJSON))
	{
		boost::property_tree::ptree tree;
		boost::property_tree::json_parser::read_json(existingJSON.string(), tree);

		auto choice = QMessageBox::question(nullptr, "Rewrite " + QString::fromStdString(tree.get<std::string>("title", "Unnamed filter")),
			"Are you sure to rewrite this filter?\n\n" + QString::fromStdString(relativePath.string()));

		if (choice == QMessageBox::No)
		{
			return;
		}
	}

	fs::create_directory(relativePath);

	bool rewrite = false;
	if (fs::equivalent(relativePath, editableFilter->resourcesRoot))
	{
		rewrite = true;
	}
	else
	{
		editableFilter->resourcesRoot = relativePath;
	}

	editableFilter->saveToJSON(relativePath, data);

	liveCamWindow->addFilter(rewrite);
	liveCamWindow->ui.btn_startEffectLeft->setEnabled(true);

	resetEditableFilter();
	
	close();
}

// ======================= SLOTS =======================

void EditorWindow::on_hide_button_clicked()
{
	close();
}

void EditorWindow::on_tab_deleted()
{
	int curModelIndex = ui.models_tabWidget->currentIndex();

	ui.models_tabWidget->removeTab(curModelIndex);

	editableFilter->models.erase(editableFilter->models.begin() + curModelIndex);
	editableFilter->filterModules.erase(editableFilter->filterModules.begin() + curModelIndex);

	data.modules.erase(data.modules.begin() + curModelIndex);

	for (auto &module : editableFilter->filterModules)
	{
		module.suits.erase(module.suits.begin() + curModelIndex);
	}

	if (editableFilter->models.size() == 0)
	{
		ui.save_button->setEnabled(false);
		ui.export_button->setEnabled(false);
	}

	liveCamWindow->on_editor_moduleIcon_changed();
}

void EditorWindow::on_save_button_clicked()
{
	QString filterFolder = QDir::currentPath() + "/assets/fx_config/custom-filters/" + ui.filterName_edit->text();
	
	saveFilter(filterFolder);
}

void EditorWindow::on_export_button_clicked()
{
	auto filterFolder = QFileDialog::getExistingDirectory(nullptr, tr("Choose a Filter Directory to Save"),
		filterFolderStart, QFileDialog::ShowDirsOnly);

	if (filterFolder.isEmpty())
	{
		return;
	}

	filterFolderStart = filterFolder;

	saveFilter(filterFolder);
}

void EditorWindow::on_import_button_clicked()
{
	auto JSONsource = QFileDialog::getOpenFileName(nullptr, tr("Choose a Filter Config to Load"),
		"./assets/fx_config/custom-filters/", "filter-config.json");

	if (JSONsource.isEmpty())
	{
		return;
	}

	FilterUiModel externalInfo;
	externalInfo.setJSONsource(JSONsource.toStdString());

	auto filter = make_shared<FxTemplate>(&externalInfo);

	readFilterProperties(filter, externalInfo);

	liveCamWindow->ui.label_current_filter->setText(ui.filterName_edit->text());
	liveCamWindow->animatedSubmenuReplacing(editableFilter);
	liveCamWindow->animatedMainMenuReplacing(true);
	showComposite(LEFT_VIEWER, true, editableFilter);
}

void EditorWindow::on_filterName_edit_textEdited(const QString &text)
{
	liveCamWindow->ui.label_current_filter->setText(text);

	data.title = text.toStdString();
}

void EditorWindow::on_filterIcon_button_clicked()
{
	auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Choose a Filter Icon"), fileDialogStart);

	if (fileName.isEmpty())
	{
		return;
	}

	fileDialogStart = QFileInfo(fileName).dir().path();

	setFilterIcon(fileName);
}

void EditorWindow::on_filterIcon_delete_clicked()
{
	QString fileName = "";
	setFilterIcon(fileName);
}

void EditorWindow::on_models_tabWidget_currentChanged(int index)
{
	if (index == ui.models_tabWidget->count() - 1)
	{
// 		ui.elementIcon_button->setEnabled(false);
// 		ui.elementIcon_button->setIcon(chooseImageIcon);
		return;
	}

// 	ui.elementIcon_button->setEnabled(true);
// 	ui.elementIcon_button->setIcon(ui.models_tabWidget->tabIcon(ui.models_tabWidget->currentIndex()));

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editableFilter->applyModule(&editableFilter->filterModules[index], i, true);
	}
}

void EditorWindow::on_new_emoji_clicked()
{
	ui.save_button->setEnabled(true);
	ui.export_button->setEnabled(true);

	createEmojiModelTab();
}

void EditorWindow::on_new_3D_clicked()
{
	ui.save_button->setEnabled(true);
	ui.export_button->setEnabled(true);

	createModel3DTab(false);
}

void EditorWindow::on_new_LipsJoint_clicked()
{
	ui.save_button->setEnabled(true);
	ui.export_button->setEnabled(true);

	createModel3DTab(true);
}