#pragma once
/* 自定义标题栏 */

#include <QWidget>
#include <QLabel>
#include <QPushButton>

/*
   使用枚举值表示，我们自己定义的窗口的几种风格，当然也可以自定义别的
*/
enum ButtonType              // 按钮类型模式的枚举
{
	MIN_BUTTON = 0,          // 最小化以及关闭按钮
	MIN_MAX_BUTTON,          // 最小化，最大化以及关闭按钮
	ONLY_CLOSE_BUTTON        // 只有关闭按钮
};


// 自定义标题栏
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent = Q_NULLPTR);  // 要传递给父类的指针，默认为0，表示其依附于谁
	~TitleBar();

	// 设置标题栏的图标（就是左上角那个qq） 
	void setTitleIcon(const QString &filePath);
	// 设置标题栏的内容
	void setTitleContent(const QString& titleContext);
	// 设置标题栏的宽度
	void setTitleWidth(int width);
	// 设置标题栏的按钮类型
	void setButtonType(ButtonType buttonType);

	// 保存窗口最大化前的位置以及大小，方便我们最大化之后再变回去
	void saveRestoreInfo(const QPoint& point, const QSize& size);
	// 获取最大化之前保存的窗体状态数据，以便于恢复
	void getRestoreInfo(QPoint& point, QSize& size);

private:
	void paintEvent(QPaintEvent* event);							// 处理绘图事件
	void mouseDoubleClickEvent(QMouseEvent *event);               // 处理鼠标的双击事件
	void mousePressEvent(QMouseEvent* event);                        // 处理鼠标按下事件‘
	void mouseMoveEvent(QMouseEvent* event);                        // 处理鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event);                     // 处理鼠标释放事件

	/*
	   标题栏的最小化和最大化，关闭等等都是按钮，其实都是控件，需要初始化
	*/
	void initControl();              // 初始化控件
	void initConnections();          // 初始化信号与槽的链接
	void loadStyleSheet(const QString& sheetName);  // 加载样式表(设置控件的一种方式)

signals:
	/*
	   标题栏按钮被点击的时候触发的信号
	*/
	void signalButtonMinClicked();      // 最小化按钮被点击
	void signalButtonRestoreClicked();  // 最大化按钮还原被点击
	void signalButtonMaxClicked();	    // 最大化按钮被点击
	void signalButtonCloseClicked();    // 关闭按钮被点击

private slots:
	/*
	   响应信号的槽方法
	*/
	void onButtonMinClicked();
	void onButtonMaxClicked();
	void onButtonRestoreClicked();
	void onButtonCloseClicked();

private:
	/*
	  定义一些变量，用来创建需要显示的控件
	*/
	// 标签
	QLabel* m_pIcon;           // 标题栏标题图标
	QLabel* m_pTitleContent;   // 标题栏的内容

	// 按钮
	QPushButton* m_pButtonMin;     // 最小化按钮
	QPushButton* m_pButtonMax;     // 最大化按钮
	QPushButton* m_pButtonRestore; // 最大化恢复按钮
	QPushButton* m_pButtonClose;   // 关闭按钮		

	// 保存最大化之前窗口的的位置和大小，记录数据方便还原
	QPoint m_MaxRestorePoint;  // 最大化之前的位置                        
	QSize m_MaxRestoreSize;    // 最大化之前的大小

	bool m_isPressed;          // 用来判断标题栏是否被点击
	QPoint m_startMovePos;     // 记录开始移动的位置

	QString m_titleContent;   // 标题栏内容
	ButtonType m_buttonType;  // 标题栏窗口的按钮类型
};
