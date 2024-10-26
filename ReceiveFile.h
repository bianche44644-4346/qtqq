#pragma once

#include "BasicWindow.h"
#include "ui_ReceiveFile.h"

class ReceieveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceieveFile(QWidget *parent = nullptr);
	~ReceieveFile();

	void setMsg(QString &msgLabel);   // ���ñ�ǩ�е��ļ�������ʾ��Ϣ

private slots:
	// ������ť��Ӧ�Ĳۺ���
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();

Q_SIGNALS:
	// ��ȡ����ť����Ӧ�ۺ����н��з���
	void refuseFile();     // �ܾ��ļ������ǵ����Ǿܾ������ļ�֮��������źţ����ö�Ӧ�Ĳۺ���������Ӧ�Ĳ���

private:
	Ui::ReceiveFile ui;
};
