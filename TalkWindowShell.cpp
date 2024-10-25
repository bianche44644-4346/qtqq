#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include "WindowManager.h"
#include "ReceiveFile.h"
#include <QListWidget>
#include <QMap>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>

/*加g表示是一个全局的变量*/
// const int gtcpPort = 6666;  // 端口号  --  端口号一般不会改变，所以我们将其设置为const
const int gtcpPort = 8888;     // 这个端口号应该和服务器端设置的端口号是一样的，否则会连接失败
const int gudpPort = 6666;     // udp的端口号
extern QString gLoginEmployeeID;    // 登陆者的qq号

QString gFileName;  // 保存文件名称
QString gFileData;  // 保存文件数据

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();

	// 在构造函数调用初始化tcp和udp的函数
	initTcpSocket();
	initUdpSocket();

	
	// 我们不需要什么时候都更新，如果js文件为空了，我们再进行更新，所以我们需要判断
	// 当前的js文件是否为空
	QFile file("Resources/MainWindow/MsgHtml/msgtmpl.js");  // 添加到资源文件中可以使用:/来访问，如果不在那么就不要加:/
	
	if (!file.size()) {  // 文件长度为0，那么写入数据
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);  // 获取所有员工qq号的链表

		if (!createJSFile(employeesIDList)) {            // 判断是否更新js文件是否失败
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("更新js文件写入数据失败"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	/*   // 释放表情对象
	   delete m_emotionWindow;
	   m_emotionWindow = nullptr;
	 */
}

void TalkWindowShell::initControl()
{
	// 加载样式表
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("聊天窗口"));

	// 表情窗口当用户还没有点击表情按钮的时候，我们是不需要进行显示的，只有点击了才会系那是
	m_emotionWindow = new EmotionWindow;   // 构造表情窗口
	// 没有点击的时候隐藏表情窗口
	m_emotionWindow->hide();               // 隐藏表情窗口

	// 给左侧的部件设置一下尺寸(给分裂器设置尺寸，左边设置多少或者右边设置多少)
	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;    // 左侧宽度154，右侧宽度等于窗口宽度-154
	ui.splitter->setSizes(leftWidgetSize);     // 分裂器设置尺寸

	// 点击不显示黑框的样式，将当前窗体作为其父类
	ui.listWidget->setStyle(new CustomProxyStyle(this));

	// 连接信号与槽 -- 当左侧的列表部件，就更新相应的聊天窗口，包括选中表情，就更新表情窗口
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);   // 构造tcp
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceive = new QUdpSocket(this);

	// 客户端端口绑定，也测试多个端口，防止端口被占用 -- 客户端只需要绑定一个端口，不用和服务器端一样，需要对多个端口进行广播
	for (quint16 port = gudpPort; port < gudpPort + 200; port++) {
		// 绑定一个端口成功就行了 -- 采用共享的方式绑定端口
		if (m_udpReceive->bind(port, QUdpSocket::ShareAddress)) {
			break;
		}
	}
	/*绑定成功之后 -- 发送一个准备读取的信号，然后触发槽函数进行处理*/
	connect(m_udpReceive, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void  TalkWindowShell::getEmployeesID(QStringList& employeeList)
{
	QSqlQueryModel queryModel;
	// 查找所有有效数据的员工id
	queryModel.setQuery("SELECT employeeID FROM table_employees WHERE status = 1");

	// 返回模型的总行数(员工的总数)
	int employeesNum = queryModel.rowCount();
	QModelIndex index;  // 从模型取数据，需要索引
	for (int i = 0; i < employeesNum; i++) {
		index = queryModel.index(i, 0);       // 因为我们只查询了1列所以所有数据都在第1列，而行是根据i递增的
		employeeList << queryModel.data(index).toString();  // 将数据添加到链表中去
	}
}

/*
   对于此次的js文件的修改，我们先有一个txt文件的数据，然后根据这个模版数据取出需要替换
   的部分，作为字符串进行相应内容进行替换，然后将替换的部分添加到js文件当中，实现js文件
   的更新

   也就是说更新js文件需要存在一个模板
*/
// 重写js文件，为了我们方便实现接收和发送信息，js文件在初始化的时候就应该被调用
bool TalkWindowShell::createJSFile(QStringList & employeeList)
{
	// 读取对应的txt文件数据 -- 因为js文件需要在对应的txt文件中进行一些修改
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;    // 保存读到的数据

	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取msgtmpl.txt文本文件失败"));
		return false;
	}

	// 替换数据(external0保留自己发信息用,appendHtml0用作自己发信息使用)
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {  // 以写的方式进行，并且进行内容覆盖
	     // 先来更新最上面的初始化数据为空
		 // 准备好原始数据
		QString strSourceInitNull = "var external = null;";    // 更新空值
		QString strSourceInit = "external = channel.objects.external;";  // 更新初始化
		// 更新newWebChannel 
		QString strSourceNew = "new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external = channel.objects.external;\
		}); ";

		/*  这样写是不对的，因为QString是不能够支持""中再写双引号的，所以直接这样复制过来是不行的
		QString strSourceRecvHtml = "function recvHtml(msg){
	    $("#placeholder").append(external.msgLHtmlTmpl.format(msg));
	    window.scrollTo(0,document.body.scrollHeight); ;  
        };";*/

		// 那么怎样解决呢?  遇到字符串中有双引号的，我们需要将这段数据单独放到文件中
		// 然后从文件中读取出来，然后再赋值，这样Qt是支持的

		// 更新追加recvHtml，脚本中有双引号无法直接进行赋值，采用读文件方式
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resource/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("读取recvHtml.txt文本文件失败"));
			return false;
		}

		// 保存替换后的脚本
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		// 有多少个员工就进行多少次替换 -- 当for循环走完之后，我们的js文件就添加了对应的员工数量的相应变量和接收函数
		for (int i = 0; i < employeeList.length(); i++) {
			//编辑替换后的空值
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeeList.at(i)));
			// 将替换之后的字符串添加到替换中去
			strReplaceInitNull += strInitNull;

			// 编辑替换后的初始值
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";    // 加一个换行

			// 编辑替换后的newWebChannel
			QString strNew = strSourceNew;
			strSourceNew.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			// 编辑替换后的recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml%1").arg(employeeList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
 		}

		// 将对应的文件中需要替换的数据，进行一个替换(前面只是将需要替换的部分提取出来了，并没有对文件中的内容进行替换，当我们根据替换的数据准备好替换的文本之后，我们就可以将文件中的数据进行替换)
		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		// 文件流，将我们拼接好的数据进行写入到js文件
		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return true;  // 写成功
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("写msgtmpl.js文件失败"));
		return false;
	}

	return false;
}

// 用于将接收到的数据显示到网页
void TalkWindowShell::handleReceivedMsg(int senderID, int msgType, QString strMsg)
{
	// 定义一个文本编辑器 -- 我们自己定义的类
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);      // 设置文本

	if (msgType == 1) { // 文本信息
		// 如果是文本信息，就将当前的文档转换为html
		msgTextEdit.document()->toHtml();   // 转换为html
	}
	else if (msgType == 0) { // 表情信息
		// 往编辑器中添加表情，因为我们可能传递的是多个表情，所以我们使用一个for循环
		// 在数据中挨个的去拿，每次往出拿3个字节
		const int emotionWidth = 3;  // 表情的宽度
		int emotionNum = strMsg.length() / emotionWidth; // 计算出发送表情的数量 -- 就是使用发送的数据长度 / 一个表情的长度3
		for (int i = 0; i < emotionNum; i++) {
			// 往信息编辑器中添加表情数据，参数是一个整数的表情序号，我们从strMsg中截取3个长度的数据，然后i*3就是从表情序号开始
			// 截取出来的是字符串，我们参数需要的是整数，所以需要转换为整数
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	// 保存html的文本数据，根据当前的文档并且转换为html的格式
	QString html = msgTextEdit.document()->toHtml();

	// 对于文本html如果没有字体，则添加字体 -- 首先要保证是文本的 -- 因为对于文本信息，如果html没有设置字体，其就无法显示，因为显示文本必须要字体
	// 如果判断html是否是文本的呢? 如果是图片的，其内部一定包含有.png这个字符串，所以我们可以通过判断有没有这个字符串来判断是否是文本
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("文件不存在"));
			return;
		}

		if (!html.contains(fontHtml)) {
			html.replace(strMsg, fontHtml);
		}
	}

	// 将数据添加到网页中去 -- 因为右边的多页面部件，有很多的聊天窗口在里面，所以先设置聊天窗口
	// TalkWindow当前的，使用当前的去添加，不要全部去添加
	// 函数返回的是QWidget的指针，我们需要TalkWindow的指针，所以需要类型转换
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());  // 获取右侧多页面的当前页面

	// 获取到了当前的聊天窗口，那么就给它追加信息
	// 这里是TalkWindowShell当中，ui是TalkWindow中的私有成员，我们要访问别的类的私有成员
    // 需要将其设置为友元的关系
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderID));
}

// 使用数据库之后，参数进行了改变
void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow); // 初始化右边
	//connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

	// 左边需要一个列表项
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	// 为了方便点击列表项，实现改变右侧列表的显示
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);   // 设置被选中

	// 设置头像，判断是群聊还是单聊
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM table_department WHERE departmentID = %1").arg(uid.toInt());
	sqlDepModel.setQuery(strQuery);      // 给模型设置查询
	int rows = sqlDepModel.rowCount();

	if (rows == 0) { // 单聊
		strQuery = QString("SELECT picture FROM table_employees WHERE employeeID = %1").arg(uid.toInt());
		sqlDepModel.setQuery(strQuery);  // 设置sql
	}
	 
	QModelIndex index;
	index = sqlDepModel.index(0, 0); // 0行0列

	QImage img;
	img.load(sqlDepModel.data(index).toString());  // 加载索引中查询图片的数据

	// 设置头像 -- 增加数据库之后进行修改
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));  // 用图片构造图标，然后进行加载
	// talkWindowItem->setHeadPixmap("");    // 设置头像，目前为了效果而设置以后需要修改
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	// 点击关闭按钮之后，将聊天窗口之前添加的映射进行删除，并且将对应的talkwindow的资源
	// 进行释放，右边多页面显示的也需要进行删除
	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked,
		[talkWindowItem, talkWindow, aItem, this]() {
		// 先将映射中管理的资源进行删除
		m_talkwindowItemMap.remove(aItem);
		talkWindow->close();   // 将聊天窗口进行关闭
		ui.listWidget->takeItem(ui.listWidget->row(aItem)); // 根据aItem所在的列表的行号，对其就能行删除

		ui.rightStackedWidget->removeWidget(talkWindow);    // 删除多页面中的此页面
		if (ui.rightStackedWidget->count() < 1) {           // 如果多页中的页面个数已经少于1了，那么就将聊天窗口进行删除
			close();
		}
	});
}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkwindowItemMap;   // const修饰的函数，返回指针也必须是const的
}

/* 为了接收方(服务端)解析数据更加的方便，我们规定相应数据包的格式(也就是相应的协议)
   文本数据包的格式: 群聊标志(是不是群聊) + 发信息员工qq号 + 收信息员工的qq号(服务器端是通过广播的形式进行传递数据的，所以根据收信息的员工的qq，客户端就会判断当前是否是发给自己的，如果是就接受，不是就丢弃),如果群聊表示是1，也就是群聊，那么收信息员工的qq号就是群的qq号
                     + 信息类型(判断数据是什么类型) + 数据长度 + 数据
   表情数据包格式:  群聊标志 + 发信息员工qq号 + 收信息员工qq号(群qq号) + 信息类型 + 表情个数 + images + 表情的数据

   msgType:  0 表情或者是图片信息  1 是文本信息  2 是文件信息
*/
// 当发送信息的时候，网页视图会发送信号，并传递出发送的数据类型以及数据，方便我们在
// 此函数中处理并且使用网络通信传递数据
// 但是网络通信的时候，方便对方可以解析数据，我们对数据包的封装进行相应的格式规定(就是上面的)
void TalkWindowShell::updateSendTcpMessage(QString & strData, int & msgType, QString fileName)
{
	// 发送数据之前，首先要获取一下，当前在哪个聊天窗口，如果我们打开了多个聊天窗口，那自然要发送对窗口
	// 获取当前的聊天窗口
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());  //获取分页器中的当前显示的页面，也就是发送页面
	QString talkId = curTalkWindow->getTalkId();     // 获取当前窗体的id

	// 开始组合要发的数据
	// 1. 群聊标志
	QString strGroupFlag;
	if (talkId.length() == 4) {  // 群qq号的长度  --  因为我们群id号的长度就是4，所以如果是群那么其id的长度就是4
		strGroupFlag = "1";    // 是群，那么群聊标志就是"1"
	}
	else {
		strGroupFlag = "0";    // 否则就不是群
	}

	// 发信息员工qq号:  我们使用userLogin中定义的表示员工qq号的全局变量表示
	// 收信息员工的qq号: 群聊，使用群号，也就是talkId
	// 信息类型:  参数已经传递过来了

	// 计算数据长度
	int nstrDataLength = strData.length();  // 数据的长度
	int dataLength = QString::number(nstrDataLength).length();  // 计算出表示数据长度的长度 -- 这样你写5000个字符那么这个长度是4，因为5000是你数据的长度，这里计算的是你数据长度的长度
	// const int sourceDataLength = dataLength;      // 原始数据的长度

	QString strdataLength;  // 保存数据的长度
	QString strSend;        // 用于发送数据的字符串
	if (msgType == 1) {  // 发送的是文本信息
		/*注意:  这里我们发送10以内的字符就是长度为1，因为占有的位数为1，也就是
		         发50000个字符长度才是5，因为50000占有的位数为5*/
		// 文本信息的长度约定为5位(也就表示输入字符串的长度是个5位数否则就补全)，也就是我们发送的文本信息的长度保持为5，不够5就不全，超过5就报错 --  几万个字符已经足够了，所以5位已经足够发送了
		if (dataLength == 1) {      // 一位长
			strdataLength = "0000" + QString::number(nstrDataLength);  // 在前面拼接4个0,保证文本信息的长度为5
		}
		else if (dataLength == 2) {  // 两位长
			strdataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3) {
			strdataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4) {
			strdataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5) {
			strdataLength = QString::number(nstrDataLength);
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不合理的数据长度"));
		}

		// 拼接在发送文本数据的格式,talkId可以是群号也可以是人id，根据当前选择的窗口进行判断
		// 群聊标志(是不是群聊) + 发信息员工qq号 + 收信息员工的qq号 + 信息类型(判断数据是什么类型) + 数据长度 + 数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0)   // 发送表情信息
	{
	 // 表情数据包格式:  群聊标志 + 发信息员工qq号 + 收信息员工qq号(群qq号) + 信息类型 + 表情个数 + images + 表情的数据
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;    // 因为在我们发送表情信息的信号的时候，已经将表情个数 + images + 表情的数据拼接到了strData，所以这里就不需要进行拼接了
	}
	else if (msgType == 2) // 文件信息
	{
		// 文件数据包格式:   群聊标志 + 发信息员工qq号 + 收信息员工qq号 + 信息类型 + 文件的长度 + "bytes"(加这个是为了方便拆包) + 文件名称 + "data_begin"(也是为了方便解包) + 文件的内容

		// 获取文件的字节长度
		// QByteArray 转换为字节数组  strData.toUtf8()将对应的字符串转换为字节数组,调用length()获取字节数组的方法
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName/*通过参数传递进来了*/ + "data_begin" + strData;
	}

	// 向tcp中发送数据 -- 使用QByteArray字节数组来发送
	QByteArray dataBt;
	dataBt.resize(strSend.length());   // 根据发送数据的长度重新设置自己数组的长度
	// 进行赋值
	dataBt = strSend.toUtf8();   //  调用接口，将字符串转换为字节数组的形式
	m_tcpClientSocket->write(dataBt);  // 客户端给套接字缓存写入数据，方便tcp传输数据
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	// 获取到当前使用的聊天窗口
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	// 判断指针是否有效
	if (curTalkWindow) {
		curTalkWindow->addEmotionImage(emotionNum);
	}
}

/***********************************
  其实在实际开发当中，这些传递的数据格式都是我们自己指定的

  广播来的数据我们要对其进行解包，根据我们之前设定的格式
  数据包格式:  
  文本数据包格式:  群聊标志 + 发信息员工的id号 + 收信息员工的id(如果是群就是群id) + 
                   信息类型(文本为1) + 数据长度 + 文本数据
  
  表情数据格式:    群聊标志 + 发信息员工的id号 + 收信息员工的id(如果是群就是群id) + 
                   信息类型(表情为0) + 表情的个数 + images + 表情数据(我们使用的是三位的数据) 
  
  文件数据包格式:  群聊标志 + 发信息员工的id号 + 收信息员工的id(如果是群就是群id) + 
                   信息类型(文件为2) + 文件字节数 + bytes + 文件名 + data_begin(用来
				   定位数据的开始) + 文件数据

  群聊标志占一位，0表示单聊，1表示群聊
  信息类型占一位，0表示表情信息，1表示文本信息，2表示文件信息

  个人id占5位，群id占4位，数据长度占5位，表情名称占三位
  (注意:  当群聊标志为1的时候，那么数据包中就没有收信息员工qq号，而是受信息群的id号，
          当群聊标志为0的时候，那么数据包没有群qq号，而是收信息员工的id号)

   举例子:

   群聊文本信息:   1100012001100005hello  表示qq10001向群2001发送文本信息，长度为5，数据长度为5
   单聊图片信息:   0100011000201images060        表示qq10001向qq10002发送1个表情60.png
   群聊文件信息:   1100052000210bytestest.txtdata_beginhelloworld   
                                          表示qq10005向群2000发送文件，文件是test.txt,文件长度为10bytes,文件内容为helloworld

   群聊文件信息解析:  1 10001 2001 1 00005 hello
   单聊图片信息解析:  0 10001 10002 0 060
   群聊文件信息解析:  1 10005 2000 2 10 bytes test.txt data_begin helloworld
************************************/
void TalkWindowShell::processPendingData()
{
	// 根据这个方法返回对应的udp端口里面是否存在有没有处理的数据 -- 就是数据传递过来还没有进行处理
	while (m_udpReceive->hasPendingDatagrams()) {
		// 定义一些变量，用于解包时候的定位
		/*因为这些数据对应的长度，我们在封装数据包的时候，已经设置好了，也就是说其
		  是不会改变的，所以我们这里直接设置为const常量就可以了*/
		const static int groupFlagWidth = 1;   // 群聊标志在我们设置中只会占有一位
		const static int groupWidth = 4;       // 群id的宽度
		const static int employeeWidth = 5;    // 员工id的宽度
		const static int msgTypeWidth = 1;     // 信息类型的宽度
		const static int msgLengthWidth = 5;   // 文本信息长度的宽度
		const static int pictureWidth = 3;     // 表情图片的宽度

		QByteArray btData;  // 保存接受的数据

		// 读取udp数据
		// 根据udp类中提供的接口，获取传输数据中没有处理的数据的大小，然后对保存传输
		// 数据的字节数据设置大小，将其大小设置为未处理数据的大小
		btData.resize(m_udpReceive->pendingDatagramSize());    // 设置一下大小，就是我们收到的数据的大小
		m_udpReceive->readDatagram(btData.data(), btData.size());  // 读取udp数据 -- 放到btData的数组中

		QString strData = btData.data();   // 保存接受到的数据，以字符串形式，方便我们处理

		/*
		   定义一些变量，获取从数据中解析出来的数据
		*/
		QString strWindowID;               // 聊天窗口ID，群聊就是群号，单聊就是员工qq号
		QString strSendEmployeeID, strReceiveEmployeeID;  // 发送以及接受端的qq号
		QString strMsg;   // 保存发送的数据

		int msgLength;    // 数据的长度
		int msgType;      // 数据类型

		// 参数:    第一个: 传入开始截取的位置，  第二个:  表示截取的长度
		// 我们之前定义了一些常量，我们可以借助这些常量来定位到我们所要获取的数据的
		// 位置，groupFlagWidth群聊标志之后就是我们发送者的id，然后直接截取发送者id的长度就行 -- 我们之前都设置过
		strSendEmployeeID = strData.mid(groupFlagWidth,employeeWidth);   // 获取发送端的qq号，mid就是从对应字符串中截取相应的数据
		
		/*
		   这里我们应该对发送端的id进行一个判断: 因为我们接受的来自服务端的数据是以
		   广播的形式，也就是说，有可能我们自己发送的信息也可能会经过服务器端再传递
		   回来，所以我们应该进行相应的判断，如果接受到的数据是自己发送的就不做任何
		   处理
		*/
		if (strSendEmployeeID == gLoginEmployeeID) {  // 发送信息的用户是当前登录者，那么我们不做任何操作，进行一个返回
			return;  // 直接返回
		}

		if (btData[0] == '1') {  // 群聊
			// 群ID号
			strWindowID = strData.mid(groupFlagWidth + employeeWidth,groupWidth);  // 获取到群聊的id

			/*对字节数组直接下标操作，得到的是QChar类型的数据(其实就是char类型)，所以
			  我们在比较的时候就需要与''的字符进行比较*/
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1') {   // 文本信息
				msgType = 1;  // 设定信息类型
				msgLength = strData.mid(groupFlagWidth + employeeWidth + groupWidth
					+ msgTypeWidth, msgLengthWidth).toInt();  // 获取信息长度，其实就是从数据包中，根据前面数据的长度确定信息长度的位置然后截取值
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth
					+ msgTypeWidth + msgLengthWidth, msgLengthWidth);  // 获取数据


			}
			else if (cMsgType == '0') {  // 表情信息
				msgType = 0;  // 设定信息类型
				// 因为我们封装的图片信息数据包中有一个字符串images，而且其后面就是图片
				// 的名称，所以我们根据这个字符串的位置来确定图片的名字
                // 当然首先我们应该获取到images字符串的索引，调用下面的函数
				int posImages = strData.indexOf("images");
				// 获取表情的数据信息，我们这里直接不传递第二个参数，直接计算好好事位置将images后面的数据全部都截取出来
				strMsg = strData.mid(posImages + QString("images").length());  
			}
			else if (cMsgType == '2') {  // 文件信息
				msgType = 2;  // 获取信息类型

				// 计算一下bytes的宽度以及位置
				int bytesWidth = QString("bytes").length();  // 用bytes字符串构造一个QString然后调用其长度的函数
				int posBytes = strData.indexOf("bytes");     // 找到bytes的位置
				int posData_begin = strData.indexOf("data_begin");  // 找到data_begin的位置

				// 获取文件名称 -- 数据包中文件名在bytes和data_begin字符串之间
				// 这里第二个参数，我们通过data_begin的下标减去bytes尾部的下标来确定文件名称的宽度，也就是截取的个数
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - (posBytes + bytesWidth));
				gFileName = fileName;  // 赋值文件名称

				// 文件内容 -- 先计算所在位置
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").size();  // 获取到数据的位置
				strMsg = strData.mid(posData);  // 截取文件数据，这里没有第二个参数，就是截取从开始位置到字符串结尾的所有数据
				gFileData = strMsg;   // 赋值文件内容

				/*获取一下发送者的姓名,因为我们收到文件的时候，我们需要提醒一下，收到谁谁的
				  文件，你是否要接受这个文件*/
				// 根据employeeID获取发送者名字
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery query(QString("SELECT employee_name FROM table_employees WHERE employeeID = %1").arg(employeeID));
				query.exec();  // 执行sql语句

				if (query.first()) {  // 是否查询出结果 -- 这里结果集指向第一个数据和是否查询成功结合起来了
					sender = query.value(0).toString();
				}

				// 接收文件的后续操作
				ReceieveFile *recvFile = new ReceieveFile(this);
				connect(recvFile, &ReceieveFile::refuseFile, [this]() {
					return; });    // 如果我们不接受这个文件，也就是发送了拒绝接受的信号，就什么操作也不做，直接退出

				QString msgLabel = QString::fromLocal8Bit("收到来自") + sender
					+ QString::fromLocal8Bit("发来的文件，是否接收?");

				recvFile->setMsg(msgLabel);   // 对接收文件设置提示信息
				recvFile->show();              // 显示接收的文件
			}
		}
		else {  // 单聊
			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth,employeeWidth);   // 获取接收方的id
			strWindowID = strSendEmployeeID;   // 发送者的窗口就是发送者的ID号

			// 判断是否是发给我的信息，不是发给我的就不做处理
			if (strReceiveEmployeeID != gLoginEmployeeID) {
				return;   // 如果不是发给我的信息那么我就直接返回不做任何的处理了
			}

			// 获取信息类型
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1') { // 文本信息
				msgType = 1;

				// 文本信息的长度
				msgLength = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
				         + msgTypeWidth,msgLengthWidth).toInt();  // 截取出来的是字符串，我们要转换为int类型

				// 获取文本信息
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth + msgLengthWidth, msgLength);
			}
			else if (cMsgType == '0') { // 表情信息
				msgType = 0;
				int posImages = strData.indexOf("images");  // 返回对应字符串开始的位置
				int imagesWidth = QString("images").length();

				// 获取数据
				strMsg = strData.mid(posImages + imagesWidth);  // 截取从指定位置开始后面的全部信息
			}
			else if (cMsgType == '2') { // 文件信息
				msgType = 2;
				/*获取标志子字符串的位置和宽度*/
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidtg = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				// 保存文件名称 -- 第二个参数是数据包中文件名称的大小
				QString fileName = strData.mid(bytesWidth + posBytes, posData_begin - bytesWidth - posBytes);
				gFileName = fileName;

				// 文件内容  --  data_begin之后的都是内容了
				strMsg = strData.mid(posData_begin);
				gFileData = strMsg;

				// 获取发送者名称
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery query(QString("SELECT employee_name FROM table_employees WHERE employeeID = %1").arg(employeeID));
				query.exec();  // 执行sql语句

				if (query.first()) {  // 是否查询出结果 -- 这里结果集指向第一个数据和是否查询成功结合起来了
					sender = query.value(0).toString();
				}

				// 接收文件的后续操作
				ReceieveFile *recvFile = new ReceieveFile(this);
				connect(recvFile, &ReceieveFile::refuseFile, [this]() {
					return; });    // 如果我们不接受这个文件，也就是发送了拒绝接受的信号，就什么操作也不做，直接退出

				QString msgLabel = QString::fromLocal8Bit("收到来自") + sender
					+ QString::fromLocal8Bit("发来的文件，是否接收?");

				recvFile->setMsg(msgLabel);   // 对接收文件设置提示信息
				recvFile->show();              // 显示接收的文件
			}
		}

		// 激活窗口
		/*
		    数据解析出来之后，我们就需要将解析出来的数据添加到网页中去，这样可以显示
			你去收到的数据

			接收信息的时候，如果我们打开的是一个群，接收到信息的是另外一个群，那么我们
			这时候可以将接收到的信息的群设置为活动窗口

			此处将业务简单化，如果发送信息的群已经打开，我们就将其设置为活动窗口，然后
			将信息显示在页面中，当然如果不是我们当前显示的窗口，那就将其设置为活动窗口

			如果窗口没有打开，那么我们就不做任何操作
		*/

        // 将聊天窗口设置为活动的窗口
		// 获取窗口 -- 使用窗口管理类的方法直接查询 -- 根据窗口id查询是否我们已经打开这个窗口
		// 返回一个聊天窗口的地址
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget) {   // 打开了那么就设置为活动窗口
			this->setCurrentWidget(widget);     // 根据聊天窗口的地址设置为聊天活动窗口
			
		    // 将左侧列表和活动窗口进行同步 -- 我们之间定义了一个映射，每一个聊天窗口都对应了一个左侧的列表
			// 所以可以根据窗口的地址获取到左侧列表的项
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);     // key(值)根据映射值返回映射键
			item->setSelected(true);   // 设置当前项被选中
		}
		else { // 发送的聊天窗口没有打开，那么直接返回
			return;
		}

		int senderID = strSendEmployeeID.toInt();  // 将其转换为整形
		// 将解析出的数据显示到网页中去 -- 需要给函数传入参数实现这一功能
		// 如果数据类型不等于2，也就是发送的不是文件的时候我们调用这个数据，因为发送
		// 文件我们需要特殊处理，不需要进行信息的显示
		// 文件信息另外做处理
		if (msgType != 2) {
			handleReceivedMsg(senderID, msgType, strMsg);
		}
	}
}

void TalkWindowShell::onEmotionBtnClicked(bool checked)
{
	// 表情按钮被点击主要涉及到我们的表情窗口是否可见
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());  // 返回当前的可见状态，
																 // 并且取反，如果当前可见点击后设置为不可见，另一种情况也类似

	// 进行坐标的设置
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));       // 将当前控件的相对位置转换为屏幕的绝对位置

	// 设置偏移
	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);

	// 设置位置
	m_emotionWindow->move(emotionPoint);    // 将窗移动到对应的位置
}

// 左边列表选择了具体的窗口，那么就进行窗口的转换
void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item) {
	QWidget* talkWindowWidget = m_talkwindowItemMap.find(item).value();   // 取到项对应的窗体    
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}
