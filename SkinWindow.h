#pragma once
/*�����޸�Ƥ����ɫ��Ƥ�����ڣ�����ʾ��*/
#include <QWidget>
#include "ui_SkinWindow.h"
#include "BasicWindow.h"

class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = nullptr);
	~SkinWindow();

public slots:  // ��д�ۺ���
	void onShowClose();

public:
	void initControl();   // ��ʼ���ؼ�

private:
	Ui::SkinWindow ui;
};
