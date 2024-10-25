#pragma once

#include <QWidget>
#include "ui_UserLogin.h"     // 这个头文件可能会出现错误，因为其是编译之后才出现的，
// 所以第一此使用需要先编译一次，之后就会生成了
#include "BasicWindow.h"

// 将UserLogin的基类设置为我们的BasicWindow
class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();

private slots:
	void onLoginBtnClicked();
	bool connectMysql();   // 连接数据库的方法
	// 加参数是为了获取用户输入的账号或者id
	bool veryfyAccount(bool& isAccountLogin,QString& strAccount);  // 验证账号的方法

private:
	void initControl();   // 初始化控件的方法

private:
	Ui::UserLogin ui;
};
