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
	void initControl();       // ��ʼ���ؼ�

private slots:
	void addEmotion(int emotionNum);   // ��ӱ���   ���ݱ������Ž������

Q_SIGNALS:
	void signalEmotionWindowHide();   // ���鴰������
	void signalEmotionItemClicked(int emotionNum);     // �����ĸ����鱻�����

private:
	void paintEvent(QPaintEvent* event)override;

private:
	Ui::EmotionWindow ui;
};
