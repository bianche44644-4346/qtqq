#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

// 用来存放登陆者的qq号(员工号) -- 后面要使用 -- 账号验证的时候进行赋值
// 在ccmainWindow中初始化群信息的时候使用
QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	// 初始化标题栏，继承自父类的方法
	initTitleBar();
	// 设置标题栏，内容为空，图标使用标准的qq图标
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	// 加载登录窗口的样式表
	loadStyleSheet("UserLogin");
	initControl();  //初始化控件
}

UserLogin::~UserLogin()
{}

bool UserLogin::connectMysql()
{
	// 连接数据库，这里我们使用mysql数据库，所以使用
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // 注册数据库驱动 -- 方便后续使用
	qDebug() << QSqlDatabase::drivers() << "\r\n";

	// if(QSqlDatabase::isDriverAvailable("QMYSQL") == false) QMessageBox::critical(nullptr, "错误", QString::fromLocal8Bit("数据库驱动加载失败！"));

	db.setDatabaseName("qtqq");   // 设置数据库名称 -- 用我们已经设置好的
	db.setUserName("root");       // 设置对应数据库的对应账号
	db.setPassword("msj12345");   // 设置对应的数据库密码
	db.setPort(3306);            // 设置数据库端口 -- mysql默认端口为3306

	if (db.open()) {             // 判断我们指定的数据库是否能够打开
		return true;             // 打开成功，返回true
	}
	else {
		return false;            // 打开失败，返回false
	}
}

bool UserLogin::veryfyAccount(bool& isAccountLogin, QString& strAccount)
{
	// 验证账号密码是否正确，这里我们假设账号密码正确，所以直接返回true
	QString strAccountInput = ui.editUserAccount->text();   // 获取用户在账号的输入框中输入的账号，并且保存，方便和数据库中存储的账号进行比对
	QString strCodeInput = ui.editPassword->text();   // 获取密码，并存储

	// 根据用户输入的账号，去数据库查询用户的密码是多少，然后进行匹配
	// 输入员工号(qq号)  --  因为我们这里有两个登录方式，一种是账号(account)一种是员工号(employeeID)
	// sql语句查询语句

	// 开发中写的sql语句，我们应该先去数据库中验证是否写的正确，再在程序中使用，防止出现数据库语句无法查询结构导致出错，这完全不必要
	// 此处查询employeeID是给后面gLoginEmployeeID赋值所用
	QString strSqlCode = QString("SELECT code,employeeID FROM table_accounts WHERE employeeID = %1").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);   // 构造查询语句,将上面的查询语句构造成查询对象，这样才能后面执行和使用，调用提供的方法
	queryEmployeeID.exec();  				 // 执行查询语句

	// 我们上面是根据员工id查询的数据，如果根据输入的员工数据可以查出数据，那么说明这个
	// 员工id在数据库中存在(也就是证明了账号正确)，再根据其查询结果判断其密码是否正确

	// 如果没有查到记录，说明数据库中没有这个员工id，那么账号就错误
	if (queryEmployeeID.first()) {           // 指向结果集合的第一条，如果可以指向，并且说明结果集中存在数据，我们就取出查询出的密码
		//                   第0列(因为我们只查询了code，所以第0列就是code)
		//       数据库中员工id对应的密码            
		QString strCode = queryEmployeeID.value(0).toString();   // 取出密码转换成字符串并保存

		// 密码匹配，如果不对就错误
		if (strCode == strCodeInput) {
			// 因为上面在第二列查询到结果了
			gLoginEmployeeID = queryEmployeeID.value(1).toString();
			isAccountLogin = false;  // 因为这里是id登录，所以设置为false
			strAccount = strAccountInput;
			return true;
		}
		else {
			return false;
		}
	}

	// account账号登录  -- fromlocal8bit的参数不是QString
	strSqlCode = QString("SELECT code FROM table_accounts WHERE account = '%1'").arg(QString::fromLocal8Bit(strAccountInput.toStdString().c_str()));
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();

	// 执行之后，和上面一样，判断是否可以查询出结果，有则说明账号输入正确
	if (queryAccount.first()) {
		// 有，那么账号存在，开始验证密码是否正确
		QString strCode = queryAccount.value(0).toString();   // 还是从结果集中取出密码

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
	// 创建一个label作为圆头像的载体，将其作为当前所在的窗体为父窗体
	QLabel* headlabel = new QLabel(this);
	headlabel->setFixedSize(68, 68);   // 设置固定大小

	// 获取圆头像之后，再将圆头像进行设置
	// 设置一个空的圆头像，脑袋的空头像
	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	// 给标签设置头像， 调用之前分装的获取一个圆头像，使用一个头像和圆头像的底框,并且
	// 指定大小，就可以通过我们定义的函数创建一个圆头像并且返回
	// 使用对应路径中指定的资源创建一个程序中使用的图片。
	headlabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headlabel->size()));

	// 将设置好的带有圆头像的标签放到我们定义好的wigetTile和wigetBody的中间的中间
	headlabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	/*我们在点击了登录按钮之后进行相应的操作，执行相应的槽函数，所以需要建立连接*/
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	// 如果链接数据库失败，则提示错误信息
	if (!connectMysql()) {
		QMessageBox::information(nullptr, "提示", QString::fromLocal8Bit("连接数据库失败！"));
		close();   // 关闭当前窗口
	}
}

/*槽函数*/
void UserLogin::onLoginBtnClicked() {

	// 判断输入是否验证成功，失败就提示(警告)
	// 获取登录是用户输入的是账号还是id号
	bool isAccountLogin; 
	QString strAccount;
	if (!veryfyAccount(isAccountLogin, strAccount)) {
		QMessageBox::warning(nullptr, "提示", QString::fromLocal8Bit("您输入的账号或者密码有误，请重新输入"));
		// 因为我们这里，账号和密码输入错误需要重新输入，所以不能执行下面的关闭和构造
		// 主窗口的代码，所以在此处直接返回，然后再等用户输入，再判断，直到正确才执行
		// 后面的代码，构造窗体，没有主窗体，后面的代码都不会执行。

		// 你可以在账号密码输错的时候清空，也可以不清坤哥，让用户在原来的输入上进行修
		// 改，看具体的实现要求

		//ui.editUserAccount->setText("");  // 账号输入框清空
		//ui.editPassword->setText("");     // 密码输入框清空
		return;
	}

	// 登录之后更新数据库的用户的状态为登录
	QString strSqlStatus = QString("UPDATE table_employees SET online_status = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery sqlStatus(strSqlStatus);
	sqlStatus.exec();

	// 点击登录之后，注册界面关闭
	close();

	// 主窗口显示 -- 添加数据库之后进行修改，因为我们要在主窗口根据用户登录的时候
	// 输入的账号或者id去数据库中查找对应用户的头像，但是在主窗体出现的时候登录窗体
	// 已经关闭了，为了得到登录信息，我们在登录窗口关闭，构造主窗体的时候传入一些参数
	// 这样就可以知道了
	// 这样主窗口就可以知道输入的账号
	CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	mainWindow->show();
}

