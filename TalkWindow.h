#pragma once

#include <QWidget> 
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"
#include <QTreeWidgetItem>


class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	// �������ݿ�֮���캯�������˸ı�
	TalkWindow(QWidget *parent, const QString& uid/*, GroupType groupType*/);
	~TalkWindow();

	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId()const;

private slots:
	void onFileOpenBtnClicked(bool);
	void onSendBtnClicked(bool);
	// ˫��Ⱥ��Ա���Ȼ��ͽ��뵽���Ӧ˫����Ⱥ��Ա��������
	void onItemDoubleClicked(QTreeWidgetItem*, int);

private:
	void initControl();
	/* ������ݿ�֮����� */
	void initGroupTalkStatus();  // ��ʼ��Ⱥ��״̬����m_isGroupTalk����Ϊtrue����false
	/* ������ݿ�֮����� */
	int getCompDepId();         // ��ȡ��˾���ŵ�qq�Ż���id��

	/* ������ݿ�֮�����ǲ���Ҫ��ʼ������ô��ϸ��ֻ��Ҫ��һ����ʼ��Ⱥ�ĵķ���������
	   ��Ϊ��ͬ���û����ĸ�Ⱥ�ľͳ�ʼ���Ǹ�
	void initCompanyTalk();   // ��ʼ����˾Ⱥ����
	void initPersonelTalk();  // ��ʼ�����²�����
	void initMarketTalk();    // ��ʼ���г�������
	void initDevelopTalk();   // ��ʼ���з�������
	*/
	void initTalkWindow();    // ��ʼ��Ⱥ�ģ�ʹ�����ݿ�֮�����
	void initPtoPTalk();      // ��ʼ������
	// void addPeopleInfo(QTreeWidgetItem* pRootGroupItem);   // ������ĸ����������
	// ������ݿ�����޸�
	void addPeopleInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);

private:
	Ui::TalkWindow ui;
	// ���浱ǰ���촰�ڵ�ID
	QString m_talkId;
	// ���浱ǰ���촰�ڷ��������
	// GroupType m_groupType;  -- �������ݿ�֮������Ͳ���Ҫ��
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;   // ���з�����ϵ������
	bool m_isGroupTalk;   // �Ƿ�ΪȺ�� -- ���ݿ�����֮�����
	friend class TalkWindowShell;   // ������Ԫ����Ϊ������Ҫ��TalkWindowShell�з���TalkWindow��˽�г�Աui���������ó���Ԫ����TalkWindowShell�����ܵ�����׷�ӵ���ǰ������ʱʹ��
};
