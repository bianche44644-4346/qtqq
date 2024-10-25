#include "MsgWebView.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QUrl>
#include <QDomDocument>
#include <QDomNode>
#include <QSqlQueryModel>

extern QString gstrLoginHeadPath;  // 引用CCMainWindow中的记录登录者头像路径的全局变量，在显示网页对话的时候要使用

// 这个类在聊天窗口构造的时候其才会构造
MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent),m_channel(new QWebChannel(this))
{  // 这里代码在后续中进行了改动，原本进行了注释
	MsgWebPage* page = new MsgWebPage(this);  // 构造一个网页
	setPage(page);  // 将和构造的网页设置为当前的网页

	// QWebChannel* channel = new QWebChannel(this);  // 构造一个通道  -- 这里删除的原因是因为在类中我们定义了这样的属性
	m_msgHtmlObj = new MsgHtmlObj(this);

	// 注册一个对象
	//channel->registerObject("external0", m_msgHtmlObj);
	m_channel->registerObject("external0", m_msgHtmlObj);
	// 设置当前网页的通道
	// this->page()->setWebChannel(channel);  // 通道就是我们当前的channel

	/*连接信号与槽，当网页发送信息的时候，发送信号我们调用相应的tcp函数进行发送*/
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMessage);

	/*这里要进行处理，就是我们哪个群发送过来的信息，只有群中的人可以收到*/
	// 当前正在构建的聊天窗口的id
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();
	
	QSqlQueryModel queryEmployModel;
	QString strEmployeeID, strPicturePath;  // 保存员工ID和头像路径
	QString strExternal;
	bool isGroupTalk = false;   // 记录是否是群聊

	// 获取公司群的id号
	// 如果我们字符串中存在中文，即使是写死的，我们也需要拼接，因为需要进行编码转换(使用from哪个函数，或者和stringLiteral宏)，不然会出现乱码
	queryEmployModel.setQuery(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QStringLiteral("公司群")));
	QModelIndex companyIndex = queryEmployModel.index(0, 0);  // 使用索引获取模型中的数据
	QString strCompanyID = queryEmployModel.data(companyIndex).toString();

	if (strTalkId == strCompanyID) {  // 公司群聊 -- 将员工表中所有的状态为1的员工的qq号都拿出来，获取一下员工qq号以及头像
		queryEmployModel.setQuery("SELECT employeeID,picture FROM table_employees WHERE status = 1");
		isGroupTalk = true;
	}
	else {  // 如果不是公司群聊 -- 再判断一下是普通群聊还是单独聊天
		// 如何区分普通群聊和单独聊天?  -- 可以通过判断对应id的宽度来区分(群聊是4位，单聊是5位)
		if (strTalkId.length() == 4) {  // 其它群聊
			// 这里的查询语句在公司群的基础上及逆行了添加条件，因为我们获取的用户id都必须是对应群的员工
			queryEmployModel.setQuery(QString("SELECT employeeID,picture FROM table_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId));
			isGroupTalk = true;
		}
		else {  // 单独聊天
			// 设置sql语句 -- 这里已经直到对应聊天的id了(因为参数传递进来了)，所以只需要根据聊天人的id查询对应的头像就行
			queryEmployModel.setQuery(QString("SELECT picture FROM table_employees WHERE status = 1 AND employeeId = %1").arg(strTalkId));

			// 根据模型索引获取查询到的数据
			QModelIndex index = queryEmployModel.index(0, 0);
			strPicturePath = queryEmployModel.data(index).toString();

			strExternal = "external_" + strTalkId;
			// 构建网页对象
			MsgHtmlObj *msjHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msjHtmlObj);   // 在通道中注册
		}
	}

	// 如果是单聊就不需要了，因为上面已经写好了，群聊比较多，所以写在下面
	if (isGroupTalk) {  // 如果是群聊，那么就进行操作，这个bool中我们在判断是群聊的时候进行了赋值
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployModel.rowCount();
		/*
		  查询出来的模型总行数就是对应群所有的员工，我们对这些员工遍历进行处理
		*/
		for (int i = 0; i < rows; i++) {
			employeeModelIndex = queryEmployModel.index(i, 0);  // 员工号的索引就是第i行第0列
			pictureModelIndex = queryEmployModel.index(i, 1);

			strEmployeeID = queryEmployModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			// 构建网页对象
			MsgHtmlObj *msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}

	// 设置当前网页的通道
	this->page()->setWebChannel(m_channel);  // 通道就是我们当前的channel
	// 初始化一下收信息网页页面 加载网页信息
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));  // 使用对应的html文件进行初始化网页
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString & html, QString strObj)
{

	QJsonObject msgObj;  // 用于发送信息的
	// 追加信息
	QString qsMsg;
	// 解析html
	const QList<QStringList> msgList = parseHtml(html);

	int msgType = 1;      // 信息类型: 0是表情 1是文本 2是文件，默认写1
	QString strData;      // 如果发送的表情是55号，那么就不全三位为055
	int imagesNum = 0;    // 表情的数量 默认为0 -- 为了方便记录一次性发送多个表情的情况
	bool isImageMsg = false;  // 记录是否是图片数据，默认是false，信号发送传递的数据方便解包。

	// 遍历处理链表中解析到的html链表
	for (int i = 0; i < msgList.size(); i++) {
		if (msgList.at(i).at(0) == "img") {   // 解析出来的链表中对应元素的第一个元素是否是img
			// 保存图片的路径
			QString imagePath = msgList.at(i).at(1);  // 0是img，那么1就是srcl，这和你写的html顺序有关系
			// 路径可能是 "qrc:/MainWindow/.../77.png" 或者 ":/MainWindow/.../77.png"
			// 所以我们需要判断其路径是否有qrc
			QPixmap pixmap;  // 用来加载图片
			 
			// 获取表情的路径(位置)  qrc:/表示资源文件的路径
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();  // 位置就是，表情路径的长度
			isImageMsg = true;                // 这里我们发送的是图片信息，所以设置为true

			// 表情的名称，使用mid函数从路径中进行截取
			QString strEmotionName = imagePath.mid(pos); // 截取到的就是xxx.png
			strEmotionName.replace(".png", "");    // 我们需要的是表情的名称，也就是xxx表示的内容，所以我们将后面的.png使用空字符串进行替换
			
			// 根据表情名称的长度进行设置表情数据，不足3为则补足3位
			// 如:  23.png 则表情名称为23，则补足3位，为023
			int emotionNameLength = strEmotionName.length();   // 获取表情名字的长度，根据长度我们要判断需不需要补全到三位
			if (emotionNameLength == 1)   // 1位长度补2个0
				strData = strData + "00" + strEmotionName;
			else if(emotionNameLength == 2)
				strData = strData + "0" + strEmotionName;
			else   // 原来就是3位，所以不需要进行拼接
				strData = strData + strEmotionName;
		    
			msgType = 0;   // 信息类型为表情信息
			imagesNum++;   // 没进入到一次if，就解析到一个表情，那么表情的数量就进行++，为了方便发送多个表情的情况


			if (imagePath.left(3) == "qrc") {  // 判断最左边的三个字符是否是qrc，如果是进行相应的操作
				// 有qrc的话需要将qrc去掉
				// mid()函数，传入截取的开始位置，可以指定截取的长度，也可以不指定，就是将后面的都截取
				// 截取完之后进行加载
				pixmap.load(imagePath.mid(3));  // 因为qrc是前3个字符，所以我们应该从3的位置开始截取
			}
			else {
				// 没有qrc的直接加载，不用截取
				pixmap.load(imagePath);
			}

			
			// 图片加载完之后，设置一下图片的路径，加载到网页信息中，如何表示
			// %1 拼接路径 %2 拼接宽度 %3 拼接高度  ""内部写""需要加转义
			QString qimgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>").arg(imagePath)
				.arg(pixmap.width()).arg(pixmap.height());  // 表情图片html格式文本组合

			// 组合之后将其加到信息中去
			qsMsg += qimgPath;
		}
		else if (msgList.at(i).at(0) == "text") {  // 如果是文本
			qsMsg += msgList.at(i).at(1);      // 0就是text标签，1就是内容了
			strData = qsMsg;     // 直接将发送的数据信息赋值，此处不设置发送类型是因为类型的默认情况(进入函数就会设置)就是1，发送文本
		}
	}

	msgObj.insert("MSG", qsMsg);   // 插入的话是键和值
	// json对象转换为json文档
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);   //将Json转换为字符串存储起来
	// 判断对象是不是0，也就是是不是自己发送的消息
	if (strObj == "0") {  // 发信息
		// 这时候运行js脚本的时候就在后面加一个0，这样运行的就是自己发送信息的数据
		// 因为external0就是自己发送信息的通道
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		// 发送信号的时候，判断是否是发送的是图片，如果是就进行相应的处理
		if (isImageMsg) {
			// 到时候解包的时候就根据这种方式进行解包，因为发送数据的时候我们要发送信号给槽函数
			// 槽函数就是执行实现了一些tcp的操作方式。
			strData = QString::number(imagesNum) + "images" + strData;
		}

		// 发送信号告诉tcp的处理函数，我们发送的数据以及类型
		emit signalSendMsg(strData, msgType); 
	}
	else {  // 接受信息，只需要追加，不需要发射信号
		// 我们这里第一个拼接的时候直接可以使用传进来的对象参数，因为我们这时候已经
		// 进行了相应的判断，进入这个分支之后，已经是strObj传入的不是0,也就是说不是
		// 自己发送信息，而是接受哪个用户发送来的数据
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}

//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));   // 跑一下js     

	// 发送信号  -- 指定刚才设置的发送的数据以及发送的数据的类型(暂时不处理文件)
	// 发送信号告诉tcp的处理函数，我们发送的数据以及类型
    //	emit signalSendMsg(strData, msgType);
}

// 解析Html
QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	// 传进来的是一个Html的字符串，首先需要对DOM文档获取对象
	QDomDocument doc;
	doc.setContent(html);    // 设置节点文档的内容，直接调用函数将传入的html字符串传入进去就可以设置了
	// 想要解析节点里面的body节点
	const QDomElement& root = doc.documentElement();    // 节点元素
	// 获取body的节点
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);   // 解析节点后返回
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	// 解析节点 -- 解析出来的是一个字符串的链表
	QList<QStringList> attribute;
	// 获取传进来节点的子节点
	const QDomNodeList& list = node.childNodes();   // 返回所有子节点

	// 遍历获取到的节点
	for (int i = 0; i < list.size(); i++) {
		const QDomNode& node = list.at(i);

		// 对当前遍历到的节点进行解析
		if (node.isElement()) {   // 如果得到的是元素
			// 那么就转换元素
			const QDomElement& element = node.toElement();   // 直接转换为元素
			// 判断元素的标签名称，是不是图片或者文本之类的
			if (element.tagName() == "img") {  // 如果是图片
				// 获取图片的值，表示表情是哪一个
				QStringList attributeList;
				attributeList << "img" << element.attribute("src");  // 获取img标签的src属性的值，也就是图片的路径
				attribute << attributeList;    // 将解析的结果，写入attribute
			}

			if (element.tagName() == "span") { // 如果是文本
				QStringList attributeList;
				attributeList << "text" << element.text();   // 获取文本标签中的文本
				attribute << attributeList;
			}

			if (node.hasChildNodes()) {  // 如果其还有子节点，那么就继续解析
				attribute << parseDocNode(node);
			}
		}
		return attribute;    // 返回属性
	}

	return QList<QStringList>();
}

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath) :QObject(parent),
    m_msgLPicPath(msgLPicPath)    // 给左侧传数据的头像路径
{
	initHtmlTmpl();   // 构造中调用初始化网页
}

void MsgHtmlObj::initHtmlTmpl()
{
	// 初始化网页 -- 读取html文件的信息，对左右初始给对象的成员变量
	m_msgLHtmlTmpl = getMsgTmpHtml("msgleftTmpl");  // 传入文件中准备好的html文件名
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);    // 将左边的信息的头像路径替换为我们保存的左边头像的路径

	m_msgRHtmlTmpl = getMsgTmpHtml("msgrightTmpl");
	/* 右边的头像就是登录者的头像，我们直接将%1占位符替换成我们的登陆者头像路径 */
	// 通过CCMainWindow中的全局变量中进行赋值，保存登录者头像，从那里来直接获取登陆者头像
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgTmpHtml(const QString& code)
{
	// 获取html的信息，就是读取 -- 直接将传入文件中的信息全部读取出来，然后返回
	// 使用:/导入资源路径的时候，此资源必须添加到qrc资源管理文件中，否则就不要这样使用
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QFile::ReadOnly);    // 只读打开
	QString strData;
	if (file.isOpen()) {           // 判断是否是打开的
		strData = QLatin1String(file.readAll());   // 价格文件中的数据全部读取过来，构造成字符串
	}
	else {
		// 因为对象是QObject派生过来的，不是从什么窗体部件派生过来的，所以我们不能使用当前窗体this，作为其父窗体，应该是不可视化吧
		QMessageBox::information(nullptr, "Tips", "Failed to init html");   // 打印异常信息
		return QString("");    // 返回空
	}
	file.close();

	return strData;
}

// 对网页的请求进行限制，仅仅允许接收我们qrc:html的格式或者网址，它其实是一个路径
// QUrl不仅仅可以保存http等常用的网址，还可以存放路径(资源路径等)
bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	if (url.scheme() == QString("qrc"))  // 判断url类型是否是qrc
	{
		// 是qrc，就接收到请求，并打开到对应的网页
		return true;   // 所来的请求就是qrc的请求
	}
	return false;
}
