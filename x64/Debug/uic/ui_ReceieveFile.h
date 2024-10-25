/********************************************************************************
** Form generated from reading UI file 'ReceieveFile.ui'
**
** Created by: Qt User Interface Compiler version 5.9.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECEIEVEFILE_H
#define UI_RECEIEVEFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReceieveFileClass
{
public:
    QWidget *titleWidget;
    QWidget *bodyWidget;
    QLabel *label;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    void setupUi(QWidget *ReceieveFileClass)
    {
        if (ReceieveFileClass->objectName().isEmpty())
            ReceieveFileClass->setObjectName(QStringLiteral("ReceieveFileClass"));
        ReceieveFileClass->resize(515, 160);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ReceieveFileClass->sizePolicy().hasHeightForWidth());
        ReceieveFileClass->setSizePolicy(sizePolicy);
        ReceieveFileClass->setMinimumSize(QSize(515, 160));
        ReceieveFileClass->setMaximumSize(QSize(515, 160));
        titleWidget = new QWidget(ReceieveFileClass);
        titleWidget->setObjectName(QStringLiteral("titleWidget"));
        titleWidget->setGeometry(QRect(0, 0, 515, 50));
        titleWidget->setProperty("titleskin", QVariant(true));
        bodyWidget = new QWidget(ReceieveFileClass);
        bodyWidget->setObjectName(QStringLiteral("bodyWidget"));
        bodyWidget->setGeometry(QRect(0, 50, 515, 110));
        bodyWidget->setProperty("bottomskin", QVariant(true));
        label = new QLabel(bodyWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(40, 20, 381, 16));
        okBtn = new QPushButton(bodyWidget);
        okBtn->setObjectName(QStringLiteral("okBtn"));
        okBtn->setGeometry(QRect(270, 70, 93, 28));
        cancelBtn = new QPushButton(bodyWidget);
        cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
        cancelBtn->setGeometry(QRect(400, 70, 93, 28));

        retranslateUi(ReceieveFileClass);

        QMetaObject::connectSlotsByName(ReceieveFileClass);
    } // setupUi

    void retranslateUi(QWidget *ReceieveFileClass)
    {
        ReceieveFileClass->setWindowTitle(QApplication::translate("ReceieveFileClass", "ReceieveFile", Q_NULLPTR));
        label->setText(QApplication::translate("ReceieveFileClass", "\346\235\245\350\207\252xxx\345\217\221\346\235\245\347\232\204\346\226\207\344\273\266,\346\230\257\345\220\246\346\216\245\346\224\266", Q_NULLPTR));
        okBtn->setText(QApplication::translate("ReceieveFileClass", "\347\241\256\345\256\232", Q_NULLPTR));
        cancelBtn->setText(QApplication::translate("ReceieveFileClass", "\345\217\226\346\266\210", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ReceieveFileClass: public Ui_ReceieveFileClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECEIEVEFILE_H
