#include "TalkWindowItem.h"
#include "CommonUtils.h"

TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initControl();
}

TalkWindowItem::~TalkWindowItem()
{}

// ʹ�����ݿ�����޸�
void TalkWindowItem::setHeadPixmap(const QPixmap& pix/*const QString & pixmap*/)
{
	// ��Ϊ��Բͷ��������Ҫ��ȡһ��Դͷ��
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");

	// ���ݿ�������޸�
//	QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");   // ����д���ȿ�һ��Ч��

	// ʹ�����ݿ�֮������޸ģ�ֱ��ʹ�ô����pix
	const QPixmap& headpixmap = CommonUtils::getRoundImage(pix, mask, ui.headlabel->size());
	// const QPixmap& headpixmap = CommonUtils::getRoundImage(head, mask, ui.headlabel->size());
	ui.headlabel->setPixmap(headpixmap);
}

void TalkWindowItem::setMsgLabelContent(const QString& msg)
{
	// ��������
	ui.msgLabel->setText(msg);
}

QString TalkWindowItem::getMsgLabelText()
{
	// ��ȡlabel��ǩ�е�����
	return ui.msgLabel->text();
}

void TalkWindowItem::initControl()
{
	ui.tCloseBtn->setVisible(false);     // �رհ�ť�ʼ�ǲ��ɼ��ģ���������Ϊfalse

	// ��Ϊͨ����ǰ�Ĵ����޷�ʵ�ֽ����촰�ڽ��йرգ������������������ӵ���ǰ�Ĵ��ڵ�
	// ʱ��ȥ���òۺ���������ȥ����һ���źţ��������촰�ڣ�Ȼ�����촰�ڽ��йرռ���
	connect(ui.tCloseBtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}

void TalkWindowItem::enterEvent(QEvent* event)
{
	// һ�����ǽ��뵽�������ô��ť�Ϳɼ��Ϳ�����
	ui.tCloseBtn->setVisible(true);
	__super::enterEvent(event);      // ����������Ĭ�ϲ������¼�
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	// �뿪������رհ�ť����ʾ
	ui.tCloseBtn->setVisible(false);
	__super::leaveEvent(event);
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	// �ߴ�����
	__super::resizeEvent(event);
}
