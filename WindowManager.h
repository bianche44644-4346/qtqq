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
	// 根据传入的窗口名称找到并返回对应的窗口
	QWidget* findWindowName(const QString& qsWindowName);
	// 根据窗口的名称删除掉对应的窗口
	void deleteWindowName(const QString& qsWindowName);
	// 添加窗口名，到映射当中(映射当中存放的是对应的窗口名和窗口的匹配，用来存储我们打开
	// 了几个窗口)
	void addWindowName(const QString& qsWindowName, QWidget* qWidget);

	// 单例构造
	static WindowManager* getInstance();

	//                    窗口id              创建什么窗口                  对应的联系人是谁
	// 添加数据库之后，此函数的参数进行修改
	void addNewTalkWindow(const QString& uid/*, GroupType groupType = COMPANY, const QString& strPeople = ""*/);

	// 获取talkWindowShell
	TalkWindowShell* getTalkWindowShell()const;

	QString getCreatingTalkId();   // 获取正在创建的聊天窗口的id
private:
	// 此处需要和TalkWindowShell进行一个关联操作
	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*> m_windowMap;
	QString m_strCreatingTalkId = "";     // 正在创建的聊天窗口的id -- 在显示信息的时候使用，在addNewTalkWindow初始化
};
