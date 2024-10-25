#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget* parent)
	: QLabel(parent), m_rotation(0), m_hasArrow(hasArrow)
{
	// 设置固定高度
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);   // 水平扩展，垂直延生

	// 初始化属性动画
	// 参数:  第一个:  当前窗体   第二个:  给哪个添加动画
	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(100);         // 动画的时间，单位是ms，也就是对应ms内动画就完成了
	// 设置动画变化的曲线,动画缓和曲线类型 t^2从0加速
	m_animation->setEasingCurve(QEasingCurve::InQuad);
}

RootContactItem::~RootContactItem()
{}

void RootContactItem::setText(const QString & text)
{
	m_titleText = text;
	update();                  // 文本改变，触发界面重绘，将新的数据绘制上去
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand) {  // 如果展开，那么设置动画属性的结束值
		m_animation->setEndValue(90);    // 设置值为90，就是展开的话，箭头会向下旋转90度
	}
	else {
		m_animation->setEndValue(0);     // 箭头向下旋转0度
	}

	// 设置完成之后，将动画启动起来
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();                         // 界面有变化就进行重绘
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);    // 在当前部件上进行绘制
	painter.setRenderHint(QPainter::TextAntialiasing);   // 渲染文本, 参数: 文本防锯齿(画出来的文本好看一些)

	QFont font;  // 设置字体
	font.setPointSize(10);
	painter.setFont(font);
	// 绘制文本，参数 x，y，宽度()，      高度(与部件一样高)  左对齐并且垂直居中对齐            要绘制的文本
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);    // 以平滑的方式进行转换
	painter.save();     // 对设置的进行保存,防止创建别的画家之后将其的设置覆盖

	// 判断是否有箭头，绘制的时候
	if (m_hasArrow) {
		// 有的话就获取一个位图
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");  // 加载图片

		QPixmap tmpPixmap(pixmap.size());      // 中间位图和上面的大小一样
		tmpPixmap.fill(Qt::transparent);       // 填充透明

		// 创建新画家，在临时位图上画
		QPainter painter1(&tmpPixmap);
		painter1.setRenderHint(QPainter::SmoothPixmapTransform, true);

		// 对坐标系进行偏移
		// 参数:          坐标系的偏移到设置图片的中间，参数就是x和y方向上的偏移
		painter1.translate(pixmap.width() / 2, pixmap.height() / 2);
		// 坐标系旋转多少度(顺时针旋转坐标系)
		painter1.rotate(m_rotation);
		// 旋转之后绘制图片， 在对应的位置进行绘制，因为左边在前面偏移了一半，所以此时
		// 我们绘制的应该向前面绘制一半
		painter1.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

		// 上面，就在tmpPixmap上画了一个旋转对应角度的图标(箭头)，因为是绘制到了临时的图标上，我们需要绘制到原来的上面
		painter.drawPixmap(6, ((height() - pixmap.height()) / 2), tmpPixmap);

		// 如果无法恢复的时候，我们在不展开的情况下，我们又得需要将箭头的位置转移回去
		// 上面我们保存了之前为展开的状态，这里我们直接使用Qt提供的接口就可以恢复回去了
		// 不需要自己实现了转回去了

		painter.restore();   // 将当前画家恢复到我们之前使用save()保存起来的一些设置
	}

	QLabel::paintEvent(event);          // 处理其它的绘图事件
}
