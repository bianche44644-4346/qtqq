//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>
#include "UserLogin.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//CCMainWindow w;
	//w.show();

	// 显示登录界面
	UserLogin* userLogin = new UserLogin;
	userLogin->show();

	// 此函数为true，表示最后一个窗体关闭的时候应用程序就会退出
	a.setQuitOnLastWindowClosed(false);
	return a.exec();
}
