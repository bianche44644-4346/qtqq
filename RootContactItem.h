#pragma once

#include <QLabel>
#include <QPropertyAnimation>
/*根项*/
/*项目展开的时候，三角形的方向会发生改变，表示展开和收缩，这个类用来实现相应的功能*/
class RootContactItem : public QLabel
{
	Q_OBJECT

		// 箭头角度
		Q_PROPERTY(int rotation READ rotation WRITE setRotation)

public:
	// 参数：是否有箭头，默认是有的
	RootContactItem(bool hasArrow = true, QWidget *parent = nullptr);
	~RootContactItem();

public:
	void setText(const QString& text);    // 设置文本
	void setExpanded(bool expand);        // 是否展开

private:
	int rotation();                       // 获取角度
	void setRotation(int rotation);                   // 设置角度

protected:
	void paintEvent(QPaintEvent* event);

private:
	QPropertyAnimation* m_animation;       // 使用动画属性(用于箭头转动)
	QString m_titleText;                   // 显示的文本
	int m_rotation;                        // 箭头的角度
	bool m_hasArrow;                       // 是否有箭头
};
