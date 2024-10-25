#pragma once

#include <QDialog>
#include "TitleBar.h"

/* 基础窗体类，用于其它的窗口的基类 */
class BasicWindow : public QDialog
{
	Q_OBJECT

public:
	BasicWindow(QWidget *parent = Q_NULLPTR);
	virtual ~BasicWindow();  // 虚析构

public:
	// 加载样式表
	void loadStyleSheet(const QString& sheetName);

	// 获取圆头像
	QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));

private:
	void initBackGroundColor();                 // 初始化背景

protected:
	void paintEvent(QPaintEvent*event) override; // 绘图事件
	void mousePressEvent(QMouseEvent* event) override;   // 鼠标移动事件
	void mouseMoveEvent(QMouseEvent* event) override;    // 鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event) override; // 鼠标释放事件

protected:
	void initTitleBar(ButtonType buttontype = MIN_BUTTON);   // 初始化标题栏，传入一个按钮的大小
	void setTitleBarTitle(const QString& title, const QString& icon = "");  // 设置标题栏内容，指定标题栏的名字和图标(默认为空)

public slots:
	void onShowClose(bool);   // 窗口关闭
	void onShowMin(bool);     // 窗口小化
	void onShowHide(bool);    // 窗口隐藏
	void onShowNormal(bool);  // 正常显示
	void onShowQuit(bool);    // 程序退出
	void onSignalSkinChanged(const QColor &color);      // 窗口皮肤发生改变的时候进行操作

	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
	QPoint m_mousePoint;         // 鼠标位置
	bool  m_mousePressed;        // 鼠标是否按下
	QColor m_colorBackGround;    // 背景色
	QString m_styleName;         // 样式文件名称
	TitleBar* _titleBar;        // 标题栏，这就用到了我们自定义的标题栏的类
};
