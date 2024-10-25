#pragma once

#include "QClickLabel.h"

class QMovie;

class EmotionLabelItem : public QClickLabel
{
	Q_OBJECT

public:
	EmotionLabelItem(QWidget *parent);
	~EmotionLabelItem();
	void setEmotionName(int emotionName);

private:
	void initControl();                     // 初始化控件

Q_SIGNALS:
	void emotionClicked(int emotionNum);    // 表情被点击

private:
	int m_emotionName;
	QMovie* m_apng;        // 实现动图显示
};
