#pragma once

#include <QWidget>
#include "ui_ContactItem.h"
/*联系人项*/
class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = nullptr);
	~ContactItem();

public:
	void setUserName(const QString& userName);   // 设置用户名
	void setSignName(const QString& signName);   // 设置签名
	void setPixmap(const QPixmap& headPath);     // 设置头像
	QString getUserName()const;
	QSize getHeadLabelSize()const;               // 获取头部标签的尺寸

private:
	void initControl();                               // 初始化控件

private:
	Ui::ContactItem ui;
};
