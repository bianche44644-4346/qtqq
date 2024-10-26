#include "ContactItem.h"

ContactItem::ContactItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initControl();
}

ContactItem::~ContactItem()
{}

void ContactItem::setUserName(const QString & userName)
{
	ui.userName->setText(userName);
}

void ContactItem::setSignName(const QString& signName)
{
	ui.signName->setText(signName);
}

// ����ͷ��
void ContactItem::setPixmap(const QPixmap& headPath)
{
	ui.label->setPixmap(headPath);
}

QString ContactItem::getUserName() const
{
	return ui.userName->text();              // ����userName��ǩ���ı�
}

QSize ContactItem::getHeadLabelSize() const
{
	return ui.label->size();                 // ����ͷ���ǩ�ĳߴ�
}

void ContactItem::initControl()
{

}
