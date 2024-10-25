#pragma once
#include <QSize>
#include <QPixmap>
#include <QProxyStyle>

/*这个自定义类比较简单就直接在这里写了， 目的：  改变默认的部件风格*/
class CustomProxyStyle :public QProxyStyle
{
public:
	CustomProxyStyle(QObject* parent) { setParent(parent); }
	CustomProxyStyle() {}
	~CustomProxyStyle() {}
public:
	/*重写相应方法*/
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = Q_NULLPTR) const override {
		if (PE_FrameFocusRect == element) {
			// 去掉windows中部件默认的边框或者虚线框，部件获取焦点的时候直接返回,此处
			// 直接返回，不进行默认绘制
			/* 比如当我们选中一个位置使其获得焦点的时候，有些部件会默认存在一个边框，
			   要想去掉这个边框就得需要这里这个步骤，继承对应类，重写对应方法，其实
			   好多都会出现，比如选中文件，选中一行，只是风格不同，但是都会默认生成*/
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

/*此类用于与配置文件进行交互*/
class CommonUtils
{
public:
	CommonUtils();
public:
	/* 获取圆头像，因为圆头像也是从配置文件中读取的，所以这个方法应该可以读取*/
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));
	// 加载样式表              部件指针: 给那个部件添加样式表    样式表的名字
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
	// 设置默认的颜色
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDefaultSkinColor();
};

