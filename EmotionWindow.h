#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = nullptr);
	~EmotionWindow();

private:
	void initControl();       // 初始化控件

private slots:
	void addEmotion(int emotionNum);   // 添加表情   根据表情的序号进行添加

Q_SIGNALS:
	void signalEmotionWindowHide();   // 表情窗口隐藏
	void signalEmotionItemClicked(int emotionNum);     // 发送哪个表情被点击了

private:
	void paintEvent(QPaintEvent* event)override;

private:
	Ui::EmotionWindow ui;
};
