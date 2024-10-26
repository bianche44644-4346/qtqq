#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

// ������ŵ�½�ߵ�qq��(Ա����) -- ����Ҫʹ�� -- �˺���֤��ʱ����и�ֵ
// ��ccmainWindow�г�ʼ��Ⱥ��Ϣ��ʱ��ʹ��
QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	// ��ʼ�����������̳��Ը���ķ���
	initTitleBar();
	// ���ñ�����������Ϊ�գ�ͼ��ʹ�ñ�׼��qqͼ��
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	// ���ص�¼���ڵ���ʽ��
	loadStyleSheet("UserLogin");
	initControl();  //��ʼ���ؼ�
}

UserLogin::~UserLogin()
{}

bool UserLogin::connectMysql()
{
	// �������ݿ⣬��������ʹ��mysql���ݿ⣬����ʹ��
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // ע�����ݿ����� -- �������ʹ��
	qDebug() << QSqlDatabase::drivers() << "\r\n";

	// if(QSqlDatabase::isDriverAvailable("QMYSQL") == false) QMessageBox::critical(nullptr, "����", QString::fromLocal8Bit("���ݿ���������ʧ�ܣ�"));

	db.setDatabaseName("qtqq");   // �������ݿ����� -- �������Ѿ����úõ�
	db.setUserName("root");       // ���ö�Ӧ���ݿ�Ķ�Ӧ�˺�
	db.setPassword("msj12345");   // ���ö�Ӧ�����ݿ�����
	db.setPort(3306);            // �������ݿ�˿� -- mysqlĬ�϶˿�Ϊ3306

	if (db.open()) {             // �ж�����ָ�������ݿ��Ƿ��ܹ���
		return true;             // �򿪳ɹ�������true
	}
	else {
		return false;            // ��ʧ�ܣ�����false
	}
}

bool UserLogin::veryfyAccount(bool& isAccountLogin, QString& strAccount)
{
	// ��֤�˺������Ƿ���ȷ���������Ǽ����˺�������ȷ������ֱ�ӷ���true
	QString strAccountInput = ui.editUserAccount->text();   // ��ȡ�û����˺ŵ��������������˺ţ����ұ��棬��������ݿ��д洢���˺Ž��бȶ�
	QString strCodeInput = ui.editPassword->text();   // ��ȡ���룬���洢

	// �����û�������˺ţ�ȥ���ݿ��ѯ�û��������Ƕ��٣�Ȼ�����ƥ��
	// ����Ա����(qq��)  --  ��Ϊ����������������¼��ʽ��һ�����˺�(account)һ����Ա����(employeeID)
	// sql����ѯ���

	// ������д��sql��䣬����Ӧ����ȥ���ݿ�����֤�Ƿ�д����ȷ�����ڳ�����ʹ�ã���ֹ�������ݿ�����޷���ѯ�ṹ���³�������ȫ����Ҫ
	// �˴���ѯemployeeID�Ǹ�����gLoginEmployeeID��ֵ����
	QString strSqlCode = QString("SELECT code,employeeID FROM table_accounts WHERE employeeID = %1").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);   // �����ѯ���,������Ĳ�ѯ��乹��ɲ�ѯ�����������ܺ���ִ�к�ʹ�ã������ṩ�ķ���
	queryEmployeeID.exec();  				 // ִ�в�ѯ���

	// ���������Ǹ���Ա��id��ѯ�����ݣ�������������Ա�����ݿ��Բ�����ݣ���ô˵�����
	// Ա��id�����ݿ��д���(Ҳ����֤�����˺���ȷ)���ٸ������ѯ����ж��������Ƿ���ȷ

	// ���û�в鵽��¼��˵�����ݿ���û�����Ա��id����ô�˺žʹ���
	if (queryEmployeeID.first()) {           // ָ�������ϵĵ�һ�����������ָ�򣬲���˵��������д������ݣ����Ǿ�ȡ����ѯ��������
		//                   ��0��(��Ϊ����ֻ��ѯ��code�����Ե�0�о���code)
		//       ���ݿ���Ա��id��Ӧ������            
		QString strCode = queryEmployeeID.value(0).toString();   // ȡ������ת�����ַ���������

		// ����ƥ�䣬������Ծʹ���
		if (strCode == strCodeInput) {
			// ��Ϊ�����ڵڶ��в�ѯ�������
			gLoginEmployeeID = queryEmployeeID.value(1).toString();
			isAccountLogin = false;  // ��Ϊ������id��¼����������Ϊfalse
			strAccount = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}

	// account�˺ŵ�¼  -- fromlocal8bit�Ĳ�������QString
	strSqlCode = QString("SELECT code FROM table_accounts WHERE account = '%1'").arg(QString::fromLocal8Bit(strAccountInput.toStdString().c_str()));
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();

	// ִ��֮�󣬺�����һ�����ж��Ƿ���Բ�ѯ�����������˵���˺�������ȷ
	if (queryAccount.first()) {
		// �У���ô�˺Ŵ��ڣ���ʼ��֤�����Ƿ���ȷ
		QString strCode = queryAccount.value(0).toString();   // ���Ǵӽ������ȡ������

		if (strCode == strCodeInput) {
			isAccountLogin = true;
			strAccount = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void UserLogin::initControl()
{
	// ����һ��label��ΪԲͷ������壬������Ϊ��ǰ���ڵĴ���Ϊ������
	QLabel* headlabel = new QLabel(this);
	headlabel->setFixedSize(68, 68);   // ���ù̶���С

	// ��ȡԲͷ��֮���ٽ�Բͷ���������
	// ����һ���յ�Բͷ���Դ��Ŀ�ͷ��
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	// ����ǩ����ͷ�� ����֮ǰ��װ�Ļ�ȡһ��Բͷ��ʹ��һ��ͷ���Բͷ��ĵ׿�,����
	// ָ����С���Ϳ���ͨ�����Ƕ���ĺ�������һ��Բͷ���ҷ���
	// ʹ�ö�Ӧ·����ָ������Դ����һ��������ʹ�õ�ͼƬ��
	headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));

	// �����úõĴ���Բͷ��ı�ǩ�ŵ����Ƕ���õ�wigetTile��wigetBody���м���м�
	headlabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	/*�����ڵ���˵�¼��ť֮�������Ӧ�Ĳ�����ִ����Ӧ�Ĳۺ�����������Ҫ��������*/
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	// ����������ݿ�ʧ�ܣ�����ʾ������Ϣ
	if (!connectMysql()) {
		QMessageBox::information(nullptr, "��ʾ", QString::fromLocal8Bit("�������ݿ�ʧ�ܣ�"));
		close();   // �رյ�ǰ����
	}
}

/*�ۺ���*/
void UserLogin::onLoginBtnClicked() {

	// �ж������Ƿ���֤�ɹ���ʧ�ܾ���ʾ(����)
	// ��ȡ��¼���û���������˺Ż���id��
	bool isAccountLogin; 
	QString strAccount;
	if (!veryfyAccount(isAccountLogin, strAccount)) {
		QMessageBox::warning(nullptr, "��ʾ", QString::fromLocal8Bit("��������˺Ż���������������������"));
		// ��Ϊ��������˺ź��������������Ҫ�������룬���Բ���ִ������Ĺرպ͹���
		// �����ڵĴ��룬�����ڴ˴�ֱ�ӷ��أ�Ȼ���ٵ��û����룬���жϣ�ֱ����ȷ��ִ��
		// ����Ĵ��룬���촰�壬û�������壬����Ĵ��붼����ִ�С�

		// ��������˺���������ʱ����գ�Ҳ���Բ������磬���û���ԭ���������Ͻ�����
		// �ģ��������ʵ��Ҫ��

		//ui.editUserAccount->setText("");  // �˺���������
		//ui.editPassword->setText("");     // ������������
		return;
	}

	// ��¼֮��������ݿ���û���״̬Ϊ��¼
	QString strSqlStatus = QString("UPDATE table_employees SET online_status = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();

	// �����¼֮��ע�����ر�
	close();

	// ��������ʾ -- ������ݿ�֮������޸ģ���Ϊ����Ҫ�������ڸ����û���¼��ʱ��
	// ������˺Ż���idȥ���ݿ��в��Ҷ�Ӧ�û���ͷ�񣬵�������������ֵ�ʱ���¼����
	// �Ѿ��ر��ˣ�Ϊ�˵õ���¼��Ϣ�������ڵ�¼���ڹرգ������������ʱ����һЩ����
	// �����Ϳ���֪����
	// ���������ھͿ���֪��������˺�
	CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	mainWindow->show();
}

