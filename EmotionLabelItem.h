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
	void initControl();                     // ��ʼ���ؼ�

Q_SIGNALS:
	void emotionClicked(int emotionNum);    // ���鱻���

private:
	int m_emotionName;
	QMovie* m_apng;        // ʵ�ֶ�ͼ��ʾ
};