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

extern QString gstrLoginHeadPath;  // ����CCMainWindow�еļ�¼��¼��ͷ��·����ȫ�ֱ���������ʾ��ҳ�Ի���ʱ��Ҫʹ��

// ����������촰�ڹ����ʱ����Żṹ��
MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent),m_channel(new QWebChannel(this))
{  // ��������ں����н����˸Ķ���ԭ��������ע��
	MsgWebPage* page = new MsgWebPage(this);  // ����һ����ҳ
	setPage(page);  // ���͹������ҳ����Ϊ��ǰ����ҳ

	// QWebChannel* channel = new QWebChannel(this);  // ����һ��ͨ��  -- ����ɾ����ԭ������Ϊ���������Ƕ���������������
	m_msgHtmlObj = new MsgHtmlObj(this);

	// ע��һ������
	//channel->registerObject("external0", m_msgHtmlObj);
	m_channel->registerObject("external0", m_msgHtmlObj);
	// ���õ�ǰ��ҳ��ͨ��
	// this->page()->setWebChannel(channel);  // ͨ���������ǵ�ǰ��channel

	/*�����ź���ۣ�����ҳ������Ϣ��ʱ�򣬷����ź����ǵ�����Ӧ��tcp�������з���*/
	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMessage);

	/*����Ҫ���д������������ĸ�Ⱥ���͹�������Ϣ��ֻ��Ⱥ�е��˿����յ�*/
	// ��ǰ���ڹ��������촰�ڵ�id
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();
	
	QSqlQueryModel queryEmployModel;
	QString strEmployeeID, strPicturePath;  // ����Ա��ID��ͷ��·��
	QString strExternal;
	bool isGroupTalk = false;   // ��¼�Ƿ���Ⱥ��

	// ��ȡ��˾Ⱥ��id��
	// ��������ַ����д������ģ���ʹ��д���ģ�����Ҳ��Ҫƴ�ӣ���Ϊ��Ҫ���б���ת��(ʹ��from�ĸ����������ߺ�stringLiteral��)����Ȼ���������
	queryEmployModel.setQuery(QString("SELECT departmentID FROM table_department WHERE department_name = '%1'").arg(QStringLiteral("��˾Ⱥ")));
	QModelIndex companyIndex = queryEmployModel.index(0, 0);  // ʹ��������ȡģ���е�����
	QString strCompanyID = queryEmployModel.data(companyIndex).toString();

	if (strTalkId == strCompanyID) {  // ��˾Ⱥ�� -- ��Ա���������е�״̬Ϊ1��Ա����qq�Ŷ��ó�������ȡһ��Ա��qq���Լ�ͷ��
		queryEmployModel.setQuery("SELECT employeeID,picture FROM table_employees WHERE status = 1");
		isGroupTalk = true;
	}
	else {  // ������ǹ�˾Ⱥ�� -- ���ж�һ������ͨȺ�Ļ��ǵ�������
		// ���������ͨȺ�ĺ͵�������?  -- ����ͨ���ж϶�Ӧid�Ŀ��������(Ⱥ����4λ��������5λ)
		if (strTalkId.length() == 4) {  // ����Ⱥ��
			// ����Ĳ�ѯ����ڹ�˾Ⱥ�Ļ����ϼ������������������Ϊ���ǻ�ȡ���û�id�������Ƕ�ӦȺ��Ա��
			queryEmployModel.setQuery(QString("SELECT employeeID,picture FROM table_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId));
			isGroupTalk = true;
		}
		else {  // ��������
			// ����sql��� -- �����Ѿ�ֱ����Ӧ�����id��(��Ϊ�������ݽ�����)������ֻ��Ҫ���������˵�id��ѯ��Ӧ��ͷ�����
			queryEmployModel.setQuery(QString("SELECT picture FROM table_employees WHERE status = 1 AND employeeId = %1").arg(strTalkId));

			// ����ģ��������ȡ��ѯ��������
			QModelIndex index = queryEmployModel.index(0, 0);
			strPicturePath = queryEmployModel.data(index).toString();

			strExternal = "external_" + strTalkId;
			// ������ҳ����
			MsgHtmlObj *msjHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msjHtmlObj);   // ��ͨ����ע��
		}
	}

	// ����ǵ��ľͲ���Ҫ�ˣ���Ϊ�����Ѿ�д���ˣ�Ⱥ�ıȽ϶࣬����д������
	if (isGroupTalk) {  // �����Ⱥ�ģ���ô�ͽ��в��������bool���������ж���Ⱥ�ĵ�ʱ������˸�ֵ
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployModel.rowCount();
		/*
		  ��ѯ������ģ�����������Ƕ�ӦȺ���е�Ա�������Ƕ���ЩԱ���������д���
		*/
		for (int i = 0; i < rows; i++) {
			employeeModelIndex = queryEmployModel.index(i, 0);  // Ա���ŵ��������ǵ�i�е�0��
			pictureModelIndex = queryEmployModel.index(i, 1);

			strEmployeeID = queryEmployModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			// ������ҳ����
			MsgHtmlObj *msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}

	// ���õ�ǰ��ҳ��ͨ��
	this->page()->setWebChannel(m_channel);  // ͨ���������ǵ�ǰ��channel
	// ��ʼ��һ������Ϣ��ҳҳ�� ������ҳ��Ϣ
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));  // ʹ�ö�Ӧ��html�ļ����г�ʼ����ҳ
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString & html, QString strObj)
{

	QJsonObject msgObj;  // ���ڷ�����Ϣ��
	// ׷����Ϣ
	QString qsMsg;
	// ����html
	const QList<QStringList> msgList = parseHtml(html);

	int msgType = 1;      // ��Ϣ����: 0�Ǳ��� 1���ı� 2���ļ���Ĭ��д1
	QString strData;      // ������͵ı�����55�ţ���ô�Ͳ�ȫ��λΪ055
	int imagesNum = 0;    // ��������� Ĭ��Ϊ0 -- Ϊ�˷����¼һ���Է��Ͷ����������
	bool isImageMsg = false;  // ��¼�Ƿ���ͼƬ���ݣ�Ĭ����false���źŷ��ʹ��ݵ����ݷ�������

	// �������������н�������html����
	for (int i = 0; i < msgList.size(); i++) {
		if (msgList.at(i).at(0) == "img") {   // ���������������ж�ӦԪ�صĵ�һ��Ԫ���Ƿ���img
			// ����ͼƬ��·��
			QString imagePath = msgList.at(i).at(1);  // 0��img����ô1����srcl�������д��html˳���й�ϵ
			// ·�������� "qrc:/MainWindow/.../77.png" ���� ":/MainWindow/.../77.png"
			// ����������Ҫ�ж���·���Ƿ���qrc
			QPixmap pixmap;  // ��������ͼƬ
			 
			// ��ȡ�����·��(λ��)  qrc:/��ʾ��Դ�ļ���·��
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();  // λ�þ��ǣ�����·���ĳ���
			isImageMsg = true;                // �������Ƿ��͵���ͼƬ��Ϣ����������Ϊtrue

			// ��������ƣ�ʹ��mid������·���н��н�ȡ
			QString strEmotionName = imagePath.mid(pos); // ��ȡ���ľ���xxx.png
			strEmotionName.replace(".png", "");    // ������Ҫ���Ǳ�������ƣ�Ҳ����xxx��ʾ�����ݣ��������ǽ������.pngʹ�ÿ��ַ��������滻
			
			// ���ݱ������Ƶĳ��Ƚ������ñ������ݣ�����3Ϊ����3λ
			// ��:  23.png ���������Ϊ23������3λ��Ϊ023
			int emotionNameLength = strEmotionName.length();   // ��ȡ�������ֵĳ��ȣ����ݳ�������Ҫ�ж��費��Ҫ��ȫ����λ
			if (emotionNameLength == 1)   // 1λ���Ȳ�2��0
				strData = strData + "00" + strEmotionName;
			else if(emotionNameLength == 2)
				strData = strData + "0" + strEmotionName;
			else   // ԭ������3λ�����Բ���Ҫ����ƴ��
				strData = strData + strEmotionName;
		    
			msgType = 0;   // ��Ϣ����Ϊ������Ϣ
			imagesNum++;   // û���뵽һ��if���ͽ�����һ�����飬��ô����������ͽ���++��Ϊ�˷��㷢�Ͷ����������


			if (imagePath.left(3) == "qrc") {  // �ж�����ߵ������ַ��Ƿ���qrc������ǽ�����Ӧ�Ĳ���
				// ��qrc�Ļ���Ҫ��qrcȥ��
				// mid()�����������ȡ�Ŀ�ʼλ�ã�����ָ����ȡ�ĳ��ȣ�Ҳ���Բ�ָ�������ǽ�����Ķ���ȡ
				// ��ȡ��֮����м���
				pixmap.load(imagePath.mid(3));  // ��Ϊqrc��ǰ3���ַ�����������Ӧ�ô�3��λ�ÿ�ʼ��ȡ
			}
			else {
				// û��qrc��ֱ�Ӽ��أ����ý�ȡ
				pixmap.load(imagePath);
			}

			
			// ͼƬ������֮������һ��ͼƬ��·�������ص���ҳ��Ϣ�У���α�ʾ
			// %1 ƴ��·�� %2 ƴ�ӿ�� %3 ƴ�Ӹ߶�  ""�ڲ�д""��Ҫ��ת��
			QString qimgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>").arg(imagePath)
				.arg(pixmap.width()).arg(pixmap.height());  // ����ͼƬhtml��ʽ�ı����

			// ���֮����ӵ���Ϣ��ȥ
			qsMsg += qimgPath;
		}
		else if (msgList.at(i).at(0) == "text") {  // ������ı�
			qsMsg += msgList.at(i).at(1);      // 0����text��ǩ��1����������
			strData = qsMsg;     // ֱ�ӽ����͵�������Ϣ��ֵ���˴������÷�����������Ϊ���͵�Ĭ�����(���뺯���ͻ�����)����1�������ı�
		}
	}

	msgObj.insert("MSG", qsMsg);   // ����Ļ��Ǽ���ֵ
	// json����ת��Ϊjson�ĵ�
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);   //��Jsonת��Ϊ�ַ����洢����
	// �ж϶����ǲ���0��Ҳ�����ǲ����Լ����͵���Ϣ
	if (strObj == "0") {  // ����Ϣ
		// ��ʱ������js�ű���ʱ����ں����һ��0���������еľ����Լ�������Ϣ������
		// ��Ϊexternal0�����Լ�������Ϣ��ͨ��
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		// �����źŵ�ʱ���ж��Ƿ��Ƿ��͵���ͼƬ������Ǿͽ�����Ӧ�Ĵ���
		if (isImageMsg) {
			// ��ʱ������ʱ��͸������ַ�ʽ���н������Ϊ�������ݵ�ʱ������Ҫ�����źŸ��ۺ���
			// �ۺ�������ִ��ʵ����һЩtcp�Ĳ�����ʽ��
			strData = QString::number(imagesNum) + "images" + strData;
		}

		// �����źŸ���tcp�Ĵ����������Ƿ��͵������Լ�����
		emit signalSendMsg(strData, msgType); 
	}
	else {  // ������Ϣ��ֻ��Ҫ׷�ӣ�����Ҫ�����ź�
		// ���������һ��ƴ�ӵ�ʱ��ֱ�ӿ���ʹ�ô������Ķ����������Ϊ������ʱ���Ѿ�
		// ��������Ӧ���жϣ����������֧֮���Ѿ���strObj����Ĳ���0,Ҳ����˵����
		// �Լ�������Ϣ�����ǽ����ĸ��û�������������
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}

//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));   // ��һ��js     

	// �����ź�  -- ָ���ղ����õķ��͵������Լ����͵����ݵ�����(��ʱ�������ļ�)
	// �����źŸ���tcp�Ĵ����������Ƿ��͵������Լ�����
    //	emit signalSendMsg(strData, msgType);
}

// ����Html
QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	// ����������һ��Html���ַ�����������Ҫ��DOM�ĵ���ȡ����
	QDomDocument doc;
	doc.setContent(html);    // ���ýڵ��ĵ������ݣ�ֱ�ӵ��ú����������html�ַ��������ȥ�Ϳ���������
	// ��Ҫ�����ڵ������body�ڵ�
	const QDomElement& root = doc.documentElement();    // �ڵ�Ԫ��
	// ��ȡbody�Ľڵ�
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);   // �����ڵ�󷵻�
}

QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	// �����ڵ� -- ������������һ���ַ���������
	QList<QStringList> attribute;
	// ��ȡ�������ڵ���ӽڵ�
	const QDomNodeList& list = node.childNodes();   // ���������ӽڵ�

	// ������ȡ���Ľڵ�
	for (int i = 0; i < list.size(); i++) {
		const QDomNode& node = list.at(i);

		// �Ե�ǰ�������Ľڵ���н���
		if (node.isElement()) {   // ����õ�����Ԫ��
			// ��ô��ת��Ԫ��
			const QDomElement& element = node.toElement();   // ֱ��ת��ΪԪ��
			// �ж�Ԫ�صı�ǩ���ƣ��ǲ���ͼƬ�����ı�֮���
			if (element.tagName() == "img") {  // �����ͼƬ
				// ��ȡͼƬ��ֵ����ʾ��������һ��
				QStringList attributeList;
				attributeList << "img" << element.attribute("src");  // ��ȡimg��ǩ��src���Ե�ֵ��Ҳ����ͼƬ��·��
				attribute << attributeList;    // �������Ľ����д��attribute
			}

			if (element.tagName() == "span") { // ������ı�
				QStringList attributeList;
				attributeList << "text" << element.text();   // ��ȡ�ı���ǩ�е��ı�
				attribute << attributeList;
			}

			if (node.hasChildNodes()) {  // ����仹���ӽڵ㣬��ô�ͼ�������
				attribute << parseDocNode(node);
			}
		}
		return attribute;    // ��������
	}

	return QList<QStringList>();
}

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath) :QObject(parent),
    m_msgLPicPath(msgLPicPath)    // ����ഫ���ݵ�ͷ��·��
{
	initHtmlTmpl();   // �����е��ó�ʼ����ҳ
}

void MsgHtmlObj::initHtmlTmpl()
{
	// ��ʼ����ҳ -- ��ȡhtml�ļ�����Ϣ�������ҳ�ʼ������ĳ�Ա����
	m_msgLHtmlTmpl = getMsgTmpHtml("msgleftTmpl");  // �����ļ���׼���õ�html�ļ���
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);    // ����ߵ���Ϣ��ͷ��·���滻Ϊ���Ǳ�������ͷ���·��

	m_msgRHtmlTmpl = getMsgTmpHtml("msgrightTmpl");
	/* �ұߵ�ͷ����ǵ�¼�ߵ�ͷ������ֱ�ӽ�%1ռλ���滻�����ǵĵ�½��ͷ��·�� */
	// ͨ��CCMainWindow�е�ȫ�ֱ����н��и�ֵ�������¼��ͷ�񣬴�������ֱ�ӻ�ȡ��½��ͷ��
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgTmpHtml(const QString& code)
{
	// ��ȡhtml����Ϣ�����Ƕ�ȡ -- ֱ�ӽ������ļ��е���Ϣȫ����ȡ������Ȼ�󷵻�
	// ʹ��:/������Դ·����ʱ�򣬴���Դ������ӵ�qrc��Դ�����ļ��У�����Ͳ�Ҫ����ʹ��
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QFile::ReadOnly);    // ֻ����
	QString strData;
	if (file.isOpen()) {           // �ж��Ƿ��Ǵ򿪵�
		strData = QLatin1String(file.readAll());   // �۸��ļ��е�����ȫ����ȡ������������ַ���
	}
	else {
		// ��Ϊ������QObject���������ģ����Ǵ�ʲô���岿�����������ģ��������ǲ���ʹ�õ�ǰ����this����Ϊ�丸���壬Ӧ���ǲ����ӻ���
		QMessageBox::information(nullptr, "Tips", "Failed to init html");   // ��ӡ�쳣��Ϣ
		return QString("");    // ���ؿ�
	}
	file.close();

	return strData;
}

// ����ҳ������������ƣ����������������qrc:html�ĸ�ʽ������ַ������ʵ��һ��·��
// QUrl���������Ա���http�ȳ��õ���ַ�������Դ��·��(��Դ·����)
bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	if (url.scheme() == QString("qrc"))  // �ж�url�����Ƿ���qrc
	{
		// ��qrc���ͽ��յ����󣬲��򿪵���Ӧ����ҳ
		return true;   // �������������qrc������
	}
	return false;
}
