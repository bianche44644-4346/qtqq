//#include "CCMainWindow.h"
#include <QtWidgets/QApplication>
#include "UserLogin.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//CCMainWindow w;
	//w.show();

	// ��ʾ��¼����
	UserLogin* userLogin = new UserLogin;
	userLogin->show();

	// �˺���Ϊtrue����ʾ���һ������رյ�ʱ��Ӧ�ó���ͻ��˳�
	a.setQuitOnLastWindowClosed(false);
	return a.exec();
}
