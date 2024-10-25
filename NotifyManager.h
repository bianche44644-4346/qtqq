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

	// 对应窗口改变颜色之后，应该提醒其它的窗口也改变颜色
	void notifyOtherWindowChangeSkin(const QColor& color);

private:
	// 数据就是操作的通知实例
	static NotifyManager* instance;
};
