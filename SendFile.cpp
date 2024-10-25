#include "SendFile.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QFileDialog>
#include <QMessageBox>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent),m_filePath("")  // 开始置为空
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);  // 窗口关闭的时候进行资源回收

	// 这些函数都是继承自BasicWindow
	initTitleBar();         // 初始化标题
	setTitleBarTitle("",":/Resources/MainWindow/qqlogoclassic.png");     // 设置标题内容
	loadStyleSheet("SendFile");   // 加载样式表

	this->move(100, 400);     // 将窗口移动到100,400的位置

	// 因为我们点击发送按钮之后，talkWindowShell中的槽函数，更新发送的信息，需要将文件
	// 信息进行封装，然后进行发送  --  所以要对信号和槽进行连接
	// 因为信号与槽的链接是对象之间的通信，所以需要获取talkWindowShell的对象  -- 调用封装好的接口
	TalkWindowShell *talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMessage);
}

SendFile::~SendFile()
{}

void SendFile::on_openBtn_clicked() {
	// 打开文件对话框，用户选择文件之后，保存一下文件路径  --  调用文件对话框的方法进行获取
	m_filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件")
		, "/"
		, QString::fromLocal8Bit("发送的文件(*.txt *.doc);;所有文件(*.*);;"));

	// 更新输入框
	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	// 判断文件路径是否合法  --  合法就进行操作(主要是检测是否为空)
	if (!m_filePath.isEmpty()) {  
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly)) {
			int msgType = 2;     // 保存信息类型为文件，用于发送信号
			QString str = file.readAll();

			// 获取文件名称 -- 通过这个QFileInfo类获取文件名，这个类存放指定路径下的文件信息
			QFileInfo fileinfo(m_filePath);
			QString fileName = fileinfo.fileName();

			emit sendFileClicked(str, msgType, fileName);
			file.close();   // 关闭文件
		}
		else {
			QMessageBox::information(this, QStringLiteral("提示"), QString::fromLocal8Bit("读取文件:%1失败").arg(m_filePath));
			m_filePath = "";   // 失败之后这个文件路径清空就好，因为是错误的
			ui.lineEdit->clear();
			this->close();    // 关闭窗口
			return;
		}

		m_filePath = "";   // 失败之后这个文件路径清空就好，因为是错误的
		this->close();     // 关闭窗口
	}
}
