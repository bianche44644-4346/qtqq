#include "SysTray.h"
#include "CustomMenu.h"

SysTray::SysTray(QWidget *parent)
	: m_parent(parent), QSystemTrayIcon(parent)
{
	initSystemTray();                // 初始化系统托盘
	show();                          // 显示
}

SysTray::~SysTray()
{}

void SysTray::initSystemTray()
{
	// 设置一个提示 -- 鼠标放在托盘图标上面的时候有个提示
	setToolTip(QStringLiteral("QQ"));    // 使用宏构造一个QString

	// 设置图标
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));

	// 当我们点击托盘图标的时候，其对应的窗体进行显示，这时候我们需要进行一个信号和槽
	// 的链接
	connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
		, this, SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
}

// 添加菜单
void SysTray::addSysTrayMenu()
{
	// 实现两种，一种是显示，一种是关闭
	CustomMenu* customMenu = new CustomMenu(m_parent);
	// 添加显示菜单
	//                       动作按钮名称  图标所在位置
	customMenu->addCustomMenu("onShow", ":/Resources/MainWindow/app/logo.ico",
		QStringLiteral("显示"));  // 文本

	// 添加关闭菜单
	customMenu->addCustomMenu("onQuit",
		":/Resources/MainWindow/app/page_close_btn_hover.png",
		QStringLiteral("退出"));  // 文本

	// 功能实现，信号槽的链接
	//  显示
	//      返回显示的动作按钮       
	connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)),
		m_parent,
		SLOT(onShowNormal(bool)));    // 正常显示

 // 关闭
	connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent,
		SLOT(onShowQuit(bool)));         // 退出

	// 将菜单进入事件循环，接收鼠标的操作
	// 让菜单进入自己事件循环，让其跑起来
	customMenu->exec(QCursor::pos());  // pos()是跑起来的在坐标系中的位置

	// 事件结束的时候对资源进行释放 -- 此处为特殊情况，如果有父部件，父部件会代替我们
	// 释放资源，但是此时我们是自己进入了事件循环，所以我们需要自己释放资源
	delete customMenu;
	customMenu = nullptr;
}

/*
   根据触发托盘图标的原因，来进行相应的操作
*/
void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason) {
	if (reason == QSystemTrayIcon::Trigger) {
		m_parent->show();             // 将其父窗体进行显示
		m_parent->activateWindow();   // 将主窗口设置为活动窗口
	}
	else if (reason == QSystemTrayIcon::Context) {  // 触发的是系统的托盘内容的时候显示菜单
		addSysTrayMenu();
	}
}

