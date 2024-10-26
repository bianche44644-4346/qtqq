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

/*��g��ʾ��һ��ȫ�ֵı���*/
// const int gtcpPort = 6666;  // �˿ں�  --  �˿ں�һ�㲻��ı䣬�������ǽ�������Ϊconst
const int gtcpPort = 8888;     // ����˿ں�Ӧ�úͷ����������õĶ˿ں���һ���ģ����������ʧ��
const int gudpPort = 6666;     // udp�Ķ˿ں�
extern QString gLoginEmployeeID;    // ��½�ߵ�qq��

QString gFileName;  // �����ļ�����
QString gFileData;  // �����ļ�����

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();

	// �ڹ��캯�����ó�ʼ��tcp��udp�ĺ���
	initTcpSocket();
	initUdpSocket();

	
	// ���ǲ���Ҫʲôʱ�򶼸��£����js�ļ�Ϊ���ˣ������ٽ��и��£�����������Ҫ�ж�
	// ��ǰ��js�ļ��Ƿ�Ϊ��
	QFile file("Resources/MainWindow/MsgHtml/msgtmpl.js");  // ��ӵ���Դ�ļ��п���ʹ��:/�����ʣ����������ô�Ͳ�Ҫ��:/
	
	if (!file.size()) {  // �ļ�����Ϊ0����ôд������
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);  // ��ȡ����Ա��qq�ŵ�����

		if (!createJSFile(employeesIDList)) {            // �ж��Ƿ����js�ļ��Ƿ�ʧ��
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����js�ļ�д������ʧ��"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	/*   // �ͷű������
	   delete m_emotionWindow;
	   m_emotionWindow = nullptr;
	 */
}

void TalkWindowShell::initControl()
{
	// ������ʽ��
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("���촰��"));

	// ���鴰�ڵ��û���û�е�����鰴ť��ʱ�������ǲ���Ҫ������ʾ�ģ�ֻ�е���˲Ż�ϵ����
	m_emotionWindow = new EmotionWindow;   // ������鴰��
	// û�е����ʱ�����ر��鴰��
	m_emotionWindow->hide();               // ���ر��鴰��

	// �����Ĳ�������һ�³ߴ�(�����������óߴ磬������ö��ٻ����ұ����ö���)
	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;    // �����154���Ҳ��ȵ��ڴ��ڿ��-154
	ui.splitter->setSizes(leftWidgetSize);     // ���������óߴ�

	// �������ʾ�ڿ����ʽ������ǰ������Ϊ�丸��
	ui.listWidget->setStyle(new CustomProxyStyle(this));

	// �����ź���� -- �������б������͸�����Ӧ�����촰�ڣ�����ѡ�б��飬�͸��±��鴰��
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

void TalkWindowShell::initTcpSocket()
{
	m_tcpClientSocket = new QTcpSocket(this);   // ����tcp
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceive = new QUdpSocket(this);

	// �ͻ��˶˿ڰ󶨣�Ҳ���Զ���˿ڣ���ֹ�˿ڱ�ռ�� -- �ͻ���ֻ��Ҫ��һ���˿ڣ����úͷ�������һ������Ҫ�Զ���˿ڽ��й㲥
	for (quint16 port = gudpPort; port < gudpPort + 200; port++) {
		// ��һ���˿ڳɹ������� -- ���ù���ķ�ʽ�󶨶˿�
		if (m_udpReceive->bind(port, QUdpSocket::ShareAddress)) {
			break;
		}
	}
	/*�󶨳ɹ�֮�� -- ����һ��׼����ȡ���źţ�Ȼ�󴥷��ۺ������д���*/
	connect(m_udpReceive, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

void  TalkWindowShell::getEmployeesID(QStringList& employeeList)
{
	QSqlQueryModel queryModel;
	// ����������Ч���ݵ�Ա��id
	queryModel.setQuery("SELECT employeeID FROM table_employees WHERE status = 1");

	// ����ģ�͵�������(Ա��������)
	int employeesNum = queryModel.rowCount();
	QModelIndex index;  // ��ģ��ȡ���ݣ���Ҫ����
	for (int i = 0; i < employeesNum; i++) {
		index = queryModel.index(i, 0);       // ��Ϊ����ֻ��ѯ��1�������������ݶ��ڵ�1�У������Ǹ���i������
		employeeList << queryModel.data(index).toString();  // ��������ӵ�������ȥ
	}
}

/*
   ���ڴ˴ε�js�ļ����޸ģ���������һ��txt�ļ������ݣ�Ȼ��������ģ������ȡ����Ҫ�滻
   �Ĳ��֣���Ϊ�ַ���������Ӧ���ݽ����滻��Ȼ���滻�Ĳ�����ӵ�js�ļ����У�ʵ��js�ļ�
   �ĸ���

   Ҳ����˵����js�ļ���Ҫ����һ��ģ��
*/
// ��дjs�ļ���Ϊ�����Ƿ���ʵ�ֽ��պͷ�����Ϣ��js�ļ��ڳ�ʼ����ʱ���Ӧ�ñ�����
bool TalkWindowShell::createJSFile(QStringList & employeeList)
{
	// ��ȡ��Ӧ��txt�ļ����� -- ��Ϊjs�ļ���Ҫ�ڶ�Ӧ��txt�ļ��н���һЩ�޸�
	QString strFileTxt = "Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;    // �������������

	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ȡmsgtmpl.txt�ı��ļ�ʧ��"));
		return false;
	}

	// �滻����(external0�����Լ�����Ϣ��,appendHtml0�����Լ�����Ϣʹ��)
	QFile fileWrite("Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {  // ��д�ķ�ʽ���У����ҽ������ݸ���
	     // ��������������ĳ�ʼ������Ϊ��
		 // ׼����ԭʼ����
		QString strSourceInitNull = "var external = null;";    // ���¿�ֵ
		QString strSourceInit = "external = channel.objects.external;";  // ���³�ʼ��
		// ����newWebChannel 
		QString strSourceNew = "new QWebChannel(qt.webChannelTransport,\
			function(channel) {\
			external = channel.objects.external;\
		}); ";

		/*  ����д�ǲ��Եģ���ΪQString�ǲ��ܹ�֧��""����д˫���ŵģ�����ֱ���������ƹ����ǲ��е�
		QString strSourceRecvHtml = "function recvHtml(msg){
	    $("#placeholder").append(external.msgLHtmlTmpl.format(msg));
	    window.scrollTo(0,document.body.scrollHeight); ;  
        };";*/

		// ��ô���������?  �����ַ�������˫���ŵģ�������Ҫ��������ݵ����ŵ��ļ���
		// Ȼ����ļ��ж�ȡ������Ȼ���ٸ�ֵ������Qt��֧�ֵ�

		// ����׷��recvHtml���ű�����˫�����޷�ֱ�ӽ��и�ֵ�����ö��ļ���ʽ
		QString strSourceRecvHtml;
		QFile fileRecvHtml("Resource/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ȡrecvHtml.txt�ı��ļ�ʧ��"));
			return false;
		}

		// �����滻��Ľű�
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		// �ж��ٸ�Ա���ͽ��ж��ٴ��滻 -- ��forѭ������֮�����ǵ�js�ļ�������˶�Ӧ��Ա����������Ӧ�����ͽ��պ���
		for (int i = 0; i < employeeList.length(); i++) {
			//�༭�滻��Ŀ�ֵ
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1").arg(employeeList.at(i)));
			// ���滻֮����ַ�����ӵ��滻��ȥ
			strReplaceInitNull += strInitNull;

			// �༭�滻��ĳ�ʼֵ
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceInit += strInit;
			strReplaceInit += "\n";    // ��һ������

			// �༭�滻���newWebChannel
			QString strNew = strSourceNew;
			strSourceNew.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";

			// �༭�滻���recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1").arg(employeeList.at(i)));
			strRecvHtml.replace("recvHtml", QString("recvHtml%1").arg(employeeList.at(i)));
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
 		}

		// ����Ӧ���ļ�����Ҫ�滻�����ݣ�����һ���滻(ǰ��ֻ�ǽ���Ҫ�滻�Ĳ�����ȡ�����ˣ���û�ж��ļ��е����ݽ����滻�������Ǹ����滻������׼�����滻���ı�֮�����ǾͿ��Խ��ļ��е����ݽ����滻)
		strFile.replace(strSourceInitNull, strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

		// �ļ�����������ƴ�Ӻõ����ݽ���д�뵽js�ļ�
		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return true;  // д�ɹ�
	}
	else {
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("дmsgtmpl.js�ļ�ʧ��"));
		return false;
	}

	return false;
}

// ���ڽ����յ���������ʾ����ҳ
void TalkWindowShell::handleReceivedMsg(int senderID, int msgType, QString strMsg)
{
	// ����һ���ı��༭�� -- �����Լ��������
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);      // �����ı�

	if (msgType == 1) { // �ı���Ϣ
		// ������ı���Ϣ���ͽ���ǰ���ĵ�ת��Ϊhtml
		msgTextEdit.document()->toHtml();   // ת��Ϊhtml
	}
	else if (msgType == 0) { // ������Ϣ
		// ���༭������ӱ��飬��Ϊ���ǿ��ܴ��ݵ��Ƕ�����飬��������ʹ��һ��forѭ��
		// �������а�����ȥ�ã�ÿ��������3���ֽ�
		const int emotionWidth = 3;  // ����Ŀ��
		int emotionNum = strMsg.length() / emotionWidth; // ��������ͱ�������� -- ����ʹ�÷��͵����ݳ��� / һ������ĳ���3
		for (int i = 0; i < emotionNum; i++) {
			// ����Ϣ�༭������ӱ������ݣ�������һ�������ı�����ţ����Ǵ�strMsg�н�ȡ3�����ȵ����ݣ�Ȼ��i*3���Ǵӱ�����ſ�ʼ
			// ��ȡ���������ַ��������ǲ�����Ҫ����������������Ҫת��Ϊ����
			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionWidth, emotionWidth).toInt());
		}
	}

	// ����html���ı����ݣ����ݵ�ǰ���ĵ�����ת��Ϊhtml�ĸ�ʽ
	QString html = msgTextEdit.document()->toHtml();

	// �����ı�html���û�����壬��������� -- ����Ҫ��֤���ı��� -- ��Ϊ�����ı���Ϣ�����htmlû���������壬����޷���ʾ����Ϊ��ʾ�ı�����Ҫ����
	// ����ж�html�Ƿ����ı�����? �����ͼƬ�ģ����ڲ�һ��������.png����ַ������������ǿ���ͨ���ж���û������ַ������ж��Ƿ����ı�
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else {
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�ļ�������"));
			return;
		}

		if (!html.contains(fontHtml)) {
			html.replace(strMsg, fontHtml);
		}
	}

	// ��������ӵ���ҳ��ȥ -- ��Ϊ�ұߵĶ�ҳ�沿�����кܶ�����촰�������棬�������������촰��
	// TalkWindow��ǰ�ģ�ʹ�õ�ǰ��ȥ��ӣ���Ҫȫ��ȥ���
	// �������ص���QWidget��ָ�룬������ҪTalkWindow��ָ�룬������Ҫ����ת��
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());  // ��ȡ�Ҳ��ҳ��ĵ�ǰҳ��

	// ��ȡ���˵�ǰ�����촰�ڣ���ô�͸���׷����Ϣ
	// ������TalkWindowShell���У�ui��TalkWindow�е�˽�г�Ա������Ҫ���ʱ�����˽�г�Ա
    // ��Ҫ��������Ϊ��Ԫ�Ĺ�ϵ
	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderID));
}

// ʹ�����ݿ�֮�󣬲��������˸ı�
void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow); // ��ʼ���ұ�
	//connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()), talkWindow, SLOT(onSetEmotionBtnStatus()));

	// �����Ҫһ���б���
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	// Ϊ�˷������б��ʵ�ָı��Ҳ��б����ʾ
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);   // ���ñ�ѡ��

	// ����ͷ���ж���Ⱥ�Ļ��ǵ���
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM table_department WHERE departmentID = %1").arg(uid.toInt());
	sqlDepModel.setQuery(strQuery);      // ��ģ�����ò�ѯ
	int rows = sqlDepModel.rowCount();

	if (rows == 0) { // ����
		strQuery = QString("SELECT picture FROM table_employees WHERE employeeID = %1").arg(uid.toInt());
		sqlDepModel.setQuery(strQuery);  // ����sql
	}
	 
	QModelIndex index;
	index = sqlDepModel.index(0, 0); // 0��0��

	QImage img;
	img.load(sqlDepModel.data(index).toString());  // ���������в�ѯͼƬ������

	// ����ͷ�� -- �������ݿ�֮������޸�
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));  // ��ͼƬ����ͼ�꣬Ȼ����м���
	// talkWindowItem->setHeadPixmap("");    // ����ͷ��ĿǰΪ��Ч���������Ժ���Ҫ�޸�
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	// ����رհ�ť֮�󣬽����촰��֮ǰ��ӵ�ӳ�����ɾ�������ҽ���Ӧ��talkwindow����Դ
	// �����ͷţ��ұ߶�ҳ����ʾ��Ҳ��Ҫ����ɾ��
	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked,
		[talkWindowItem, talkWindow, aItem, this]() {
		// �Ƚ�ӳ���й������Դ����ɾ��
		m_talkwindowItemMap.remove(aItem);
		talkWindow->close();   // �����촰�ڽ��йر�
		ui.listWidget->takeItem(ui.listWidget->row(aItem)); // ����aItem���ڵ��б���кţ����������ɾ��

		ui.rightStackedWidget->removeWidget(talkWindow);    // ɾ����ҳ���еĴ�ҳ��
		if (ui.rightStackedWidget->count() < 1) {           // �����ҳ�е�ҳ������Ѿ�����1�ˣ���ô�ͽ����촰�ڽ���ɾ��
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
	return m_talkwindowItemMap;   // const���εĺ���������ָ��Ҳ������const��
}

/* Ϊ�˽��շ�(�����)�������ݸ��ӵķ��㣬���ǹ涨��Ӧ���ݰ��ĸ�ʽ(Ҳ������Ӧ��Э��)
   �ı����ݰ��ĸ�ʽ: Ⱥ�ı�־(�ǲ���Ⱥ��) + ����ϢԱ��qq�� + ����ϢԱ����qq��(����������ͨ���㲥����ʽ���д������ݵģ����Ը�������Ϣ��Ա����qq���ͻ��˾ͻ��жϵ�ǰ�Ƿ��Ƿ����Լ��ģ�����Ǿͽ��ܣ����ǾͶ���),���Ⱥ�ı�ʾ��1��Ҳ����Ⱥ�ģ���ô����ϢԱ����qq�ž���Ⱥ��qq��
                     + ��Ϣ����(�ж�������ʲô����) + ���ݳ��� + ����
   �������ݰ���ʽ:  Ⱥ�ı�־ + ����ϢԱ��qq�� + ����ϢԱ��qq��(Ⱥqq��) + ��Ϣ���� + ������� + images + ���������

   msgType:  0 ���������ͼƬ��Ϣ  1 ���ı���Ϣ  2 ���ļ���Ϣ
*/
// ��������Ϣ��ʱ����ҳ��ͼ�ᷢ���źţ������ݳ����͵����������Լ����ݣ�����������
// �˺����д�����ʹ������ͨ�Ŵ�������
// ��������ͨ�ŵ�ʱ�򣬷���Է����Խ������ݣ����Ƕ����ݰ��ķ�װ������Ӧ�ĸ�ʽ�涨(���������)
void TalkWindowShell::updateSendTcpMessage(QString & strData, int & msgType, QString fileName)
{
	// ��������֮ǰ������Ҫ��ȡһ�£���ǰ���ĸ����촰�ڣ�������Ǵ��˶�����촰�ڣ�����ȻҪ���ͶԴ���
	// ��ȡ��ǰ�����촰��
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());  //��ȡ��ҳ���еĵ�ǰ��ʾ��ҳ�棬Ҳ���Ƿ���ҳ��
	QString talkId = curTalkWindow->getTalkId();     // ��ȡ��ǰ�����id

	// ��ʼ���Ҫ��������
	// 1. Ⱥ�ı�־
	QString strGroupFlag;
	if (talkId.length() == 4) {  // Ⱥqq�ŵĳ���  --  ��Ϊ����Ⱥid�ŵĳ��Ⱦ���4�����������Ⱥ��ô��id�ĳ��Ⱦ���4
		strGroupFlag = "1";    // ��Ⱥ����ôȺ�ı�־����"1"
	}
	else {
		strGroupFlag = "0";    // ����Ͳ���Ⱥ
	}

	// ����ϢԱ��qq��:  ����ʹ��userLogin�ж���ı�ʾԱ��qq�ŵ�ȫ�ֱ�����ʾ
	// ����ϢԱ����qq��: Ⱥ�ģ�ʹ��Ⱥ�ţ�Ҳ����talkId
	// ��Ϣ����:  �����Ѿ����ݹ�����

	// �������ݳ���
	int nstrDataLength = strData.length();  // ���ݵĳ���
	int dataLength = QString::number(nstrDataLength).length();  // �������ʾ���ݳ��ȵĳ��� -- ������д5000���ַ���ô���������4����Ϊ5000�������ݵĳ��ȣ����������������ݳ��ȵĳ���
	// const int sourceDataLength = dataLength;      // ԭʼ���ݵĳ���

	QString strdataLength;  // �������ݵĳ���
	QString strSend;        // ���ڷ������ݵ��ַ���
	if (msgType == 1) {  // ���͵����ı���Ϣ
		/*ע��:  �������Ƿ���10���ڵ��ַ����ǳ���Ϊ1����Ϊռ�е�λ��Ϊ1��Ҳ����
		         ��50000���ַ����Ȳ���5����Ϊ50000ռ�е�λ��Ϊ5*/
		// �ı���Ϣ�ĳ���Լ��Ϊ5λ(Ҳ�ͱ�ʾ�����ַ����ĳ����Ǹ�5λ������Ͳ�ȫ)��Ҳ�������Ƿ��͵��ı���Ϣ�ĳ��ȱ���Ϊ5������5�Ͳ�ȫ������5�ͱ��� --  ������ַ��Ѿ��㹻�ˣ�����5λ�Ѿ��㹻������
		if (dataLength == 1) {      // һλ��
			strdataLength = "0000" + QString::number(nstrDataLength);  // ��ǰ��ƴ��4��0,��֤�ı���Ϣ�ĳ���Ϊ5
		}
		else if (dataLength == 2) {  // ��λ��
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
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����������ݳ���"));
		}

		// ƴ���ڷ����ı����ݵĸ�ʽ,talkId������Ⱥ��Ҳ��������id�����ݵ�ǰѡ��Ĵ��ڽ����ж�
		// Ⱥ�ı�־(�ǲ���Ⱥ��) + ����ϢԱ��qq�� + ����ϢԱ����qq�� + ��Ϣ����(�ж�������ʲô����) + ���ݳ��� + ����
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0)   // ���ͱ�����Ϣ
	{
	 // �������ݰ���ʽ:  Ⱥ�ı�־ + ����ϢԱ��qq�� + ����ϢԱ��qq��(Ⱥqq��) + ��Ϣ���� + ������� + images + ���������
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;    // ��Ϊ�����Ƿ��ͱ�����Ϣ���źŵ�ʱ���Ѿ���������� + images + ���������ƴ�ӵ���strData����������Ͳ���Ҫ����ƴ����
	}
	else if (msgType == 2) // �ļ���Ϣ
	{
		// �ļ����ݰ���ʽ:   Ⱥ�ı�־ + ����ϢԱ��qq�� + ����ϢԱ��qq�� + ��Ϣ���� + �ļ��ĳ��� + "bytes"(�������Ϊ�˷�����) + �ļ����� + "data_begin"(Ҳ��Ϊ�˷�����) + �ļ�������

		// ��ȡ�ļ����ֽڳ���
		// QByteArray ת��Ϊ�ֽ�����  strData.toUtf8()����Ӧ���ַ���ת��Ϊ�ֽ�����,����length()��ȡ�ֽ�����ķ���
		QByteArray bt = strData.toUtf8();
		QString strLength = QString::number(bt.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName/*ͨ���������ݽ�����*/ + "data_begin" + strData;
	}

	// ��tcp�з������� -- ʹ��QByteArray�ֽ�����������
	QByteArray dataBt;
	dataBt.resize(strSend.length());   // ���ݷ������ݵĳ������������Լ�����ĳ���
	// ���и�ֵ
	dataBt = strSend.toUtf8();   //  ���ýӿڣ����ַ���ת��Ϊ�ֽ��������ʽ
	m_tcpClientSocket->write(dataBt);  // �ͻ��˸��׽��ֻ���д�����ݣ�����tcp��������
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	// ��ȡ����ǰʹ�õ����촰��
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	// �ж�ָ���Ƿ���Ч
	if (curTalkWindow) {
		curTalkWindow->addEmotionImage(emotionNum);
	}
}

/***********************************
  ��ʵ��ʵ�ʿ������У���Щ���ݵ����ݸ�ʽ���������Լ�ָ����

  �㲥������������Ҫ������н������������֮ǰ�趨�ĸ�ʽ
  ���ݰ���ʽ:  
  �ı����ݰ���ʽ:  Ⱥ�ı�־ + ����ϢԱ����id�� + ����ϢԱ����id(�����Ⱥ����Ⱥid) + 
                   ��Ϣ����(�ı�Ϊ1) + ���ݳ��� + �ı�����
  
  �������ݸ�ʽ:    Ⱥ�ı�־ + ����ϢԱ����id�� + ����ϢԱ����id(�����Ⱥ����Ⱥid) + 
                   ��Ϣ����(����Ϊ0) + ����ĸ��� + images + ��������(����ʹ�õ�����λ������) 
  
  �ļ����ݰ���ʽ:  Ⱥ�ı�־ + ����ϢԱ����id�� + ����ϢԱ����id(�����Ⱥ����Ⱥid) + 
                   ��Ϣ����(�ļ�Ϊ2) + �ļ��ֽ��� + bytes + �ļ��� + data_begin(����
				   ��λ���ݵĿ�ʼ) + �ļ�����

  Ⱥ�ı�־ռһλ��0��ʾ���ģ�1��ʾȺ��
  ��Ϣ����ռһλ��0��ʾ������Ϣ��1��ʾ�ı���Ϣ��2��ʾ�ļ���Ϣ

  ����idռ5λ��Ⱥidռ4λ�����ݳ���ռ5λ����������ռ��λ
  (ע��:  ��Ⱥ�ı�־Ϊ1��ʱ����ô���ݰ��о�û������ϢԱ��qq�ţ���������ϢȺ��id�ţ�
          ��Ⱥ�ı�־Ϊ0��ʱ����ô���ݰ�û��Ⱥqq�ţ���������ϢԱ����id��)

   ������:

   Ⱥ���ı���Ϣ:   1100012001100005hello  ��ʾqq10001��Ⱥ2001�����ı���Ϣ������Ϊ5�����ݳ���Ϊ5
   ����ͼƬ��Ϣ:   0100011000201images060        ��ʾqq10001��qq10002����1������60.png
   Ⱥ���ļ���Ϣ:   1100052000210bytestest.txtdata_beginhelloworld   
                                          ��ʾqq10005��Ⱥ2000�����ļ����ļ���test.txt,�ļ�����Ϊ10bytes,�ļ�����Ϊhelloworld

   Ⱥ���ļ���Ϣ����:  1 10001 2001 1 00005 hello
   ����ͼƬ��Ϣ����:  0 10001 10002 0 060
   Ⱥ���ļ���Ϣ����:  1 10005 2000 2 10 bytes test.txt data_begin helloworld
************************************/
void TalkWindowShell::processPendingData()
{
	// ��������������ض�Ӧ��udp�˿������Ƿ������û�д�������� -- �������ݴ��ݹ�����û�н��д���
	while (m_udpReceive->hasPendingDatagrams()) {
		// ����һЩ���������ڽ��ʱ��Ķ�λ
		/*��Ϊ��Щ���ݶ�Ӧ�ĳ��ȣ������ڷ�װ���ݰ���ʱ���Ѿ����ú��ˣ�Ҳ����˵��
		  �ǲ���ı�ģ�������������ֱ������Ϊconst�����Ϳ�����*/
		const static int groupFlagWidth = 1;   // Ⱥ�ı�־������������ֻ��ռ��һλ
		const static int groupWidth = 4;       // Ⱥid�Ŀ��
		const static int employeeWidth = 5;    // Ա��id�Ŀ��
		const static int msgTypeWidth = 1;     // ��Ϣ���͵Ŀ��
		const static int msgLengthWidth = 5;   // �ı���Ϣ���ȵĿ��
		const static int pictureWidth = 3;     // ����ͼƬ�Ŀ��

		QByteArray btData;  // ������ܵ�����

		// ��ȡudp����
		// ����udp�����ṩ�Ľӿڣ���ȡ����������û�д�������ݵĴ�С��Ȼ��Ա��洫��
		// ���ݵ��ֽ��������ô�С�������С����Ϊδ�������ݵĴ�С
		btData.resize(m_udpReceive->pendingDatagramSize());    // ����һ�´�С�����������յ������ݵĴ�С
		m_udpReceive->readDatagram(btData.data(), btData.size());  // ��ȡudp���� -- �ŵ�btData��������

		QString strData = btData.data();   // ������ܵ������ݣ����ַ�����ʽ���������Ǵ���

		/*
		   ����һЩ��������ȡ�������н�������������
		*/
		QString strWindowID;               // ���촰��ID��Ⱥ�ľ���Ⱥ�ţ����ľ���Ա��qq��
		QString strSendEmployeeID, strReceiveEmployeeID;  // �����Լ����ܶ˵�qq��
		QString strMsg;   // ���淢�͵�����

		int msgLength;    // ���ݵĳ���
		int msgType;      // ��������

		// ����:    ��һ��: ���뿪ʼ��ȡ��λ�ã�  �ڶ���:  ��ʾ��ȡ�ĳ���
		// ����֮ǰ������һЩ���������ǿ��Խ�����Щ��������λ��������Ҫ��ȡ�����ݵ�
		// λ�ã�groupFlagWidthȺ�ı�־֮��������Ƿ����ߵ�id��Ȼ��ֱ�ӽ�ȡ������id�ĳ��Ⱦ��� -- ����֮ǰ�����ù�
		strSendEmployeeID = strData.mid(groupFlagWidth,employeeWidth);   // ��ȡ���Ͷ˵�qq�ţ�mid���ǴӶ�Ӧ�ַ����н�ȡ��Ӧ������
		
		/*
		   ��������Ӧ�öԷ��Ͷ˵�id����һ���ж�: ��Ϊ���ǽ��ܵ����Է���˵���������
		   �㲥����ʽ��Ҳ����˵���п��������Լ����͵���ϢҲ���ܻᾭ�����������ٴ���
		   ��������������Ӧ�ý�����Ӧ���жϣ�������ܵ����������Լ����͵ľͲ����κ�
		   ����
		*/
		if (strSendEmployeeID == gLoginEmployeeID) {  // ������Ϣ���û��ǵ�ǰ��¼�ߣ���ô���ǲ����κβ���������һ������
			return;  // ֱ�ӷ���
		}

		if (btData[0] == '1') {  // Ⱥ��
			// ȺID��
			strWindowID = strData.mid(groupFlagWidth + employeeWidth,groupWidth);  // ��ȡ��Ⱥ�ĵ�id

			/*���ֽ�����ֱ���±�������õ�����QChar���͵�����(��ʵ����char����)������
			  �����ڱȽϵ�ʱ�����Ҫ��''���ַ����бȽ�*/
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1') {   // �ı���Ϣ
				msgType = 1;  // �趨��Ϣ����
				msgLength = strData.mid(groupFlagWidth + employeeWidth + groupWidth
					+ msgTypeWidth, msgLengthWidth).toInt();  // ��ȡ��Ϣ���ȣ���ʵ���Ǵ����ݰ��У�����ǰ�����ݵĳ���ȷ����Ϣ���ȵ�λ��Ȼ���ȡֵ
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth
					+ msgTypeWidth + msgLengthWidth, msgLengthWidth);  // ��ȡ����


			}
			else if (cMsgType == '0') {  // ������Ϣ
				msgType = 0;  // �趨��Ϣ����
				// ��Ϊ���Ƿ�װ��ͼƬ��Ϣ���ݰ�����һ���ַ���images��������������ͼƬ
				// �����ƣ��������Ǹ�������ַ�����λ����ȷ��ͼƬ������
                // ��Ȼ��������Ӧ�û�ȡ��images�ַ�������������������ĺ���
				int posImages = strData.indexOf("images");
				// ��ȡ�����������Ϣ����������ֱ�Ӳ����ݵڶ���������ֱ�Ӽ���ú���λ�ý�images���������ȫ������ȡ����
				strMsg = strData.mid(posImages + QString("images").length());  
			}
			else if (cMsgType == '2') {  // �ļ���Ϣ
				msgType = 2;  // ��ȡ��Ϣ����

				// ����һ��bytes�Ŀ���Լ�λ��
				int bytesWidth = QString("bytes").length();  // ��bytes�ַ�������һ��QStringȻ������䳤�ȵĺ���
				int posBytes = strData.indexOf("bytes");     // �ҵ�bytes��λ��
				int posData_begin = strData.indexOf("data_begin");  // �ҵ�data_begin��λ��

				// ��ȡ�ļ����� -- ���ݰ����ļ�����bytes��data_begin�ַ���֮��
				// ����ڶ�������������ͨ��data_begin���±��ȥbytesβ�����±���ȷ���ļ����ƵĿ�ȣ�Ҳ���ǽ�ȡ�ĸ���
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - (posBytes + bytesWidth));
				gFileName = fileName;  // ��ֵ�ļ�����

				// �ļ����� -- �ȼ�������λ��
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").size();  // ��ȡ�����ݵ�λ��
				strMsg = strData.mid(posData);  // ��ȡ�ļ����ݣ�����û�еڶ������������ǽ�ȡ�ӿ�ʼλ�õ��ַ�����β����������
				gFileData = strMsg;   // ��ֵ�ļ�����

				/*��ȡһ�·����ߵ�����,��Ϊ�����յ��ļ���ʱ��������Ҫ����һ�£��յ�˭˭��
				  �ļ������Ƿ�Ҫ��������ļ�*/
				// ����employeeID��ȡ����������
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery query(QString("SELECT employee_name FROM table_employees WHERE employeeID = %1").arg(employeeID));
				query.exec();  // ִ��sql���

				if (query.first()) {  // �Ƿ��ѯ����� -- ��������ָ���һ�����ݺ��Ƿ��ѯ�ɹ����������
					sender = query.value(0).toString();
				}

				// �����ļ��ĺ�������
				ReceieveFile *recvFile = new ReceieveFile(this);
				connect(recvFile, &ReceieveFile::refuseFile, [this]() {
					return; });    // ������ǲ���������ļ���Ҳ���Ƿ����˾ܾ����ܵ��źţ���ʲô����Ҳ������ֱ���˳�

				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ����?");

				recvFile->setMsg(msgLabel);   // �Խ����ļ�������ʾ��Ϣ
				recvFile->show();              // ��ʾ���յ��ļ�
			}
		}
		else {  // ����
			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth,employeeWidth);   // ��ȡ���շ���id
			strWindowID = strSendEmployeeID;   // �����ߵĴ��ھ��Ƿ����ߵ�ID��

			// �ж��Ƿ��Ƿ����ҵ���Ϣ�����Ƿ����ҵľͲ�������
			if (strReceiveEmployeeID != gLoginEmployeeID) {
				return;   // ������Ƿ����ҵ���Ϣ��ô�Ҿ�ֱ�ӷ��ز����κεĴ�����
			}

			// ��ȡ��Ϣ����
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1') { // �ı���Ϣ
				msgType = 1;

				// �ı���Ϣ�ĳ���
				msgLength = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
				         + msgTypeWidth,msgLengthWidth).toInt();  // ��ȡ���������ַ���������Ҫת��Ϊint����

				// ��ȡ�ı���Ϣ
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth
					+ msgTypeWidth + msgLengthWidth, msgLength);
			}
			else if (cMsgType == '0') { // ������Ϣ
				msgType = 0;
				int posImages = strData.indexOf("images");  // ���ض�Ӧ�ַ�����ʼ��λ��
				int imagesWidth = QString("images").length();

				// ��ȡ����
				strMsg = strData.mid(posImages + imagesWidth);  // ��ȡ��ָ��λ�ÿ�ʼ�����ȫ����Ϣ
			}
			else if (cMsgType == '2') { // �ļ���Ϣ
				msgType = 2;
				/*��ȡ��־���ַ�����λ�úͿ��*/
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidtg = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				// �����ļ����� -- �ڶ������������ݰ����ļ����ƵĴ�С
				QString fileName = strData.mid(bytesWidth + posBytes, posData_begin - bytesWidth - posBytes);
				gFileName = fileName;

				// �ļ�����  --  data_begin֮��Ķ���������
				strMsg = strData.mid(posData_begin);
				gFileData = strMsg;

				// ��ȡ����������
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery query(QString("SELECT employee_name FROM table_employees WHERE employeeID = %1").arg(employeeID));
				query.exec();  // ִ��sql���

				if (query.first()) {  // �Ƿ��ѯ����� -- ��������ָ���һ�����ݺ��Ƿ��ѯ�ɹ����������
					sender = query.value(0).toString();
				}

				// �����ļ��ĺ�������
				ReceieveFile *recvFile = new ReceieveFile(this);
				connect(recvFile, &ReceieveFile::refuseFile, [this]() {
					return; });    // ������ǲ���������ļ���Ҳ���Ƿ����˾ܾ����ܵ��źţ���ʲô����Ҳ������ֱ���˳�

				QString msgLabel = QString::fromLocal8Bit("�յ�����") + sender
					+ QString::fromLocal8Bit("�������ļ����Ƿ����?");

				recvFile->setMsg(msgLabel);   // �Խ����ļ�������ʾ��Ϣ
				recvFile->show();              // ��ʾ���յ��ļ�
			}
		}

		// �����
		/*
		    ���ݽ�������֮�����Ǿ���Ҫ������������������ӵ���ҳ��ȥ������������ʾ
			��ȥ�յ�������

			������Ϣ��ʱ��������Ǵ򿪵���һ��Ⱥ�����յ���Ϣ��������һ��Ⱥ����ô����
			��ʱ����Խ����յ�����Ϣ��Ⱥ����Ϊ�����

			�˴���ҵ��򵥻������������Ϣ��Ⱥ�Ѿ��򿪣����Ǿͽ�������Ϊ����ڣ�Ȼ��
			����Ϣ��ʾ��ҳ���У���Ȼ����������ǵ�ǰ��ʾ�Ĵ��ڣ��Ǿͽ�������Ϊ�����

			�������û�д򿪣���ô���ǾͲ����κβ���
		*/

        // �����촰������Ϊ��Ĵ���
		// ��ȡ���� -- ʹ�ô��ڹ�����ķ���ֱ�Ӳ�ѯ -- ���ݴ���id��ѯ�Ƿ������Ѿ����������
		// ����һ�����촰�ڵĵ�ַ
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget) {   // ������ô������Ϊ�����
			this->setCurrentWidget(widget);     // �������촰�ڵĵ�ַ����Ϊ��������
			
		    // ������б�ͻ���ڽ���ͬ�� -- ����֮�䶨����һ��ӳ�䣬ÿһ�����촰�ڶ���Ӧ��һ�������б�
			// ���Կ��Ը��ݴ��ڵĵ�ַ��ȡ������б����
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);     // key(ֵ)����ӳ��ֵ����ӳ���
			item->setSelected(true);   // ���õ�ǰ�ѡ��
		}
		else { // ���͵����촰��û�д򿪣���ôֱ�ӷ���
			return;
		}

		int senderID = strSendEmployeeID.toInt();  // ����ת��Ϊ����
		// ����������������ʾ����ҳ��ȥ -- ��Ҫ�������������ʵ����һ����
		// ����������Ͳ�����2��Ҳ���Ƿ��͵Ĳ����ļ���ʱ�����ǵ���������ݣ���Ϊ����
		// �ļ�������Ҫ���⴦������Ҫ������Ϣ����ʾ
		// �ļ���Ϣ����������
		if (msgType != 2) {
			handleReceivedMsg(senderID, msgType, strMsg);
		}
	}
}

void TalkWindowShell::onEmotionBtnClicked(bool checked)
{
	// ���鰴ť�������Ҫ�漰�����ǵı��鴰���Ƿ�ɼ�
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());  // ���ص�ǰ�Ŀɼ�״̬��
																 // ����ȡ���������ǰ�ɼ����������Ϊ���ɼ�����һ�����Ҳ����

	// �������������
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));       // ����ǰ�ؼ������λ��ת��Ϊ��Ļ�ľ���λ��

	// ����ƫ��
	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);

	// ����λ��
	m_emotionWindow->move(emotionPoint);    // �����ƶ�����Ӧ��λ��
}

// ����б�ѡ���˾���Ĵ��ڣ���ô�ͽ��д��ڵ�ת��
void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item) {
	QWidget* talkWindowWidget = m_talkwindowItemMap.find(item).value();   // ȡ�����Ӧ�Ĵ���    
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}
