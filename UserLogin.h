#pragma once

#include <QWidget>
#include "ui_UserLogin.h"     // ���ͷ�ļ����ܻ���ִ�����Ϊ���Ǳ���֮��ų��ֵģ�
// ���Ե�һ��ʹ����Ҫ�ȱ���һ�Σ�֮��ͻ�������
#include "BasicWindow.h"

// ��UserLogin�Ļ�������Ϊ���ǵ�BasicWindow
class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();

private slots:
	void onLoginBtnClicked();
	bool connectMysql();   // �������ݿ�ķ���
	// �Ӳ�����Ϊ�˻�ȡ�û�������˺Ż���id
	bool veryfyAccount(bool& isAccountLogin,QString& strAccount);  // ��֤�˺ŵķ���

private:
	void initControl();   // ��ʼ���ؼ��ķ���

private:
	Ui::UserLogin ui;
};
