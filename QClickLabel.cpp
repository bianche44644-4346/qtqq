#include "QClickLabel.h"
#include <QMouseEvent>

QClickLabel::QClickLabel(QWidget *parent)
	: QLabel(parent)
{}

QClickLabel::~QClickLabel()
{}

void QClickLabel::mousePressEvent(QMouseEvent* event)
{
	// 如果用户按下了左键，就发射一个信号
	if (event->button() == Qt::LeftButton)
		emit clicked();   // 发送信号
}
