#pragma once

#include <QLabel>

class QClickLabel : public QLabel
{
	Q_OBJECT

Q_SIGNALS:
	//  标签被点击的信号
	void clicked();

protected:
	// 重写鼠标按下事件
	void mousePressEvent(QMouseEvent* event);

public:
	QClickLabel(QWidget *parent);
	~QClickLabel();
};
