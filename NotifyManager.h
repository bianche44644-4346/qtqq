#pragma once

#include <QObject>

class NotifyManager : public QObject
{
	Q_OBJECT

public:
	NotifyManager();
	~NotifyManager();

signals:
	void signalSkinChanged(const QColor& color);

public:
	static NotifyManager* getInstance();

	// ��Ӧ���ڸı���ɫ֮��Ӧ�����������Ĵ���Ҳ�ı���ɫ
	void notifyOtherWindowChangeSkin(const QColor& color);

private:
	// ���ݾ��ǲ�����֪ͨʵ��
	static NotifyManager* instance;
};
