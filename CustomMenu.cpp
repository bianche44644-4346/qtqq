#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);   // 设置属性为透明的背景
	CommonUtils::loadStyleSheet(this, "Menu");    // 给当前的类添加样式表
}

CustomMenu::~CustomMenu()
{}

void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{
	// 调用action的方法来添加动作
	QAction* pAction = addAction(QIcon(icon), name);
	m_menuActionMap.insert(text, pAction);                // 添加到菜单映射当中
}

QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
