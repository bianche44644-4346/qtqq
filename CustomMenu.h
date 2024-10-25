#pragma once

#include <QMenu>
#include <QMap>
#include <QAction>
/*自定义菜单*/
class CustomMenu : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = nullptr);
	~CustomMenu();

public:
	// 添加自定义菜单 参数:  动作按钮文本    菜单的图标          菜单文本
	void addCustomMenu(const QString& text, const QString& icon, const QString& Name);
	QAction* getAction(const QString& text);    // 获取动作按钮


private:
	QMap<QString, QAction*> m_menuActionMap;   // 菜单动作映射，第一个值为动作映射的名字
											  // 第二个参数为具体的动作映射
};
