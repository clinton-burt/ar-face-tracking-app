#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <iostream>
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <models\FilterUiModel.h>
#pragma once

using namespace std;

class JsonParser {
public:
	JsonParser();
	~JsonParser();

	static vector<FilterUiModel> readUiConfig(QString path)
	{
		vector<FilterUiModel> filterUiModels;

		QFile file(path);
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		QByteArray jsonData = file.readAll();
		file.close();

		QJsonDocument document = QJsonDocument::fromJson(jsonData);
		QJsonObject object = document.object();

		QJsonValue value = object.value("filters");
		QJsonArray arrayValue = value.toArray();
		foreach(const QJsonValue & v, arrayValue){
			//qDebug() << v.toObject().value("id").toInt();
			//qDebug() << v.toObject().value("title").toString();
			//qDebug() << v.toObject().value("icon").toString();
			FilterUiModel model = FilterUiModel();
			model.setId(v.toObject().value("id").toInt());
			model.setIcon(QString(v.toObject().value("icon").toString()).toStdString());
			model.setTitle(QString(v.toObject().value("title").toString()).toStdString());
			model.setJSONsource(QString(v.toObject().value("JSONsource").toString()).toStdString());
			filterUiModels.push_back(model);
		}

		return filterUiModels;
	}
};
#endif