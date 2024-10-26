#pragma once

#include <QLabel>

class QClickLabel : public QLabel
{
	Q_OBJECT

Q_SIGNALS:
	//  ��ǩ��������ź�
	void clicked();

protected:
	// ��д��갴���¼�
	void mousePressEvent(QMouseEvent* event);

public:
	QClickLabel(QWidget *parent);
	~QClickLabel();
};
