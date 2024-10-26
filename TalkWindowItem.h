#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();

	// ���ݿ�������޸� 
	void setHeadPixmap(const QPixmap& pixmap/*const QString& pixmap*/);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();

private:
	void initControl();

Q_SIGNALS:
	void signalCloseClicked();      // ���͹رմ��ڵ��ź�

private:
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);

private:
	Ui::TalkWindowItemClass ui;
};
