#pragma once

#include <QWidget>
#include <QMap>
#include <QTcpSocket>
#include <QUdpSocket>
#include "ui_TalkWindowShell.h"
#include "BasicWindow.h"
#include "EmotionWindow.h"

/*聊天窗口*/
class TalkWindowItem;
class QListWidgetItem;
class TalkWindow;

enum GroupType {
	COMPANY = 0,     // 公司群
	PERSONELGROUP,   // 人事部
	DEVELOPMENTGROUP,// 研发部
	MARKETGROUP,     // 市场部
	PTOP,            // 一个人和一个人进行聊天
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();

private:
	void initControl();   // 初始化部件
	void initTcpSocket();  // 初始化tcp套接字
	void initUdpSocket();  // 初始化udp套接字
	void getEmployeesID(QStringList& employeeList);   // 获取所有员工qq号，方便创建js的接口使用

	 // 创建js文件 -- 返回是否常见成功  参数 --  是一个链表，因为我们在js文件中需要根据数据库中存在的qq使用者，来确定发送者，external结合它们的qq号，所以我们需要传入一个链表存放员工的相应信息
	bool createJSFile(QStringList &employeeList);  
	// 参数:              发送者id      信息类型     数据
	void handleReceivedMsg(int senderID,int msgType, QString strMsg);  // 将接收到的信息显示到网页中

public:
	// 添加新的聊天窗口
	//  自定义类      聊天窗口                 聊天窗口的哪一项                因为不同的聊天窗口显示是不一样的(群聊天和单独聊天)
	//  使用数据库之后参数发生改变
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/);
	// 设置当前聊天窗口
	void setCurrentWidget(QWidget* widget);     // 设置当前的部件

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

public slots:
	// 处理表情按钮被点击了
	void onEmotionBtnClicked(bool);
	// 更新发送tcp信息，当我们点击发送按钮的时候，发送信号相应这个函数，并且使用参数传递一些
	// 发送的信息等数据，然后再槽函数中进行数据发送(客户端发送)
	//                        发送的数据        数据类型      文件
	void updateSendTcpMessage(QString& strData, int &msgType, QString fileName = "");

private slots:
	// 左侧列表点击后执行的槽函数
	void onTalkWindowItemClicked(QListWidgetItem* item);
	// 点击的表情(选中的表情)是哪一个，传入一个序号
	void onEmotionItemClicked(int emotionNum);
	// 在绑定udp成功之后，发送一个准备读取的信号，然后调用这个函数对数据进行处理
	void processPendingData();  // 处理udp广播收到的数据
private:
	// 当我们点击左侧的列表，点击哪个窗口进行聊天的时候，需要知道对应窗口的地址,使用变量进行你保存
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;   // 打开的聊天窗口
	EmotionWindow* m_emotionWindow;                         // 表情窗口

private:
	QTcpSocket *m_tcpClientSocket;    // 客户端tcp
	QUdpSocket *m_udpReceive;         // udp接收端

private:
	Ui::TalkWindowClass ui;
};
