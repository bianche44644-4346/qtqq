#include "SendFile.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QFileDialog>
#include <QMessageBox>

SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent),m_filePath("")  // ��ʼ��Ϊ��
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);  // ���ڹرյ�ʱ�������Դ����

	// ��Щ�������Ǽ̳���BasicWindow
	initTitleBar();         // ��ʼ������
	setTitleBarTitle("",":/Resources/MainWindow/qqlogoclassic.png");     // ���ñ�������
	loadStyleSheet("SendFile");   // ������ʽ��

	this->move(100, 400);     // �������ƶ���100,400��λ��

	// ��Ϊ���ǵ�����Ͱ�ť֮��talkWindowShell�еĲۺ��������·��͵���Ϣ����Ҫ���ļ�
	// ��Ϣ���з�װ��Ȼ����з���  --  ����Ҫ���źźͲ۽�������
	// ��Ϊ�ź���۵������Ƕ���֮���ͨ�ţ�������Ҫ��ȡtalkWindowShell�Ķ���  -- ���÷�װ�õĽӿ�
	TalkWindowShell *talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &SendFile::sendFileClicked, talkWindowShell, &TalkWindowShell::updateSendTcpMessage);
}

SendFile::~SendFile()
{}

void SendFile::on_openBtn_clicked() {
	// ���ļ��Ի����û�ѡ���ļ�֮�󣬱���һ���ļ�·��  --  �����ļ��Ի���ķ������л�ȡ
	m_filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ���ļ�")
		, "/"
		, QString::fromLocal8Bit("���͵��ļ�(*.txt *.doc);;�����ļ�(*.*);;"));

	// ���������
	ui.lineEdit->setText(m_filePath);
}

void SendFile::on_sendBtn_clicked()
{
	// �ж��ļ�·���Ƿ�Ϸ�  --  �Ϸ��ͽ��в���(��Ҫ�Ǽ���Ƿ�Ϊ��)
	if (!m_filePath.isEmpty()) {  
		QFile file(m_filePath);
		if (file.open(QIODevice::ReadOnly)) {
			int msgType = 2;     // ������Ϣ����Ϊ�ļ������ڷ����ź�
			QString str = file.readAll();

			// ��ȡ�ļ����� -- ͨ�����QFileInfo���ȡ�ļ������������ָ��·���µ��ļ���Ϣ
			QFileInfo fileinfo(m_filePath);
			QString fileName = fileinfo.fileName();

			emit sendFileClicked(str, msgType, fileName);
			file.close();   // �ر��ļ�
		}
		else {
			QMessageBox::information(this, QStringLiteral("��ʾ"), QString::fromLocal8Bit("��ȡ�ļ�:%1ʧ��").arg(m_filePath));
			m_filePath = "";   // ʧ��֮������ļ�·����վͺã���Ϊ�Ǵ����
			ui.lineEdit->clear();
			this->close();    // �رմ���
			return;
		}

		m_filePath = "";   // ʧ��֮������ļ�·����վͺã���Ϊ�Ǵ����
		this->close();     // �رմ���
	}
}
