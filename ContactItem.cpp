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

// 设置头像
void ContactItem::setPixmap(const QPixmap& headPath)
{
	ui.label->setPixmap(headPath);
}

QString ContactItem::getUserName() const
{
	return ui.userName->text();              // 返回userName标签的文本
}

QSize ContactItem::getHeadLabelSize() const
{
	return ui.label->size();                 // 返回头像标签的尺寸
}

void ContactItem::initControl()
{

}
