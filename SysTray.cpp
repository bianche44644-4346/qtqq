#include "SysTray.h"
#include "CustomMenu.h"

SysTray::SysTray(QWidget *parent)
	: m_parent(parent), QSystemTrayIcon(parent)
{
	initSystemTray();                // ��ʼ��ϵͳ����
	show();                          // ��ʾ
}

SysTray::~SysTray()
{}

void SysTray::initSystemTray()
{
	// ����һ����ʾ -- ����������ͼ�������ʱ���и���ʾ
	setToolTip(QStringLiteral("QQ"));    // ʹ�ú깹��һ��QString

	// ����ͼ��
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));

	// �����ǵ������ͼ���ʱ�����Ӧ�Ĵ��������ʾ����ʱ��������Ҫ����һ���źźͲ�
	// ������
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
		, this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
}

// ��Ӳ˵�
void SysTray::addSysTrayMenu()
{
	// ʵ�����֣�һ������ʾ��һ���ǹر�
	CustomMenu* customMenu = new CustomMenu(m_parent);
	// �����ʾ�˵�
	//                       ������ť����  ͼ������λ��
	customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico",
		QStringLiteral("��ʾ"));  // �ı�

	// ��ӹرղ˵�
	customMenu->addCustomMenu("onQuit",
		":/Resources/MainWindow/app/page_close_btn_hover.png",
		QStringLiteral("�˳�"));  // �ı�

	// ����ʵ�֣��źŲ۵�����
	//  ��ʾ
	//      ������ʾ�Ķ�����ť       
	connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)),
		m_parent,
		SLOT(onShowNormal(bool)));    // ������ʾ

 // �ر�
	connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent,
		SLOT(onShowQuit(bool)));         // �˳�

	// ���˵������¼�ѭ�����������Ĳ���
	// �ò˵������Լ��¼�ѭ��������������
	customMenu->exec(QCursor::pos());  // pos()����������������ϵ�е�λ��

	// �¼�������ʱ�����Դ�����ͷ� -- �˴�Ϊ�������������и����������������������
	// �ͷ���Դ�����Ǵ�ʱ�������Լ��������¼�ѭ��������������Ҫ�Լ��ͷ���Դ
	delete customMenu;
	customMenu = nullptr;
}

/*
   ���ݴ�������ͼ���ԭ����������Ӧ�Ĳ���
*/
void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		m_parent->show();             // ���丸���������ʾ
		m_parent->activateWindow();   // ������������Ϊ�����
	}
	else if (reason == QSystemTrayIcon::Context) {  // ��������ϵͳ���������ݵ�ʱ����ʾ�˵�
		addSysTrayMenu();
	}
}

