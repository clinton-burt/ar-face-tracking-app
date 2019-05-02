#pragma once

#include <QWidget>
#include "ui_RenderParamsTab.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RenderParamsTab; }
QT_END_NAMESPACE

class RenderParamsTab : public QWidget
{
	Q_OBJECT

public:
	RenderParamsTab(QWidget *parent = Q_NULLPTR);
	~RenderParamsTab();

private:
	Ui::RenderParamsTab ui;

public slots:
	
	void on_texture_button_clicked();
	void on_texture_delete_clicked();

	void on_visible_checkBox_toggled(bool checked);
	void on_cullFace_checkBox_toggled(bool checked);
	void on_blend_checkBox_toggled(bool checked);
	void on_infDist_checkBox_toggled(bool checked);

	void on_lightX_valueChanged(int value);
	void on_lightY_valueChanged(int value);
	void on_lightZ_valueChanged(int value);

	void on_matR_valueChanged(int value);
	void on_matG_valueChanged(int value);
	void on_matB_valueChanged(int value);

	void on_ambR_valueChanged(int value);
	void on_ambG_valueChanged(int value);
	void on_ambB_valueChanged(int value);

	void on_difR_valueChanged(int value);
	void on_difG_valueChanged(int value);
	void on_difB_valueChanged(int value);

	void on_specR_valueChanged(int value);
	void on_specG_valueChanged(int value);
	void on_specB_valueChanged(int value);

	void on_specPower_valueChanged(double value);
	void on_reflRatio_valueChanged(int value);
};