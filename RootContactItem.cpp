#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget* parent)
	: QLabel(parent), m_rotation(0), m_hasArrow(hasArrow)
{
	// ���ù̶��߶�
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);   // ˮƽ��չ����ֱ����

	// ��ʼ�����Զ���
	// ����:  ��һ��:  ��ǰ����   �ڶ���:  ���ĸ���Ӷ���
	m_animation = new QPropertyAnimation(this, "rotation");
	m_animation->setDuration(100);         // ������ʱ�䣬��λ��ms��Ҳ���Ƕ�Ӧms�ڶ����������
	// ���ö����仯������,���������������� t^2��0����
	m_animation->setEasingCurve(QEasingCurve::InQuad);
}

RootContactItem::~RootContactItem()
{}

void RootContactItem::setText(const QString & text)
{
	m_titleText = text;
	update();                  // �ı��ı䣬���������ػ棬���µ����ݻ�����ȥ
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand) {  // ���չ������ô���ö������ԵĽ���ֵ
		m_animation->setEndValue(90);    // ����ֵΪ90������չ���Ļ�����ͷ��������ת90��
	}
	else {
		m_animation->setEndValue(0);     // ��ͷ������ת0��
	}

	// �������֮�󣬽�������������
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();                         // �����б仯�ͽ����ػ�
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);    // �ڵ�ǰ�����Ͻ��л���
	painter.setRenderHint(QPainter::TextAntialiasing);   // ��Ⱦ�ı�, ����: �ı������(���������ı��ÿ�һЩ)

	QFont font;  // ��������
	font.setPointSize(10);
	painter.setFont(font);
	// �����ı������� x��y�����()��      �߶�(�벿��һ����)  ����벢�Ҵ�ֱ���ж���            Ҫ���Ƶ��ı�
	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);    // ��ƽ���ķ�ʽ����ת��
	painter.save();     // �����õĽ��б���,��ֹ������Ļ���֮��������ø���

	// �ж��Ƿ��м�ͷ�����Ƶ�ʱ��
	if (m_hasArrow) {
		// �еĻ��ͻ�ȡһ��λͼ
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");  // ����ͼƬ

		QPixmap tmpPixmap(pixmap.size());      // �м�λͼ������Ĵ�Сһ��
		tmpPixmap.fill(Qt::transparent);       // ���͸��

		// �����»��ң�����ʱλͼ�ϻ�
		QPainter painter1(&tmpPixmap);
		painter1.setRenderHint(QPainter::SmoothPixmapTransform, true);

		// ������ϵ����ƫ��
		// ����:          ����ϵ��ƫ�Ƶ�����ͼƬ���м䣬��������x��y�����ϵ�ƫ��
		painter1.translate(pixmap.width() / 2, pixmap.height() / 2);
		// ����ϵ��ת���ٶ�(˳ʱ����ת����ϵ)
		painter1.rotate(m_rotation);
		// ��ת֮�����ͼƬ�� �ڶ�Ӧ��λ�ý��л��ƣ���Ϊ�����ǰ��ƫ����һ�룬���Դ�ʱ
		// ���ǻ��Ƶ�Ӧ����ǰ�����һ��
		painter1.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, pixmap);

		// ���棬����tmpPixmap�ϻ���һ����ת��Ӧ�Ƕȵ�ͼ��(��ͷ)����Ϊ�ǻ��Ƶ�����ʱ��ͼ���ϣ�������Ҫ���Ƶ�ԭ��������
		painter.drawPixmap(6, ((height() - pixmap.height()) / 2), tmpPixmap);

		// ����޷��ָ���ʱ�������ڲ�չ��������£������ֵ���Ҫ����ͷ��λ��ת�ƻ�ȥ
		// �������Ǳ�����֮ǰΪչ����״̬����������ֱ��ʹ��Qt�ṩ�ĽӿھͿ��Իָ���ȥ��
		// ����Ҫ�Լ�ʵ����ת��ȥ��

		painter.restore();   // ����ǰ���һָ�������֮ǰʹ��save()����������һЩ����
	}

	QLabel::paintEvent(event);          // ���������Ļ�ͼ�¼�
}
