#pragma once

#include <QObject>

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QDebug>
#include <opencv2/opencv.hpp>


class CVImageWidgetOrg : public QWidget
{
	Q_OBJECT

public:
	CVImageWidgetOrg(QObject *parent);
	~CVImageWidgetOrg();

    explicit CVImageWidgetOrg(QWidget *parent = 0);

    QSize sizeHint() const;
//    QSize minimumSizeHint() const;

	public slots :
        void showImage(const cv::Mat& image);

protected:
	void paintEvent(QPaintEvent* /*event*/) {
		// Display the image
		QPainter painter(this);
		painter.drawImage(QPoint(0, 0), _qimage);
		painter.end();
	}

	QImage _qimage;
	cv::Mat _tmp;
};

