#include "TalkWindowItem.h"
#include "CommonUtils.h"

TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initControl();
}

TalkWindowItem::~TalkWindowItem()
{}

// 使用数据库进行修改
void TalkWindowItem::setHeadPixmap(const QPixmap& pix/*const QString & pixmap*/)
{
	// 因为是圆头像，所以需要获取一下源头像
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");

	// 数据库进行了修改
//	QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");   // 这样写，先看一下效果

	// 使用数据库之后进行修改，直接使用传入的pix
	const QPixmap& headpixmap = CommonUtils::getRoundImage(pix, mask, ui.headlabel->size());
	// const QPixmap& headpixmap = CommonUtils::getRoundImage(head, mask, ui.headlabel->size());
	ui.headlabel->setPixmap(headpixmap);
}

void TalkWindowItem::setMsgLabelContent(const QString& msg)
{
	// 设置内容
	ui.msgLabel->setText(msg);
}

QString TalkWindowItem::getMsgLabelText()
{
	// 获取label标签中的数据
	return ui.msgLabel->text();
}

void TalkWindowItem::initControl()
{
	ui.tCloseBtn->setVisible(false);     // 关闭按钮最开始是不可见的，所以设置为false

	// 因为通过当前的窗口无法实现将聊天窗口进行关闭，所以我们这里在链接到当前的窗口的
	// 时候不去调用槽函数，而是去发送一个信号，告诉聊天窗口，然后聊天窗口进行关闭即可
	connect(ui.tCloseBtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}

void TalkWindowItem::enterEvent(QEvent* event)
{
	// 一旦我们进入到聊天项，那么按钮就可见就可以了
	ui.tCloseBtn->setVisible(true);
	__super::enterEvent(event);      // 进行其它的默认操作的事件
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	// 离开聊天项，关闭按钮不显示
	ui.tCloseBtn->setVisible(false);
	__super::leaveEvent(event);
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	// 尺寸重置
	__super::resizeEvent(event);
}
