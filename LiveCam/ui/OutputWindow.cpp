#include "OutputWindow.h"

bool OutputWindow::event(QEvent * e) // overloading event(QEvent*) method of QMainWindow 
{
switch (e->type())
{
case QEvent::WindowActivate:
	if (!liveCamWindow->ui.btn_startEffectLeft_Container->isEnabled())
	{
		liveCamWindow->editorWindow->raise();
	}
	break;
};
return QMainWindow::event(e);
}

OutputWindow::OutputWindow(LiveCamWindow* liveCamWindow, QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->liveCamWindow = liveCamWindow;

	ui.mainToolBar->hide();
	ui.menuBar->hide();
	ui.statusBar->hide();

	this->layout()->setContentsMargins(0, 0, 0, 0);
	this->layout()->setSpacing(0);
	centralWidget()->layout()->setContentsMargins(0, 0, 0, 0); 
	centralWidget()->layout()->setSpacing(0);
}

OutputWindow::~OutputWindow()
{
}

void OutputWindow::setWidget(CVImageWidgetOrg* widget){
	imageWidget = widget;
	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(imageWidget, 0, Qt::AlignCenter);
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->setSpacing(0);
	ui.outputFrame->setLayout(vbox);
}

void OutputWindow::on_actionFull_Screen_triggered(){
	this->showFullScreen();
	ui.mainToolBar->hide(); 
	ui.menuBar->hide();
	ui.statusBar->hide();
}

void OutputWindow::keyPressEvent(QKeyEvent *event) // definition
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		this->showMaximized();
		ui.mainToolBar->hide();
		ui.menuBar->hide();
		ui.statusBar->hide();
		break; 
	case Qt::Key_F:
		on_actionFull_Screen_triggered();
		break;
	default:
		QMainWindow::keyPressEvent(event);
	}
}