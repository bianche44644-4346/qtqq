#pragma once

#include <QObject>
#include "TalkWindowShell.h"

class WindowManager : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();

public:
	// ���ݴ���Ĵ��������ҵ������ض�Ӧ�Ĵ���
	QWidget* findWindowName(const QString& qsWindowName);
	// ���ݴ��ڵ�����ɾ������Ӧ�Ĵ���
	void deleteWindowName(const QString& qsWindowName);
	// ��Ӵ���������ӳ�䵱��(ӳ�䵱�д�ŵ��Ƕ�Ӧ�Ĵ������ʹ��ڵ�ƥ�䣬�����洢���Ǵ�
	// �˼�������)
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);

	// ��������
	static WindowManager* getInstance();

	//                    ����id              ����ʲô����                  ��Ӧ����ϵ����˭
	// ������ݿ�֮�󣬴˺����Ĳ��������޸�
	void addNewTalkWindow(const QString& uid/*, GroupType groupType = COMPANY, const QString& strPeople = ""*/);

	// ��ȡtalkWindowShell
	TalkWindowShell* getTalkWindowShell()const;

	QString getCreatingTalkId();   // ��ȡ���ڴ��������촰�ڵ�id
private:
	// �˴���Ҫ��TalkWindowShell����һ����������
	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*> m_windowMap;
	QString m_strCreatingTalkId = "";     // ���ڴ��������촰�ڵ�id -- ����ʾ��Ϣ��ʱ��ʹ�ã���addNewTalkWindow��ʼ��
};
