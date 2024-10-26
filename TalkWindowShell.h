#pragma once

#include <QWidget>
#include <QMap>
#include <QTcpSocket>
#include <QUdpSocket>
#include "ui_TalkWindowShell.h"
#include "BasicWindow.h"
#include "EmotionWindow.h"

/*���촰��*/
class TalkWindowItem;
class QListWidgetItem;
class TalkWindow;

enum GroupType {
	COMPANY = 0,     // ��˾Ⱥ
	PERSONELGROUP,   // ���²�
	DEVELOPMENTGROUP,// �з���
	MARKETGROUP,     // �г���
	PTOP,            // һ���˺�һ���˽�������
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();

private:
	void initControl();   // ��ʼ������
	void initTcpSocket();  // ��ʼ��tcp�׽���
	void initUdpSocket();  // ��ʼ��udp�׽���
	void getEmployeesID(QStringList& employeeList);   // ��ȡ����Ա��qq�ţ����㴴��js�Ľӿ�ʹ��

	 // ����js�ļ� -- �����Ƿ񳣼��ɹ�  ���� --  ��һ��������Ϊ������js�ļ�����Ҫ�������ݿ��д��ڵ�qqʹ���ߣ���ȷ�������ߣ�external������ǵ�qq�ţ�����������Ҫ����һ��������Ա������Ӧ��Ϣ
	bool createJSFile(QStringList &employeeList);  
	// ����:              ������id      ��Ϣ����     ����
	void handleReceivedMsg(int senderID,int msgType, QString strMsg);  // �����յ�����Ϣ��ʾ����ҳ��

public:
	// ����µ����촰��
	//  �Զ�����      ���촰��                 ���촰�ڵ���һ��                ��Ϊ��ͬ�����촰����ʾ�ǲ�һ����(Ⱥ����͵�������)
	//  ʹ�����ݿ�֮����������ı�
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/);
	// ���õ�ǰ���촰��
	void setCurrentWidget(QWidget* widget);     // ���õ�ǰ�Ĳ���

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;

public slots:
	// ������鰴ť�������
	void onEmotionBtnClicked(bool);
	// ���·���tcp��Ϣ�������ǵ�����Ͱ�ť��ʱ�򣬷����ź���Ӧ�������������ʹ�ò�������һЩ
	// ���͵���Ϣ�����ݣ�Ȼ���ٲۺ����н������ݷ���(�ͻ��˷���)
	//                        ���͵�����        ��������      �ļ�
	void updateSendTcpMessage(QString& strData, int &msgType, QString fileName = "");

private slots:
	// ����б�����ִ�еĲۺ���
	void onTalkWindowItemClicked(QListWidgetItem* item);
	// ����ı���(ѡ�еı���)����һ��������һ�����
	void onEmotionItemClicked(int emotionNum);
	// �ڰ�udp�ɹ�֮�󣬷���һ��׼����ȡ���źţ�Ȼ�����������������ݽ��д���
	void processPendingData();  // ����udp�㲥�յ�������
private:
	// �����ǵ�������б�����ĸ����ڽ��������ʱ����Ҫ֪����Ӧ���ڵĵ�ַ,ʹ�ñ��������㱣��
	QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;   // �򿪵����촰��
	EmotionWindow* m_emotionWindow;                         // ���鴰��

private:
	QTcpSocket *m_tcpClientSocket;    // �ͻ���tcp
	QUdpSocket *m_udpReceive;         // udp���ն�

private:
	Ui::TalkWindowClass ui;
};
