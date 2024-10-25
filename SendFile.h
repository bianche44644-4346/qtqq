#pragma once

#include "BasicWindow.h"
#include "ui_SendFile.h"

class SendFile : public BasicWindow  // 我们自己写的类，用作所有可视窗体的基类
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = nullptr);
	~SendFile();

Q_SIGNALS:
	// 此信号用于发射文件发送的信号
	void sendFileClicked(QString& strData,int &msgType,QString fileName);

private slots:
	void on_openBtn_clicked();    // 打开按钮被点击的槽函数
	/*
	   当我们点击发送按钮的时候，tcp需要按照约定的内容进行写入数据，所以这里要发射
	   一个信号。而且带参数。发送什么数据，什么类型，以及文件名称

	   谁来接收信号，就是talkWindowshell中更新tcp数据的槽函数接收--updateSendTcpMessage
	*/
	void on_sendBtn_clicked();    // 发送按钮被点击的槽函数

private:
	Ui::SendFile ui;
	QString m_filePath;      // 用户选择的文件路径
};
