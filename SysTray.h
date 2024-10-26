#pragma once
/*ϵͳ���̣������ڴ�����Ҳ��С����*/
#include <QSystemTrayIcon>
#include <QWidget>

class SysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SysTray(QWidget *parent);
	~SysTray();

public slots:
	// ͼ�걻�����ˣ�ִ�еĲ���  --  ����꼤���ԭ��
	void onIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void initSystemTray();    // ��ʼ��ϵͳ����
	void addSysTrayMenu();    // ������̵Ĳ˵�


private:
	QWidget* m_parent;        // ����������Ϊ����
};
