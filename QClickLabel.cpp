#include "QClickLabel.h"
#include <QMouseEvent>

QClickLabel::QClickLabel(QWidget *parent)
	: QLabel(parent)
{}

QClickLabel::~QClickLabel()
{}

void QClickLabel::mousePressEvent(QMouseEvent* event)
{
	// ����û�������������ͷ���һ���ź�
	if (event->button() == Qt::LeftButton)
		emit clicked();   // �����ź�
}
