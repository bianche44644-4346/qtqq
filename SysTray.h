#pragma once
/*系统托盘，就是在窗体的右侧的小框中*/
#include <QSystemTrayIcon>
#include <QWidget>

class SysTray : public QSystemTrayIcon
{
	Q_OBJECT

public:
	SysTray(QWidget *parent);
	~SysTray();

public slots:
	// 图标被触发了，执行的操作  --  被鼠标激活的原因
	void onIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void initSystemTray();    // 初始化系统托盘
	void addSysTrayMenu();    // 添加托盘的菜单


private:
	QWidget* m_parent;        // 定义属性作为父类
};
