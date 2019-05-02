#include "CVImageWidgetOrg.h"

#include "utils/Resolutions.h"

CVImageWidgetOrg::~CVImageWidgetOrg()
{
}

CVImageWidgetOrg::CVImageWidgetOrg(QWidget *parent) : QWidget(parent)
{
}

QSize CVImageWidgetOrg::sizeHint() const
{
    return _qimage.size();
}

//QSize CVImageWidgetOrg::minimumSizeHint() const
//{
//    return _qimage.size();
//}

void CVImageWidgetOrg::showImage(const cv::Mat &image)
{
    // Convert the image to the RGB888 format
    switch (image.type()) {
    case CV_8UC1:
        cvtColor(image, _tmp, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(image, _tmp, CV_BGR2RGB);
        break;
    }

    // QImage needs the data to be stored continuously in memory
    assert(_tmp.isContinuous());
    // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
    // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
    // has three bytes.

  //  QWidget *parent_widget = qobject_cast<QWidget *>(parent());

    QImage qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols * 3, QImage::Format_RGB888);

    //qDebug() << "CVImageWidgetOrg::showImage :::: parent" << parent_widget->width() << parent_widget->height();
  //  qDebug() << "CVImageWidgetOrg::showImage :::: image " << qimage.width() << qimage.height();
   // qDebug() << "CVImageWidgetOrg::showImage :::: image_" << _qimage.width() << _qimage.height() << "\n\n";


  /*  if (parent_widget && (qimage.width() > parent_widget->width() || qimage.height() > parent_widget->height())){
        _qimage = qimage.scaled(parent_widget->width(), parent_widget->height(), Qt::KeepAspectRatio);
    }
    else
    {*/
        _qimage = qimage;
    //}

	//this->setFixedSize(_qimage.width(), _qimage.height()); 
	this->setFixedSize(Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);

    repaint();
}
