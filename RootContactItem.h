#pragma once

#include <QLabel>
#include <QPropertyAnimation>
/*����*/
/*��Ŀչ����ʱ�������εķ���ᷢ���ı䣬��ʾչ�������������������ʵ����Ӧ�Ĺ���*/
class RootContactItem : public QLabel
{
	Q_OBJECT

		// ��ͷ�Ƕ�
		Q_PROPERTY(int rotation READ rotation WRITE setRotation)

public:
	// �������Ƿ��м�ͷ��Ĭ�����е�
	RootContactItem(bool hasArrow = true, QWidget *parent = nullptr);
	~RootContactItem();

public:
	void setText(const QString& text);    // �����ı�
	void setExpanded(bool expand);        // �Ƿ�չ��

private:
	int rotation();                       // ��ȡ�Ƕ�
	void setRotation(int rotation);                   // ���ýǶ�

protected:
	void paintEvent(QPaintEvent* event);

private:
	QPropertyAnimation* m_animation;       // ʹ�ö�������(���ڼ�ͷת��)
	QString m_titleText;                   // ��ʾ���ı�
	int m_rotation;                        // ��ͷ�ĽǶ�
	bool m_hasArrow;                       // �Ƿ��м�ͷ
};
