#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);   // ��������Ϊ͸���ı���
	CommonUtils::loadStyleSheet(this, "Menu");    // ����ǰ���������ʽ��
}

CustomMenu::~CustomMenu()
{}

void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{
	// ����action�ķ�������Ӷ���
	QAction* pAction = addAction(QIcon(icon), name);
	m_menuActionMap.insert(text, pAction);                // ��ӵ��˵�ӳ�䵱��
}

QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
