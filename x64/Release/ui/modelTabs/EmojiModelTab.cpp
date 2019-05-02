#include "EmojiModelTab.h"

EmojiModelTab::EmojiModelTab(EditorWindow *editorWindow, QWidget *parent)
	: QWidget(parent)
{
	this->editorWindow = editorWindow;

	ui.setupUi(this);

	ui.emojiImage_delete->setVisible(false);

	connect(ui.delete_button, SIGNAL(clicked()), editorWindow, SLOT(on_tab_deleted()));
	connect(ui.emojiImage_button, SIGNAL(clicked()), editorWindow->liveCamWindow, SLOT(on_editor_moduleIcon_changed()));
	connect(ui.emojiImage_delete, SIGNAL(clicked()), editorWindow->liveCamWindow, SLOT(on_editor_moduleIcon_changed()));
}

EmojiModelTab::~EmojiModelTab()
{
}

void EmojiModelTab::readEmojiModelProperties(FxCore* sourceFilter, OpenGLEmojiModel* model, boost::property_tree::ptree& suit, fs::path& resourcesRoot)
{
	ui.modelName_edit->setText(QString::fromStdString(model->name));

	setEmojiImage(QString::fromStdString(fs::path(resourcesRoot / suit.get<std::string>("emojiTexture", "")).string()));

	ui.imageDirection_comboBox->setCurrentIndex(suit.get<int>("symmetricState", 0) + 1);

	ui.rotation_checkBox->setChecked(suit.get<bool>("emojiRotation", false));

	auto tree = suit.get_child_optional("emojiOffset");
	Eigen::Vector3f shift = tree ? JSONVectorReader::readVector3f(tree.get()) : Eigen::Vector3f(0, 0, 0);

	ui.shiftX_slider->setValue(shift[0] * 100);
	ui.shiftY_slider->setValue(shift[1] * 100);
	ui.shiftZ_slider->setValue(shift[2] * 100);

	tree = suit.get_child_optional("emojiScale");
	Eigen::Vector2f scale = tree ? JSONVectorReader::readVector2f(tree.get()) : Eigen::Vector2f(1, 1);

	ui.scaleX_slider->setValue(scale[0] * 100);
	ui.scaleY_slider->setValue(scale[1] * 100);

	int curModelIndex = editorWindow->editableFilter->models.size() - 1;

	auto ID = sourceFilter->filterModules[curModelIndex].suits[curModelIndex].get<int>("renderParamsID");

	auto &shaders = sourceFilter->commonRenderParams[ID]->shadersSources;

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->objectRenderParams[i][0]->shadersSources = shaders;
		editorWindow->editableFilter->models[curModelIndex]->objectRenderParams[i][0]->shader =
			shaderManagerWrapper.LoadFromFile((sourceFilter->resourcesRoot / shaders.first).string().data(),
			(sourceFilter->resourcesRoot / shaders.second).string().data());
	}
}

void EmojiModelTab::on_emojiImage_button_clicked()
{
	auto fileName = QFileDialog::getOpenFileName(editorWindow, tr("Choose an Emoji Image"), editorWindow->fileDialogStart);

	if (fileName.isEmpty())
	{
		return;
	}

	editorWindow->fileDialogStart = QFileInfo(fileName).dir().path();

	setEmojiImage(fileName);
	editorWindow->setElementIcon(fileName);
}

void EmojiModelTab::on_emojiImage_delete_clicked()
{
	QString fileName = "";
	setEmojiImage(fileName);
	editorWindow->setElementIcon(fileName);
}

void EmojiModelTab::on_modelName_edit_textEdited(const QString &text)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	editorWindow->editableFilter->models[curModelIndex]->name = text.toStdString();

	editorWindow->ui.models_tabWidget->setTabText(curModelIndex, text);
}

void EmojiModelTab::on_imageDirection_comboBox_currentIndexChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	modelSuit.put("symmetricState", value - 1);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_rotation_checkBox_toggled(bool checked)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	modelSuit.put("emojiRotation", checked ? "true" : "false");

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::setEmojiImage(QString &imagePath)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	QPixmap pixmap = QPixmap(imagePath);

	if (pixmap.isNull())
	{
		ui.emojiImage_button->setIcon(QIcon(":/images/FileButton.png"));
		modelSuit.erase("emojiTexture");
		ui.emojiImage_delete->setVisible(false);
	}
	else
	{
		ui.emojiImage_button->setIcon(QIcon(pixmap));
		modelSuit.put("emojiTexture", fs::relative(imagePath.toStdString(), fs::current_path() /
			editorWindow->editableFilter->resourcesRoot).string());
		ui.emojiImage_delete->setVisible(true);
	}

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, true);
	}
}

// ================= SLIDERS =================

void EmojiModelTab::on_shiftX_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(value / 100., curOffset[1], curOffset[2]);

	ui.shiftX_spinBox->setValue(value / 100.);

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_shiftY_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(curOffset[0], value / 100.f, curOffset[2]);

	ui.shiftY_spinBox->setValue(value / 100.);

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_shiftZ_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(curOffset[0], curOffset[1], value / 100.f);

	ui.shiftZ_spinBox->setValue(value / 100.);

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_scaleX_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curScale = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisScale[0];

	Vector2f vector2f = Vector2f(value / 100.f, curScale[1]);

	ui.scaleX_spinBox->setValue(value / 100.);

	boost::property_tree::ptree vector2fTree;

	for (int i = 0; i < 2; ++i)
	{
		boost::property_tree::ptree vector2fTreeChild;
		vector2fTreeChild.put("", vector2f[i]);
		vector2fTree.push_back(make_pair("", vector2fTreeChild));
	}

	modelSuit.put_child("emojiScale", vector2fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_scaleY_slider_valueChanged(int value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curScale = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisScale[0];

	Vector2f vector2f = Vector2f(curScale[0], value / 100.f);

	ui.scaleY_spinBox->setValue(value / 100.);

	boost::property_tree::ptree vector2fTree;

	for (int i = 0; i < 2; ++i)
	{
		boost::property_tree::ptree vector2fTreeChild;
		vector2fTreeChild.put("", vector2f[i]);
		vector2fTree.push_back(make_pair("", vector2fTreeChild));
	}

	modelSuit.put_child("emojiScale", vector2fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

// ================= SPINBOXES =================

void EmojiModelTab::on_shiftX_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(value, curOffset[1], curOffset[2]);

	ui.shiftX_slider->setValue(int(value * 100));

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_shiftY_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(curOffset[0], value, curOffset[2]);

	ui.shiftY_slider->setValue(int(value * 100));

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_shiftZ_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curOffset = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisOffset[0];

	Vector3f vector3f = Vector3f(curOffset[0], curOffset[1], value);

	ui.shiftZ_slider->setValue(int(value * 100));

	boost::property_tree::ptree vector3fTree;

	for (int i = 0; i < 3; ++i)
	{
		boost::property_tree::ptree vector3fTreeChild;
		vector3fTreeChild.put("", vector3f[i]);
		vector3fTree.push_back(make_pair("", vector3fTreeChild));
	}

	modelSuit.put_child("emojiOffset", vector3fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_scaleX_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curScale = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisScale[0];

	Vector2f vector2f = Vector2f(value, curScale[1]);

	ui.scaleX_slider->setValue(int(value * 100));

	boost::property_tree::ptree vector2fTree;

	for (int i = 0; i < 2; ++i)
	{
		boost::property_tree::ptree vector2fTreeChild;
		vector2fTreeChild.put("", vector2f[i]);
		vector2fTree.push_back(make_pair("", vector2fTreeChild));
	}

	modelSuit.put_child("emojiScale", vector2fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}

void EmojiModelTab::on_scaleY_spinBox_valueChanged(double value)
{
	int curModelIndex = editorWindow->ui.models_tabWidget->currentIndex();

	auto &modelSuit = editorWindow->editableFilter->filterModules[curModelIndex].suits[curModelIndex];

	auto curScale = ((OpenGLEmojiModel*)editorWindow->editableFilter->models[curModelIndex].get())->emojisScale[0];

	Vector2f vector2f = Vector2f(curScale[0], value);

	ui.scaleY_slider->setValue(int(value * 100));

	boost::property_tree::ptree vector2fTree;

	for (int i = 0; i < 2; ++i)
	{
		boost::property_tree::ptree vector2fTreeChild;
		vector2fTreeChild.put("", vector2f[i]);
		vector2fTree.push_back(make_pair("", vector2fTreeChild));
	}

	modelSuit.put_child("emojiScale", vector2fTree);

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		editorWindow->editableFilter->models[curModelIndex]->applySuit(modelSuit, i, editorWindow->editableFilter->commonRenderParams, false);
	}
}