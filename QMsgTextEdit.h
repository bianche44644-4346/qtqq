#pragma once

#include <QTextEdit>

class QMsgTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();

private slots:
	// �����ı�ʱ����Ӧ�Ĳۺ���
	void onEmotionImageFrameChange(int frame);   // ����һ��������֡

public:
	// ��ӱ���
	void addEmotionUrl(int emotionNum);          // ������ʾ��������ĸ�����  
	void deleteAllEmotionImage();                // ɾ�����еı���ͼƬ -- ��������ѡ���˱���֮�������ʹ���һ�����飬���ǵ������֮���������Ӧ���ڷ��Ϳ��о�û���ˣ������ǻ�ͣ����������

private:
	QList<QString> m_listEmotionUrl;             // ����ѡ�еı���
	QMap<QMovie*, QString> m_emotionMap;         // �����Ӱ�ͱ����ӳ���ϵ
};
