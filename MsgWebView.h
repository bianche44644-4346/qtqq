#pragma once

#include <QWebEngineView>
#include <QDomNode>

class MsgHtmlObj : public QObject {
	Q_OBJECT

		// ������̬���ԣ�������ҳ��Ϣ����ʾ��һ�����Լ����͵ģ�һ���ǶԷ���������
		//                              �������Ա
		// ��������Ϣ  L��ʾ���촰�ڵ���ߣ����˷���
		Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)
		// �ҷ�����Ϣ  R��ʾ���촰�ڵ��ұߣ��Լ�����
		Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)

public:
	//                         ����Ϣ�����˵�ͷ��·��
	MsgHtmlObj(QObject* parent, QString msgLPicPath = ""); // ����һ��������������෢��Ϣ������ͷ��ĵ�ַ����ʱ�򹹽���ҳ�����ʱ�������ʾ��Ӧ��ͷ����Ϣ


Q_SIGNALS:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();    // ��ʼ��������ҳ
	// ��ʼ����ҳ���ڷ���Ϣ������Ϣ��һ������Ϊ�ҷ����������ʾ�����˷������Ҳ���ʾ
	QString getMsgTmpHtml(const QString& code);     // ���䴫��Ҫ��ʼ������ҳ()
private:
	QString m_msgLHtmlTmpl;  // ���˷�������Ϣ
	QString m_msgRHtmlTmpl;  // �Լ����͵���Ϣ
	QString m_msgLPicPath;   // ��෢����Ϣ��ͷ��·��
};

// ��ҳ
class MsgWebPage :public QWebEnginePage {
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent) {}
	~MsgWebPage() {}

	// ��д��ҳ��ԭ����Ϊ���յ���ҳ������ֻ������qrc�Ĳ��У������������ǲ��е�
	// ͨ����д������ʵ������
protected:
	// �������ֻ��Ҫ��д��һ����������ҪΪ�˶������������
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)override;
};

// QWebEngineView��ͼ����ϵ��ʾ��ҳ����
class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();
    
	// ������Ҫ�ж��Ƿ�ͨ�������ʲô���ݣ��������ǻ���Ҫ����һ������
	// ͨ���ı���������ֻ�����ַ�ʽexternal0����external_id�ŵȡ�
	// ������������Լ�����Ϣ�Ļ�����ô��ʹ��Ĭ�Ͼ���external0�����ǵĻ��ʹ���һ��id�ţ�֮�����һ��ƴ��
	// ���������оͿ��Ը��ݴ���Ĳ����жϵ������Լ�������Ϣ�����Ǳ��˷�����Ϣ��
	void appendMsg(const QString& html,QString strObj = "0");   // Ĭ��Ϊ0���ڶ�������Ҫ����Ա����qq�ţ���������Ļ�Ĭ�Ͼ���0
	// void appendMsg(const QString& html);  // ׷����Ϣ������Ĳ����ַ�����html��ʽ��������ʾ��ҳ

private:
	QList<QStringList> parseHtml(const QString& html);  // ����Html�������ַ�����������������Ҫ������html�ַ���
	// Qt������DOM�ڵ�(���ԣ�˵�����ı���)������ʹ��QDomNode���в���(��ʾ)
	QList<QStringList> parseDocNode(const QDomNode& node);   // �����ڵ㣬ʹ��QDomNub���Խ���Qt�����еĽڵ㣬����˵�������ԣ�Ԫ�أ��ı���

	// ��ҳ��ʾ��Ҫһ������Qt��htmlͨ����Ҫʹ��QWebChannel����Ҫ��ͨ���й���������ͨ�ţ����ǽ��й���

Q_SIGNALS:
	// ����ҳ������Ϣ��ʱ��ͷ����źţ�����tcp�ĺ����������ŵ����ݷ��Ͳ���
	void signalSendMsg(QString& strData, int &msgType, QString sFile = "");

private:
	MsgHtmlObj* m_msgHtmlObj;
	QWebChannel* m_channel;     // ����ͨ��
};
