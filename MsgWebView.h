#pragma once

#include <QWebEngineView>
#include <QDomNode>

class MsgHtmlObj : public QObject {
	Q_OBJECT

		// 两个动态属性，用来网页信息的显示，一个是自己发送的，一个是对方发送来的
		//                              属性类成员
		// 发来的信息  L表示聊天窗口的左边，别人发的
		Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
		// 我发的信息  R表示聊天窗口的右边，自己发的
		Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)

public:
	//                         发信息来的人的头像路径
	MsgHtmlObj(QObject* parent, QString msgLPicPath = ""); // 新增一个参数，接收左侧发信息来的人头像的地址，到时候构建网页对象的时候就能显示相应的头像信息


Q_SIGNALS:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();    // 初始化聊天网页
	// 初始化网页对于发信息和收信息不一样，因为我发的在左侧显示，别人发的在右侧显示
	QString getMsgTmpHtml(const QString& code);     // 传输传入要初始化的网页()
private:
	QString m_msgLHtmlTmpl;  // 别人发来的信息
	QString m_msgRHtmlTmpl;  // 自己发送的信息
	QString m_msgLPicPath;   // 左侧发送信息的头像路径
};

// 网页
class MsgWebPage :public QWebEnginePage {
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {}
	~MsgWebPage() {}

	// 重写网页的原因，因为接收的网页的请求，只有来自qrc的才行，其它的请求是不行的
	// 通过重写方法来实现限制
protected:
	// 这个类中只需要重写这一个方法，主要为了对请求进行限制
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)override;
};

// QWebEngineView视图用于系显示网页内容
class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();
    
	// 我们需要判断是否通道中添加什么数据，所以我们还需要传入一个数据
	// 通道的变量，我们只有两种方式external0或者external_id号等。
	// 所以如果我们自己发信息的话，那么就使用默认就是external0，不是的话就传递一个id号，之后进行一个拼接
	// 这样函数中就可以根据传入的参数判断到底是自己发送消息，还是别人发送消息了
	void appendMsg(const QString& html,QString strObj = "0");   // 默认为0，第二个参数要传递员工的qq号，如果不传的话默认就是0
	// void appendMsg(const QString& html);  // 追加信息，传入的参数字符串是html格式的用来显示网页

private:
	QList<QStringList> parseHtml(const QString& html);  // 解析Html，返回字符串链表，参数传入需要解析的html字符串
	// Qt中所有DOM节点(属性，说明，文本等)都可以使用QDomNode进行操作(表示)
	QList<QStringList> parseDocNode(const QDomNode& node);   // 解析节点，使用QDomNub可以解析Qt中所有的节点，包括说明，属性，元素，文本等

	// 网页显示需要一个对象，Qt与html通信需要使用QWebChannel，需要在通道中构造对象才能通信，我们进行构造

Q_SIGNALS:
	// 当网页发送信息的时候就发送信号，调用tcp的函数进行相信的数据发送操作
	void signalSendMsg(QString& strData, int &msgType, QString sFile = "");

private:
	MsgHtmlObj* m_msgHtmlObj;
	QWebChannel* m_channel;     // 网络通道
};
