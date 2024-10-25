#pragma once

#include <QWidget> 
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"
#include <QTreeWidgetItem>


class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	// 连接数据库之后构造函数发生了改变
	TalkWindow(QWidget *parent, const QString& uid/*, GroupType groupType*/);
	~TalkWindow();

	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId()const;

private slots:
	void onFileOpenBtnClicked(bool);
	void onSendBtnClicked(bool);
	// 双击群成员的项，然后就进入到与对应双击的群成员进行聊天
	void onItemDoubleClicked(QTreeWidgetItem*, int);

private:
	void initControl();
	/* 添加数据库之后添加 */
	void initGroupTalkStatus();  // 初始化群聊状态，将m_isGroupTalk设置为true或者false
	/* 添加数据库之后添加 */
	int getCompDepId();         // 获取公司部门的qq号或者id号

	/* 添加数据库之后，我们不需要初始化的这么详细，只需要有一个初始化群聊的方法就行了
	   因为不同的用户在哪个群聊就初始化那个
	void initCompanyTalk();   // 初始化公司群聊天
	void initPersonelTalk();  // 初始化人事部聊天
	void initMarketTalk();    // 初始化市场部聊天
	void initDevelopTalk();   // 初始化研发部聊天
	*/
	void initTalkWindow();    // 初始化群聊，使用数据库之后添加
	void initPtoPTalk();      // 初始化单聊
	// void addPeopleInfo(QTreeWidgetItem* pRootGroupItem);   // 给传入的父项添加子项
	// 添加数据库进行修改
	void addPeopleInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);

private:
	Ui::TalkWindow ui;
	// 保存当前聊天窗口的ID
	QString m_talkId;
	// 保存当前聊天窗口分组的类型
	// GroupType m_groupType;  -- 连接数据库之后这里就不需要了
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;   // 所有分组联系人姓名
	bool m_isGroupTalk;   // 是否为群聊 -- 数据库连接之后添加
	friend class TalkWindowShell;   // 设置友元，因为我们需要在TalkWindowShell中访问TalkWindow的私有成员ui，所以设置成友元，在TalkWindowShell将接受的数据追加到当前窗口中时使用
};
