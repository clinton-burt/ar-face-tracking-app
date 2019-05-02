#pragma once

#include <QObject>
#include <opencv2\opencv.hpp>


class CameraWorker : public QObject
{
	Q_OBJECT
public:
	CameraWorker(int id);
	~CameraWorker();

	public slots:
	void readVideo();

signals:
	// frame and index of label which frame will be displayed
	void frameFinished(cv::Mat3b frame, int index);

	void finished(int index);

private:
	QString filepath;
	int index;
};
