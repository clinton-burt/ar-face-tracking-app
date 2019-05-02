#include <workers/CameraWorker.h>
#include <QDebug>
#include <QThread>
#include <QTime>

Q_DECLARE_METATYPE(cv::Mat3b)

CameraWorker::CameraWorker(int id) : index(id)
{
	qRegisterMetaType<cv::Mat3b>("Mat3b");
}

CameraWorker::~CameraWorker()
{
}

void CameraWorker::readVideo()
{
	cv::VideoCapture camera;

	camera.open(index);

	if (!camera.isOpened())
	{
		qDebug() << "Can't open video file " << filepath;
		emit finished(index);
		return;
	}
	else{
		camera.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
		camera.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	}

	while (true)
	{
		cv::Mat3b frame;
		camera >> frame;
		cv::flip(frame, frame, 1);
		if (frame.empty())
		{
			frame = cv::Mat(cv::Size(640, 360), CV_8UC3, cv::Scalar(192, 192, 192));
			emit frameFinished(frame, index);
			break;
		}
		
		emit frameFinished(frame, index);
		QThread::msleep(30);
	}

	emit finished(index);
}