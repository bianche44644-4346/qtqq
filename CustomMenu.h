#pragma once

#include <QMenu>
#include <QMap>
#include <QAction>
/*�Զ���˵�*/
class CustomMenu : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = nullptr);
	~CustomMenu();

public:
	// ����Զ���˵� ����:  ������ť�ı�    �˵���ͼ��          �˵��ı�
	void addCustomMenu(const QString& text, const QString& icon, const QString& Name);
	QAction* getAction(const QString& text);    // ��ȡ������ť


private:
	QMap<QString, QAction*> m_menuActionMap;   // �˵�����ӳ�䣬��һ��ֵΪ����ӳ�������
											  // �ڶ�������Ϊ����Ķ���ӳ��
};
