#include "Model3DTab.h"

Model3DTab::Model3DTab(EditorWindow *editorWindow, QWidget *parent)
	: QWidget(parent)
{
	this->editorWindow = editorWindow;

	ui.setupUi(this);

	ui.elementIcon_delete->setVisible(false);
	ui.OBJfile_delete->setVisible(false);
	ui.MTLfile_delete->setVisible(false);

	connect(ui.delete_button, SIGNAL(clicked()), editorWindow, SLOT(on_tab_deleted()));
	connect(ui.elementIcon_button, SIGNAL(clicked()), editorWindow->liveCamWindow, SLOT(on_editor_moduleIcon_changed()));
	connect(ui.elementIcon_delete, SIGNAL(clicked()), editorWindow->liveCamWindow, SLOT(on_editor_moduleIcon_changed()));

	objIndex = -1;
}

Model3DTab::~Model3DTab()
{
}

void Model3DTab::read3DModelProperties(FxCore* sourceFilter, OpenGL3DModel* sourceModel, OpenGL3DModel* destinyModel,
	boost::property_tree::ptree& suit, fs::path& resourcesRoot)
{
	ui.modelName_edit->setText(QString::fromStdString(sourceModel->name));

	destinyModel->modelShift = sourceModel->modelShift;
	destinyModel->modelScale = sourceModel->modelScale;

	std::vector<std::string> OBJs;

	if (sourceModel->modelList.empty())
	{
		OBJs.push_back((sourceFilter->resourcesRoot / sourceModel->modelPath).string());
	}
	else
	{
		for (auto &file : sourceModel->modelList)
		{
			OBJs.push_back((sourceFilter->resourcesRoot / file).string());
		}
	}

	setOBJ(OBJs, true);

	std::vector<std::string> MTLs;

	if (sourceModel->materialList.empty())
	{
		MTLs.push_back((sourceFilter->resourcesRoot / sourceModel->materialPath).string());
	}
	else
	{
		for (auto &file : sourceModel->materialList)
		{
			MTLs.push_back((sourceFilter->resourcesRoot / file).string());
		}
	}

	auto path = (sourceFilter->resourcesRoot / sourceModel->materialPath).string();
	setMTL(MTLs, true);

	ui.shiftX_slider->setValue(sourceModel->modelShift[0] * 100);
	ui.shiftY_slider->setValue(sourceModel->modelShift[1] * 100);
	ui.shiftZ_slider->setValue(sourceModel->modelShift[2] * 100);

	ui.scaleX_slider->setValue(sourceModel->modelScale * 100);

	int curModelIndex = editorWindow->editableFilter->models.size() - 1;

	auto& sourceParams = sourceFilter->filterModules[curModelIndex].suits[curModelIndex].get_child("renderParamsIDs");

	int index = 0;
	for (auto sourceParamsID : sourceParams)
	{
		int ID = sourceParamsID.second.get_value<int>();

		auto &shaders = sourceFilter->commonRenderParams[ID]->shadersSources;

		for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		{
			editorWindow->editableFilter->models[curModelIndex]->objectRenderParams[i][index]->shadersSources = shaders;
			editorWindow->editableFilter->models[curModelIndex]->objectRenderParams[i][index]->shader =
				shaderManagerWrapper.LoadFromFile((sourceFilter->resourcesRoot / shaders.first).string().data(),
				(sourceFilter->resourcesRoot / shaders.second).string().data());
		}

		++index;
	}

	ui.elementIcon_button->setIcon(sourceFilter->filterModules[curModelIndex].icon);
	ui.elementIcon_delete->setVisible(!sourceFilter->filterModules[curModelIndex].iconPath.empty());
}

void Model3DTab::on_modelName_edit_textEdited(const QString &text)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	editorWindow->editableFilter->models[curModelIndex]->name = text.toStdString();

	editorWindow->ui.models_tabWidget->setTabText(curModelIndex, text);
}

void Model3DTab::on_elementIcon_button_clicked()
{
	auto fileName = QFileDialog::getOpenFileName(editorWindow, tr("Choose an Element Icon"), editorWindow->fileDialogStart);

	if (fileName.isEmpty())
	{
		return;
	}

	editorWindow->fileDialogStart = QFileInfo(fileName).dir().path();

	QPixmap pixmap = QPixmap(fileName);

	if (pixmap.isNull())
	{
		ui.elementIcon_button->setIcon(QIcon(":/images/FileButton.png"));
		ui.elementIcon_delete->setVisible(false);
	}
	else
	{
		ui.elementIcon_button->setIcon(QIcon(pixmap));
		ui.elementIcon_delete->setVisible(true);
	}

	editorWindow->setElementIcon(fileName);
}

void Model3DTab::on_elementIcon_delete_clicked()
{
	QString fileName = "";
	editorWindow->setElementIcon(fileName);
}

void Model3DTab::on_objectButton_clicked()
{
	objIndex = ui.objectsListContent_verticalLayout->indexOf((QWidget*)sender());

	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &renderParamsData = editorWindow->data.modules[curModelIndex].models[0].renderParams[objIndex];

	ui.shader_list->setCurrentIndex(renderParamsData.shaderIndex);
}

void Model3DTab::on_shader_list_currentIndexChanged(int value)
{
	setShader(objIndex, ui.shader_list->currentText().toStdString());
}

void Model3DTab::setShader(size_t objectIndex, std::string name)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	OpenGL3DModel* model = (OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get();
	auto &renderParamsData = editorWindow->data.modules[curModelIndex].models[0].renderParams[objectIndex];

	auto iter = std::find_if(OpenGLModel::DefaultShadersMap.begin(), OpenGLModel::DefaultShadersMap.end(),
		[name](const OpenGLModel::DefaultShadersMapElement &E) -> bool
		{
			return E.first == name;
		}
		);
	auto shaders = iter->second;
	int shaderIndex = std::distance(OpenGLModel::DefaultShadersMap.begin(), iter);

	shaders.first = fs::relative("./assets/shaders/vertex/" + shaders.first, fs::current_path() / editorWindow->editableFilter->resourcesRoot).string();
	shaders.second = fs::relative("./assets/shaders/fragment/" + shaders.second, fs::current_path() / editorWindow->editableFilter->resourcesRoot).string();
	renderParamsData.shaderIndex = shaderIndex;

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		model->objectRenderParams[i][objectIndex]->shadersSources = shaders;
		model->objectRenderParams[i][objectIndex]->shader =
			shaderManagerWrapper.LoadFromFile(fs::canonical(shaders.first, editorWindow->editableFilter->resourcesRoot).string().c_str(),
				fs::canonical(shaders.second, editorWindow->editableFilter->resourcesRoot).string().c_str());
	}

	if (shaderIndex == objectIndex)
	{
		ui.shader_list->blockSignals(true);
		ui.shader_list->setCurrentIndex(shaderIndex);
		ui.shader_list->blockSignals(false);
	}
}

void Model3DTab::setOBJ(std::vector<std::string> &fileNames, bool quickLoading)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto model = (OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get();

	model->modelList.clear();

	for (auto file : fileNames)
	{
		model->modelList.push_back(fs::relative(file, fs::current_path() / editorWindow->editableFilter->resourcesRoot).string());
	}

	ui.OBJfile_name->setText(QString::fromStdString(fileNames.empty() ? "No files selected" :
		std::to_string(fileNames.size()) + " files selected"));

	ui.OBJfile_delete->setVisible(!fileNames.empty());

	editorWindow->data.modules[curModelIndex].models[0].renderParams.clear();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		model->objectRenderParams[i].clear();
	}

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	boost::property_tree::ptree modelTextures;

	std::vector<std::string> objectsNames;

	if (quickLoading)
	{
		ModelLoader::loadObjectsNames(fileNames, objectsNames);

		for (auto &name : objectsNames)
		{
			DrawObjectsModel obj;
			obj.name = name;
			model->objects.push_back(obj); // fake objects to store name, will be used and deleted in setMTL
		}

		ModelLoader::loadMTLnames(fileNames, model->objects);
	}
	else
	{
		model->reloadOBJ();
		for (auto &obj : model->objects)
		{
			objectsNames.push_back(obj.name);
		}
	}

	while (QLayoutItem* item = ui.objectsListContent_verticalLayout->itemAt(0))
	{
		ui.objectsListContent_verticalLayout->removeItem(item);
	}

	int number = 1;
	for (auto &name : objectsNames)
	{
		auto shaders = OpenGLModel::DefaultShadersMap[0].second;
		shaders.first = "./assets/shaders/vertex/" + shaders.first;
		shaders.second = "./assets/shaders/fragment/" + shaders.second;

		ExtraRenderParamsData renderParamsData(&editorWindow->data);
		renderParamsData.shaderIndex = ui.shader_list->currentIndex();

		auto renderParams = model->createDefaultObjectRenderParams();
		renderParams->shadersSources = shaders;
		renderParams->shader = shaderManagerWrapper.LoadFromFile(shaders.first.c_str(), shaders.second.c_str());

		editorWindow->data.modules[curModelIndex].models[0].renderParams.push_back(renderParamsData);

		for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		{
			model->objectRenderParams[i].push_back(renderParams);
		}

		std::stringstream objName;
		objName << "#" << std::to_string(number++) << (name.empty() ? "" : " " + name);

		auto objectButton = new QPushButton();
		objectButton->setCheckable(true);
		objectButton->setAutoExclusive(true);
		objectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		objectButton->setText(QString::fromStdString(objName.str()));
		ui.objectsListContent_verticalLayout->addWidget(objectButton);

		connect(objectButton, SIGNAL(clicked()), this, SLOT(on_objectButton_clicked()));

		boost::property_tree::ptree textureRecord;
		textureRecord.put("", "");
		modelTextures.push_back(std::make_pair("", textureRecord));
	}

	modelSuit.put_child("modelTextures", modelTextures);

	if (objectsNames.size() > 0)
	{
		objIndex = 0;
		((QPushButton*)ui.objectsListContent_verticalLayout->itemAt(0)->widget())->setChecked(true);
	}

	if (!quickLoading)
	{
		std::vector<std::string> MTLs;

		for (auto &file : model->modelList)
		{
			auto path = (editorWindow->editableFilter->resourcesRoot / file).replace_extension(".mtl");
			MTLs.push_back(fs::exists(path) ? path.string() : "");
		}

		setMTL(MTLs);
	}
}

void Model3DTab::setMTL(std::vector<std::string> &fileNames, bool quickLoading)
{
	boost::property_tree::ptree MTLs;

	try
	{
		for (auto &file : fileNames)
		{
			auto addMTLs = ModelLoader::convertMTLtoPTree(file.empty() ? "./assets/materials/no_material/no_material.mtl" : file,
				file.empty() ? fs::relative("./assets/materials/no_material/", fs::current_path() / editorWindow->editableFilter->resourcesRoot).string() :
				fs::relative(fs::path(file).parent_path(), fs::current_path() / editorWindow->editableFilter->resourcesRoot).string());

			for (auto MTL : addMTLs)
			{
				if (!MTLs.get_child_optional(MTL.first))
				{
					MTLs.push_back(MTL);
				}
			}
		}
	}
	catch (const std::exception&)
	{
		QMessageBox::warning(nullptr, "File opening error", "Can not open selected .MTL file", QMessageBox::Ok);
		return;
	}

	bool noMaterial;

	if (fileNames.empty())
	{
		ui.MTLfile_name->setText("No files selected");
		noMaterial = true;
	}
	else
	{
		ui.MTLfile_name->setText(QString::fromStdString(std::to_string(fileNames.size()) + " files selected"));
		noMaterial = false;
	}

	auto noMTL = ModelLoader::convertMTLtoPTree("./assets/materials/no_material/no_material.mtl",
		fs::relative("./assets/materials/no_material/", fs::current_path() / editorWindow->editableFilter->resourcesRoot).string())
		.front().second;

	ui.MTLfile_delete->setVisible(!noMaterial);

	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	OpenGL3DModel* model = (OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get();

	model->materialList.clear();

	for (auto &file : fileNames)
	{
		model->materialList.push_back(noMaterial ? "" : fs::relative(file, fs::current_path() / editorWindow->editableFilter->resourcesRoot).string());
	}

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto iter = modelSuit.get_child("modelTextures").begin();

	int meshIndex = 0;
	for (DrawObjectsModel &obj : editorWindow->editableFilter->models[curModelIndex]->objects)
	{		
		auto ptr = MTLs.get_child_optional(obj.material);

		boost::property_tree::ptree& MTL = ptr ? ptr.get() : noMTL;

		std::string texture = MTL.get<std::string>("Texture", "");

		boost::property_tree::ptree newValue;
		newValue.put("", texture);

		iter->second = newValue;

		for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		{
			model->objectRenderParams[i][meshIndex]->reset();
			model->setDefaultObjectRenderParams(*model->objectRenderParams[i][meshIndex]);
			model->objectRenderParams[i][meshIndex]->applyMTL(MTL);
			model->texturesPaths[i][meshIndex] = texture;
			editorWindow->editableFilter->models[curModelIndex]->applySuit
				(modelSuit, i, editorWindow->editableFilter->commonRenderParams, true);

			if (!quickLoading)
			{
				if (texture.empty())
				{
					setShader(meshIndex, "no texture");
				}
				else
				{
					setShader(meshIndex, "regular");
				}
			}
		}

		++meshIndex;
		++iter;
	}

	if (quickLoading)
	{
		model->objects.clear();
	}
}

void Model3DTab::on_OBJfile_choose_clicked()
{
	auto fileNames = QFileDialog::getOpenFileNames(editorWindow, tr("Choose an OBJ file"), editorWindow->fileDialogStart, "*.obj");

	if (fileNames.isEmpty())
	{
		return;
	}

	editorWindow->fileDialogStart = QFileInfo(fileNames.last()).dir().path();

	std::vector<std::string> OBJs;

	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();
	auto model = (OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get();
	for (auto file : model->modelList)
	{
		OBJs.push_back(file);
	}

	for (auto file : fileNames)
	{
		OBJs.push_back(file.toStdString());
	}

	std::sort(OBJs.begin(), OBJs.end(), [](std::string &A, std::string &B) -> bool
	{
		int shortCompare = fs::path(A).filename().string().compare(fs::path(B).filename().string());
		return shortCompare == 0 ? A < B : shortCompare == -1;
	});

	setOBJ(OBJs);
}

void Model3DTab::on_OBJfile_delete_clicked()
{
	setOBJ(std::vector<std::string>());
}

void Model3DTab::on_MTLfile_choose_clicked()
{
	auto fileNames = QFileDialog::getOpenFileNames(editorWindow, tr("Choose an MTL file"), editorWindow->fileDialogStart, "*.mtl");

	if (fileNames.isEmpty())
	{
		return;
	}

	editorWindow->fileDialogStart = QFileInfo(fileNames.last()).dir().path();

	std::vector<std::string> MTLs;

	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();
	auto model = (OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get();
	for (auto file : model->materialList)
	{
		MTLs.push_back(file);
	}

	for (auto file : fileNames)
	{
		MTLs.push_back(file.toStdString());
	}

	std::sort(MTLs.begin(), MTLs.end(), [](std::string &A, std::string &B) -> bool
	{
		int shortCompare = fs::path(A).filename().string().compare(fs::path(B).filename().string());
		return shortCompare == 0 ? A < B : shortCompare == -1;
	});

	setMTL(MTLs);
}

void Model3DTab::on_MTLfile_delete_clicked()
{
	setMTL(std::vector<std::string>());
}

// ================= SLIDERS =================

void Model3DTab::on_shiftX_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[0] = value / 100.;

	ui.shiftX_spinBox->setValue(value / 100.);
}

void Model3DTab::on_shiftY_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[1] = value / 100.;

	ui.shiftY_spinBox->setValue(value / 100.);
}

void Model3DTab::on_shiftZ_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[2] = value / 100.;

	ui.shiftZ_spinBox->setValue(value / 100.);
}

void Model3DTab::on_rotateX_slider_valueChanged(int value)
{
	
}

void Model3DTab::on_rotateY_slider_valueChanged(int value)
{
	
}

void Model3DTab::on_rotateZ_slider_valueChanged(int value)
{
	
}

void Model3DTab::on_scaleX_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelScale = value / 100.;

	ui.scaleX_spinBox->setValue(value / 100.);
}

void Model3DTab::on_scaleY_slider_valueChanged(int value)
{
}

void Model3DTab::on_scaleZ_slider_valueChanged(int value)
{
}

// ================= SPINBOXES =================

void Model3DTab::on_shiftX_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[0] = value;

	ui.shiftX_slider->setValue(int(value * 100));
}

void Model3DTab::on_shiftY_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[1] = value;

	ui.shiftY_slider->setValue(int(value * 100));
}

void Model3DTab::on_shiftZ_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelShift[2] = value;

	ui.shiftZ_slider->setValue((int)(value * 100));
}

void Model3DTab::on_rotateX_spinBox_valueChanged(double value)
{
	
}

void Model3DTab::on_rotateY_spinBox_valueChanged(double value)
{
	
}

void Model3DTab::on_rotateZ_spinBox_valueChanged(double value)
{
	
}

void Model3DTab::on_scaleX_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	((OpenGL3DModel*)editorWindow->editableFilter->models[curModelIndex].get())->modelScale = value;

	ui.scaleX_slider->setValue((int)(value * 100));
}

void Model3DTab::on_scaleY_spinBox_valueChanged(double value)
{
}

void Model3DTab::on_scaleZ_spinBox_valueChanged(double value)
{
}