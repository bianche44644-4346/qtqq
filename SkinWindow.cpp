#include "SkinWindow.h"
#include "NotifyManager.h"
#include "QClickLabel.h"


SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	loadStyleSheet("SkinWindow");  // 加载SkinWindow的样式表
	setAttribute(Qt::WA_DeleteOnClose);   // 设置属性，当窗体收到关闭事件的时候就会进行关闭
	initControl();
}

SkinWindow::~SkinWindow()
{}

/*
  初始化控件
*/
void SkinWindow::initControl()
{
	// 将我们可以选择的皮肤颜色设置在QColor中
	QList<QColor> colorList = {  // 3行4列颜色
		QColor(22,154,218),QColor(40,138,221), QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),  QColor(89,92,160), QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77), QColor(240,188,189),
	};

	// 初始化控件 -- 3行4列的标签(标签中存放多个颜色)
	// 此处的标签是需要点击之后进行相应，然后进行颜色的修改，但是我们普通的标签是没有
	// 办法进行点击响应的，这时候我们需要重写一下QLabel的类，让其能够响应点击
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 4; col++) {
			QClickLabel* label = new QClickLabel(this);   // 定义一个可以被点击的标签
			label->setCursor(Qt::PointingHandCursor);     // 点击是手的形状
			// 点击之后发射信号，链接信号与槽
			// 当我们点击标签，选中相应的标签的时候，对应的窗体都应该改变颜色，所以
			// 我们点击之后所有的窗体都应该收到信号

			// 使用lambda表达式，直接写槽函数,同时实现放给多个窗体
			connect(label, &QClickLabel::clicked, [row, col, colorList]() {
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + col) // 用户点击的新颜色
				);
			});

			label->setFixedSize(84, 84);     // 笃定标签大小

			//设置标签的颜色
			QPalette palette;
			palette.setColor(QPalette::Background, colorList.at(row * 4 + col));
			label->setAutoFillBackground(true);  // 自动填充背景颜色
			label->setPalette(palette);

			// 添加到布局管理器当中去
			ui.gridLayout->addWidget(label, row, col);  // 在栅格的对应位置添加label
		}
	}

	// 连接点击皮肤窗口的按钮实现效果，进行信号与槽的链接
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

void SkinWindow::onShowClose() {
	// 直接调用窗口的close进行关闭
	close();
}

