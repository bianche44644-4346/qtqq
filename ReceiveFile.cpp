#include "ReceiveFile.h"
#include <QFileDialog>
#include <QMessageBox>

// 因为在解析数据的时候这些信息我们都可以拿到，所以可以在TalkWindowShell解析数据的文件中将这些数据存储在全局变量中，然后在这里进行引用
extern QString gFileName;
extern QString gFileData;

ReceieveFile::ReceieveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);  // 窗口关闭的时候进行资源回收

    // 这些函数都是继承自BasicWindow
	initTitleBar();         // 初始化标题
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");     // 设置标题内容
	loadStyleSheet("ReceiveFile");   // 加载样式表

	// 这里和发送文件窗体的位置不一样，因为它们的大小不一样，所以要想在对应位置也得设置相对位置
	this->move(200, 400);     // 将窗口移动到100,400的位置

	/*
	   对于发送过来的信息，我们给定了两个按钮，接收或者是取消，但是如果用户哪个都不点
	   的话，直接点击关闭窗口，也表示是拒接接受，连接信号与信号
	*/
	// 我们在点击关闭按钮发送出信号之后，我们不进行槽函数，而是让此窗体发送一个拒绝接受的信号，实现信号和信号的通信
	connect(_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceieveFile::refuseFile);
}

ReceieveFile::~ReceieveFile()
{}

void ReceieveFile::setMsg(QString & msgLabel)
{
	ui.label->setText(msgLabel);   // 设置标签中的文本
}

void ReceieveFile::on_okBtn_clicked() {
	this->close();   // 点击之后关闭窗口  -- Qt中调用关闭窗口的函数，并不会马上关闭窗口，会进行相应
	                 // 的资源管理之后，资源处理结束了再进行关闭，所以调用close()函数之后我们还可以进行一些信息处理，就像这里虽然调用close()但是我们下面还可以继续操作

	// 获取想要保存的文件路径 -- 第一个参数传入一个空，而不是this，就是防止调用close()当前窗体已经销毁了
	QString fileDirPath = QFileDialog::getExistingDirectory(nullptr,
		QStringLiteral("文件保存路径"),
		"/");

	QString filePath = fileDirPath + "/" + gFileName;  //写入传输过来数据的文件路径

	// 在目标路径写入新的文件
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::information(nullptr,
			QStringLiteral("提示"),
			QStringLiteral("文件接收失败!!"));
	}
	else {  // 成功打开之后进行写入数据
		file.write(gFileData.toUtf8());
		file.close();  // 写完折后关闭
		QMessageBox::information(nullptr,
			QStringLiteral("提示"),
			QStringLiteral("文件接收成功!!"));
	}
}

void ReceieveFile::on_cancelBtn_clicked()
{
	emit refuseFile();  // 取消就是不接受文件，发送信号
	this->close();      // 然后关闭窗口
}
