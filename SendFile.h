#pragma once

#include "BasicWindow.h"
#include "ui_SendFile.h"

class SendFile : public BasicWindow  // �����Լ�д���࣬�������п��Ӵ���Ļ���
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = nullptr);
	~SendFile();

Q_SIGNALS:
	// ���ź����ڷ����ļ����͵��ź�
	void sendFileClicked(QString& strData,int &msgType,QString fileName);

private slots:
	void on_openBtn_clicked();    // �򿪰�ť������Ĳۺ���
	/*
	   �����ǵ�����Ͱ�ť��ʱ��tcp��Ҫ����Լ�������ݽ���д�����ݣ���������Ҫ����
	   һ���źš����Ҵ�����������ʲô���ݣ�ʲô���ͣ��Լ��ļ�����

	   ˭�������źţ�����talkWindowshell�и���tcp���ݵĲۺ�������--updateSendTcpMessage
	*/
	void on_sendBtn_clicked();    // ���Ͱ�ť������Ĳۺ���

private:
	Ui::SendFile ui;
	QString m_filePath;      // �û�ѡ����ļ�·��
};
