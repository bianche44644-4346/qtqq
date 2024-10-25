#pragma once

#include "BasicWindow.h"
#include "ui_ReceiveFile.h"

class ReceieveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceieveFile(QWidget *parent = nullptr);
	~ReceieveFile();

	void setMsg(QString &msgLabel);   // 设置标签中的文件发送提示信息

private slots:
	// 两个按钮对应的槽函数
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();

Q_SIGNALS:
	// 在取消按钮的响应槽函数中进行发送
	void refuseFile();     // 拒绝文件，就是当我们拒绝接收文件之后发送这个信号，调用对应的槽函数进行相应的操作

private:
	Ui::ReceiveFile ui;
};
