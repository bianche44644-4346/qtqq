#include "EmotionLabelItem.h"
#include <QMovie>


EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: QClickLabel(parent)
{
	initControl();
	// 表情被点击之后发送信号，所以需要链接信号槽
	connect(this, &QClickLabel::clicked, [this]() {
		emit emotionClicked(m_emotionName);     // 当表情被点击之后，就发送一个信号
	});
}

EmotionLabelItem::~EmotionLabelItem()
{}

void EmotionLabelItem::setEmotionName(int emotionName)
{
	m_emotionName = emotionName;
	QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionName);

	// 设置动图
	m_apng = new QMovie(imageName, "apng", this);  // 图片名字， 转换的动图格式，父类
	m_apng->start();
	m_apng->stop();
	setMovie(m_apng);
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);
	setObjectName("emotionLabelItem");    // 设置对象的名字
	setFixedSize(32, 32);

}
