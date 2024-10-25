#pragma once
/*用于修改皮肤颜色的皮肤窗口，可显示的*/
#include <QWidget>
#include "ui_SkinWindow.h"
#include "BasicWindow.h"

class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = nullptr);
	~SkinWindow();

public slots:  // 重写槽函数
	void onShowClose();

public:
	void initControl();   // 初始化控件

private:
	Ui::SkinWindow ui;
};
