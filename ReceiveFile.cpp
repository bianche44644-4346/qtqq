#include "ReceiveFile.h"
#include <QFileDialog>
#include <QMessageBox>

// ��Ϊ�ڽ������ݵ�ʱ����Щ��Ϣ���Ƕ������õ������Կ�����TalkWindowShell�������ݵ��ļ��н���Щ���ݴ洢��ȫ�ֱ����У�Ȼ���������������
extern QString gFileName;
extern QString gFileData;

ReceieveFile::ReceieveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);  // ���ڹرյ�ʱ�������Դ����

    // ��Щ�������Ǽ̳���BasicWindow
	initTitleBar();         // ��ʼ������
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");     // ���ñ�������
	loadStyleSheet("ReceiveFile");   // ������ʽ��

	// ����ͷ����ļ������λ�ò�һ������Ϊ���ǵĴ�С��һ��������Ҫ���ڶ�Ӧλ��Ҳ���������λ��
	this->move(200, 400);     // �������ƶ���100,400��λ��

	/*
	   ���ڷ��͹�������Ϣ�����Ǹ�����������ť�����ջ�����ȡ������������û��ĸ�������
	   �Ļ���ֱ�ӵ���رմ��ڣ�Ҳ��ʾ�Ǿܽӽ��ܣ������ź����ź�
	*/
	// �����ڵ���رհ�ť���ͳ��ź�֮�����ǲ����вۺ����������ô˴��巢��һ���ܾ����ܵ��źţ�ʵ���źź��źŵ�ͨ��
	connect(_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceieveFile::refuseFile);
}

ReceieveFile::~ReceieveFile()
{}

void ReceieveFile::setMsg(QString & msgLabel)
{
	ui.label->setText(msgLabel);   // ���ñ�ǩ�е��ı�
}

void ReceieveFile::on_okBtn_clicked() {
	this->close();   // ���֮��رմ���  -- Qt�е��ùرմ��ڵĺ��������������Ϲرմ��ڣ��������Ӧ
	                 // ����Դ����֮����Դ����������ٽ��йرգ����Ե���close()����֮�����ǻ����Խ���һЩ��Ϣ��������������Ȼ����close()�����������滹���Լ�������

	// ��ȡ��Ҫ������ļ�·�� -- ��һ����������һ���գ�������this�����Ƿ�ֹ����close()��ǰ�����Ѿ�������
	QString fileDirPath = QFileDialog::getExistingDirectory(nullptr,
		QStringLiteral("�ļ�����·��"),
		"/");

	QString filePath = fileDirPath + "/" + gFileName;  //д�봫��������ݵ��ļ�·��

	// ��Ŀ��·��д���µ��ļ�
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::information(nullptr,
			QStringLiteral("��ʾ"),
			QStringLiteral("�ļ�����ʧ��!!"));
	}
	else {  // �ɹ���֮�����д������
		file.write(gFileData.toUtf8());
		file.close();  // д���ۺ�ر�
		QMessageBox::information(nullptr,
			QStringLiteral("��ʾ"),
			QStringLiteral("�ļ����ճɹ�!!"));
	}
}

void ReceieveFile::on_cancelBtn_clicked()
{
	emit refuseFile();  // ȡ�����ǲ������ļ��������ź�
	this->close();      // Ȼ��رմ���
}
