#include "TitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFile>

#define BUTTON_HEIGHT 27                       // 按钮高度
#define BUTTON_WIDTH  27					   // 按钮宽度
#define TITLE_HEIGHT  27                       // 标题栏高度
/*不设置标题栏的宽度是因为其大小政策为在宽度上根据拉伸自适应*/

/*
   自定义标题栏:   在构造标题栏的时候，我们给标题栏的属性设置一些默认值
   m_isPressed: false			 默认不被点击
   m_buttonType: MIN_MAX_BUTTON  默认使用当前这种按钮风格
*/
TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
	, m_isPressed(false)
	, m_buttonType(MIN_MAX_BUTTON)
{
	// 在构造函数当中，初始化控件
	initControl();
	// 初始化信号和槽的链接
	initConnections();
	// 加载样式表  标题样式表的名称设置为Title，将样式表写在文件当中，使用样式表.css写在文件中
	// 我们此处最后将其放在Title.css的文件中去
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{

}

/*
*  初始化控件
**/
void TitleBar::initControl() {
	m_pIcon = new QLabel(this);            // 初始化标题图标控件
	m_pTitleContent = new QLabel(this);    // 初始化标题文本控件

	m_pButtonMin = new QPushButton(this);  // 初始化最小化按钮
	m_pButtonRestore = new QPushButton(this);  // 初始化恢复最大化前状态的按钮
	m_pButtonMax = new QPushButton(this);      // 初始化最大化按钮
	m_pButtonClose = new QPushButton(this);    // 初始化关闭按钮

	// 调用函数，设置每个按钮的固定大小，我们传入一个QSize的对象，我们上面已经使用宏定义了
	// 按钮的大小了，所以这里直接传入就可以了
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	// 给相应的对象设置对象名，调用对应的函数
	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	// 给标题栏来设置布局
	QHBoxLayout* myLayout = new QHBoxLayout(this);              // 对标题栏设置水平布局

	// 在布局中添加组件  水平布局，根据添加顺序从左到右，当然必须是一行上的
	myLayout->addWidget(m_pIcon);                    // 最左边添加图标
	myLayout->addWidget(m_pTitleContent);            // 之后添加标题内容
	myLayout->addWidget(m_pButtonMin);
	myLayout->addWidget(m_pButtonRestore);
	myLayout->addWidget(m_pButtonMax);
	myLayout->addWidget(m_pButtonClose);

	//  参数分别是设置上下左右的间隙
	myLayout->setContentsMargins(5, 0, 0, 0);         // 设置水平布局的间隙
	myLayout->setSpacing(0);                          // 设置布局里部件间的间隙

	/* 设置标题栏的尺寸策略: 第一个参数: 在横向上，此处设置为延伸(根据拉伸适应)
	*                        第二个参数: 在纵向上，此处设置为固定(就是拉伸器纵向宽度不变)
	*/
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);                     // 使用我们固定的宏，设定标题的固定高度
	setWindowFlags(Qt::FramelessWindowHint);          // 设置窗体的样式为无边框的风格
}

/*
	初始化信号与槽
*/
void TitleBar::initConnections() {
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));   // 此处的this就表示的是标题栏
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}


/*
  设置标题栏的图标

  需要传入一个字符串指定的路径，就是图标所在的路径
*/
void TitleBar::setTitleIcon(const QString& filePath) {
	QPixmap titleIcon(filePath);                                // 创建一个QPixmap根据传入的图标路径设置图标大小

	m_pIcon->setFixedSize(titleIcon.size());                    // 设置我们图标对象的固定大小(以我们刚才创建的图标大小为准)
	m_pIcon->setPixmap(titleIcon);                              // 然后给我们指定的标题栏控件设置图标
}

/*
  设置标题栏内容
*/
void TitleBar::setTitleContent(const QString& titleContent) {
	m_pTitleContent->setText(titleContent);                         // 在对应的位置设置标题栏内容
	m_titleContent = titleContent;                                  // 将标题的内容更新
}

/*
  设置标题栏的长度   -- 用于在拉伸窗口的时候更新标题栏的长度
*/
void TitleBar::setTitleWidth(int width) {
	setFixedWidth(width);                                             // 设置标题栏的长度
}

/*
  设置标题栏按钮类型
*/
void TitleBar::setButtonType(ButtonType buttonType) {
	m_buttonType = buttonType;                                          // 对按钮的类型进行记录

	switch (buttonType) {
	case MIN_BUTTON:
	{
		m_pButtonRestore->setVisible(false);                            // 按钮类型为，只有缩小的和关闭的，那么就将别的设置为不可见就就行
		m_pButtonMax->setVisible(false);
	}
	break;
	case MIN_MAX_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
	}
	break;
	case ONLY_CLOSE_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
	}
	break;
	default:
		break;
	}
}

/*
  保存窗口最大化前，窗口的位置和大小
*/
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size) {
	m_MaxRestorePoint = point;                       // 将返回的位置，记录在我们定义的变量中
	m_MaxRestoreSize = size;
}

/*
  获取窗口最大化前窗口的位置以及大小
*/
void TitleBar::getRestoreInfo(QPoint& point, QSize& size) {
	point = m_MaxRestorePoint;
	size = m_MaxRestoreSize;
}

/*
  绘制标题栏 -- 窗口的标题栏发生任何的改变，都会触发重绘制(别的也一样)
*/
void TitleBar::paintEvent(QPaintEvent* event) {
	// 设置背景色
	QPainter painter(this);                                 // 获取一个画家
	QPainterPath pathBack;                                  // 绘图路径，用来设置背景
	pathBack.setFillRule(Qt::WindingFill);                  // 设置填充规则

	//  添加圆角矩形到绘图路径    位置   宽      高        x和y的弧度
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true); // 设置一个平滑的映射转换

	/*
	   如果发生拉伸，我们需要对标题栏进行重新设置大小，如何判断是否拉伸呢?
	   通过比较当前的宽度和其父部件的宽度比较，如果不相等，那么就进行变化，
	   parentWidget()返回父部件

	   并且窗口最大化和最小化，发生变化
	*/
	if (width() != parentWidget()->width()) {
		setFixedWidth(parentWidget()->width());
	}

	QWidget::paintEvent(event);   //让基类处理剩余操作
}

/*
   双击响应时间，主要实现双击标题栏，最大化最小化的操作
*/
void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {

	// 只有存在最大化，最小化按钮才有效
	if (m_buttonType == MIN_MAX_BUTTON) {
		if (m_pButtonMax->isVisible()) {            // 最大化可视说明还没有处于最大化状态，不可视说明处于最大化状态
			onButtonMaxClicked();
		}
		else {
			onButtonRestoreClicked();
		}
	}

	return QWidget::mouseDoubleClickEvent(event);     // 默认的处理过程
}

/*
  通过鼠标按下，鼠标移动，鼠标释放事件实现拖动标题栏达到移动窗口效果
*/
void TitleBar::mousePressEvent(QMouseEvent* event) {
	if (m_buttonType == MIN_MAX_BUTTON) {                  // 有最小最大按钮的情况的时候
		// 在窗口最大化的状态下禁止拖动窗口
		// 就是我们鼠标点击的时候，就是要移动窗口，那么既然移动窗口，那么窗口最大的时候其实就没有必要移动
		// 所以我们设置只有窗口最大化按钮可视(也就是窗口没有最大化)，才会设置鼠标按下，才会有后续动作
		if (m_pButtonMax->isVisible()) {                   // 最大化按钮不可视那么就是处于最大化状态
			m_isPressed = true;
			m_startMovePos = event->globalPos();           //  返回鼠标按下事件发生的时候鼠标所在的全局位置
		}
	}
	else {
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}

	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
	// 首先只有当鼠标按下的时候我们才需要对其移动进行处理,没有按下操作是无意义的
	if (m_isPressed) {
		QPoint movePos = event->globalPos() - m_startMovePos;    // 事件触发的时候鼠标的位置-鼠标开始的位置，就可以获得鼠标要移动多少
		QPoint widgetPos = parentWidget()->pos();                  // 获取部件当前所在的位置，通过返回父部件返回当前的位置
		m_startMovePos = event->globalPos();                     // 更新开始移动的位置

		/* 对窗口进行移动 移动的x坐标通过当前位置的x坐标+需要移动的x坐标，y坐标一样*/
		parentWidget()->move(widgetPos.x() + movePos.x(), widgetPos.y() + movePos.y());
	}

	// 返回默认的事件处理
	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
	m_isPressed = false;                          // 鼠标释放就没有再按下了
	return QWidget::mouseReleaseEvent(event);
}

/*
  加载样式表
*/
void TitleBar::loadStyleSheet(const QString& sheetName) {
	// 所有的样式表，因为字符串比较多，所以我们将其写入到文件，所以我们从文件进行读取
	QFile file(":/Resources/QSS/" + sheetName + ".css");    // 拼接css的文件名

	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();       // 将标题栏的样式表进行保存
		// 将文件中的样式表全部合进来 使用这个函数的目的是其开销小一些
		styleSheet += QLatin1String(file.readAll());    // 一次性全部读取文件中的内容，然后添加到保存的样式表中
		this->setStyleSheet(styleSheet);                // 设置样式表
	}
}

/*
  按钮相应的槽函数实现
*/
void TitleBar::onButtonMinClicked() {
	emit signalButtonMinClicked();      // 反射一个信号，最小化的按钮被点击
}

void TitleBar::onButtonRestoreClicked() {
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);

	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked() {
	m_pButtonRestore->setVisible(true);
	m_pButtonMax->setVisible(false);

	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked() {
	emit signalButtonCloseClicked();
}
