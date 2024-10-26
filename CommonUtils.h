#pragma once
#include <QSize>
#include <QPixmap>
#include <QProxyStyle>

/*����Զ�����Ƚϼ򵥾�ֱ��������д�ˣ� Ŀ�ģ�  �ı�Ĭ�ϵĲ������*/
class CustomProxyStyle :public QProxyStyle
{
public:
	CustomProxyStyle(QObject* parent) { setParent(parent); }
	CustomProxyStyle() {}
	~CustomProxyStyle() {}
public:
	/*��д��Ӧ����*/
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = Q_NULLPTR) const override {
		if (PE_FrameFocusRect == element) {
			// ȥ��windows�в���Ĭ�ϵı߿�������߿򣬲�����ȡ�����ʱ��ֱ�ӷ���,�˴�
			// ֱ�ӷ��أ�������Ĭ�ϻ���
			/* ���統����ѡ��һ��λ��ʹ���ý����ʱ����Щ������Ĭ�ϴ���һ���߿�
			   Ҫ��ȥ������߿�͵���Ҫ����������裬�̳ж�Ӧ�࣬��д��Ӧ��������ʵ
			   �ö඼����֣�����ѡ���ļ���ѡ��һ�У�ֻ�Ƿ��ͬ�����Ƕ���Ĭ������*/
			return;
		}
		else {
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};

/*���������������ļ����н���*/
class CommonUtils
{
public:
	CommonUtils();
public:
	/* ��ȡԲͷ����ΪԲͷ��Ҳ�Ǵ������ļ��ж�ȡ�ģ������������Ӧ�ÿ��Զ�ȡ*/
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize = QSize(0, 0));
	// ������ʽ��              ����ָ��: ���Ǹ����������ʽ��    ��ʽ�������
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
	// ����Ĭ�ϵ���ɫ
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDefaultSkinColor();
};

