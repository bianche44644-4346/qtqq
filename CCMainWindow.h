#pragma once
/*
*  ��ȡԲͼƬ��ʱ������Ӧ�ý�׼����ͼƬ��͸߶���ͬ(�����λ��߽ӽ�������)�������ȡ
   ����ԲͼƬ���ÿ���С�Ļ�û��һЩ̫������ơ�
*/
#include <QtWidgets/QWidget>
#include "ui_CCMainWindow.h"
#include "BasicWindow.h"
#include <QMouseEvent>

class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	// ���ݿ����������������һ�����˺ţ���һ�����ж��Ƿ����˺ŵ�¼(��Ϊ���������ֵ�¼��ʽ
	// false ��ʾaccount��qq�ţ�true��ôaccount���˺�)
	CCMainWindow(QString account,bool isAccountLogin,QWidget *parent = Q_NULLPTR);
	virtual ~CCMainWindow();

private:
	void resizeEvent(QResizeEvent* event);
	// ��д�¼�������
	bool eventFilter(QObject* obj, QEvent* event);
	void mousePressEvent(QMouseEvent* event);
	

private slots:
	void onAppIconClicked();
	// �����
	void onItemClicked(QTreeWidgetItem* item, int column);
	// ���չ��
	void onItemExpanded(QTreeWidgetItem* item);
	// �������
	void onItemCollapsed(QTreeWidgetItem* item);
	// ˫������
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);

public:
	// �����û��ȼ�ͼƬ
	void setLevelPixmap(int level);
	// ����ͷ��
	void setHeadPixmap(const QString& headPath); // ����һ��·��
	// ��������״̬  -- ����һ��״̬��·��
	void setStatusMenuIcon(const QString& statusPath);
	// ���Ӧ�ò���  --  ����ֵ��һ������
	// ����:  app��·����app������
	QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);
	// ��ʼ����ʱ�������ڼ�¼�ȼ�����
	void initTimer();
	// ��ʼ����ϵ��
	void initContactTree();

private:
	// ��ʼ���ؼ�
	void initControl();
	// ��ʼ���û���
	void setUserName(const QString& username);
	// �������������ʽ
	void updateSearchStyle();
	// ��ӹ�˾���� -- ��Ҫָ�����ĸ������������ �ڶ���������ʹ�����ݿ�֮��������޸ģ�ֱ��ʹ���˲��ŵ�id
	// void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, const QString& sDeps);
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepId);

	// �����ݿ��ȡ��¼�û���ʹ�õ�ͷ���·��
	QString getHeadPicturePath();

private:
	Ui::CCMainWindowClass ui;
	// ��ʾ��һ�����촰�ڶ�Ӧ������һ������

	// ��ʹ�����ݿ�֮����������Ͳ���Ҫ��
	// QMap<QTreeWidgetItem*, QString> m_groupMap;  // ���з���ķ�����

	// �����û��Ƿ�Ϊ�˺ŵ�¼���ж��Լ������û�������˺�
	bool m_isAccountLogin;
	QString m_account;   // ��¼���˺Ż���id��
};
