#include "EmotionLabelItem.h"
#include <QMovie>


EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: QClickLabel(parent)
{
	initControl();
	// ���鱻���֮�����źţ�������Ҫ�����źŲ�
	connect(this, &QClickLabel::clicked, [this]() {
		emit emotionClicked(m_emotionName);     // �����鱻���֮�󣬾ͷ���һ���ź�
	});
}

EmotionLabelItem::~EmotionLabelItem()
{}

void EmotionLabelItem::setEmotionName(int emotionName)
{
	m_emotionName = emotionName;
	QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionName);

	// ���ö�ͼ
	m_apng = new QMovie(imageName, "apng", this);  // ͼƬ���֣� ת���Ķ�ͼ��ʽ������
	m_apng->start();
	m_apng->stop();
	setMovie(m_apng);
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);
	setObjectName("emotionLabelItem");    // ���ö��������
	setFixedSize(32, 32);

}
