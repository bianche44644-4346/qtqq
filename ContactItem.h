#pragma once

#include <QWidget>
#include "ui_ContactItem.h"
/*��ϵ����*/
class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = nullptr);
	~ContactItem();

public:
	void setUserName(const QString& userName);   // �����û���
	void setSignName(const QString& signName);   // ����ǩ��
	void setPixmap(const QPixmap& headPath);     // ����ͷ��
	QString getUserName()const;
	QSize getHeadLabelSize()const;               // ��ȡͷ����ǩ�ĳߴ�

private:
	void initControl();                               // ��ʼ���ؼ�

private:
	Ui::ContactItem ui;
};
