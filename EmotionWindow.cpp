#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "QClickLabel.h"
#include "EmotionLabelItem.h"

#include <QStyleOption>
#include <QPainter>

const int emotionColum = 14;
const int emotionRow = 12;

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	// 设置窗口风格
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);          // 设置当关闭最后一个窗体的时候进行窗体资源的回收

	ui.setupUi(this);
	initControl();
}

EmotionWindow::~EmotionWindow()
{}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");  // 加载样式
	// 添加表情，窗体上每个位置都存在一个标签，标签上面放着表情，而且表情是支持点击的
	// 所以我们需要使用自定以的类QClickLabel定义标签，但是我们发出去的表情应该是一个
	// 动态的，这时候其就无法实现了，我们需要使用QMoive类来实现，所以我们再次自定以一个类

	// 加载表情
	for (int row = 0; row < emotionRow; row++) {
		for (int column = 0; column < emotionColum; column++) {
			EmotionLabelItem *label = new EmotionLabelItem(this);  // 表情窗体作为父窗体
			label->setEmotionName(row * emotionColum + column);

			connect(label, &EmotionLabelItem::emotionClicked, this, &EmotionWindow::addEmotion);
			ui.gridLayout->addWidget(label, row, column);    // 将表情添加到布局中去
		}
	}
}

void EmotionWindow::addEmotion(int emotionNum) {
	hide();    // 只有点击的时候才显示
	emit signalEmotionWindowHide();   // 发射表情窗口隐藏的信号
	emit signalEmotionItemClicked(emotionNum);  // 发射表情被点击的信号
}

void EmotionWindow::paintEvent(QPaintEvent* event) {
	// 获取当前的风格
	QStyleOption opt;
	opt.init(this);      //对其进行初始化

	QPainter painter(this);

	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);


	__super::paintEvent(event);   // 绘制其它的绘图
}