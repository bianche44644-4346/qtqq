#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();

	// 数据库进行了修改 
	void setHeadPixmap(const QPixmap& pixmap/*const QString& pixmap*/);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();

private:
	void initControl();

Q_SIGNALS:
	void signalCloseClicked();      // 发送关闭窗口的信号

private:
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);

private:
	Ui::TalkWindowItemClass ui;
};
