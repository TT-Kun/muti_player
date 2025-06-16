/********************************************************************************
** Form generated from reading UI file 'titlebar.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TITLEBAR_H
#define UI_TITLEBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TitleBar
{
public:
    QHBoxLayout *horizontalLayout;
    QSpacerItem *leftSpacer;
    QLabel *titleLabel;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;

    void setupUi(QWidget *TitleBar)
    {
        if (TitleBar->objectName().isEmpty())
            TitleBar->setObjectName("TitleBar");
        TitleBar->resize(800, 30);
        TitleBar->setMinimumSize(QSize(0, 30));
        TitleBar->setMaximumSize(QSize(16777215, 30));
        horizontalLayout = new QHBoxLayout(TitleBar);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        leftSpacer = new QSpacerItem(90, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(leftSpacer);

        titleLabel = new QLabel(TitleBar);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(9);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(titleLabel);

        minimizeButton = new QPushButton(TitleBar);
        minimizeButton->setObjectName("minimizeButton");
        minimizeButton->setMinimumSize(QSize(30, 28));
        minimizeButton->setMaximumSize(QSize(30, 28));

        horizontalLayout->addWidget(minimizeButton);

        maximizeButton = new QPushButton(TitleBar);
        maximizeButton->setObjectName("maximizeButton");
        maximizeButton->setMinimumSize(QSize(30, 28));
        maximizeButton->setMaximumSize(QSize(30, 28));

        horizontalLayout->addWidget(maximizeButton);

        closeButton = new QPushButton(TitleBar);
        closeButton->setObjectName("closeButton");
        closeButton->setMinimumSize(QSize(30, 28));
        closeButton->setMaximumSize(QSize(30, 28));

        horizontalLayout->addWidget(closeButton);


        retranslateUi(TitleBar);

        QMetaObject::connectSlotsByName(TitleBar);
    } // setupUi

    void retranslateUi(QWidget *TitleBar)
    {
        TitleBar->setWindowTitle(QCoreApplication::translate("TitleBar", "\346\240\207\351\242\230\346\240\217", nullptr));
        titleLabel->setText(QCoreApplication::translate("TitleBar", "\347\250\213\345\272\217\345\221\230\350\200\201\345\273\226-\347\201\253\350\212\261\346\222\255\346\224\276\345\231\250", nullptr));
#if QT_CONFIG(tooltip)
        minimizeButton->setToolTip(QCoreApplication::translate("TitleBar", "\346\234\200\345\260\217\345\214\226", nullptr));
#endif // QT_CONFIG(tooltip)
        minimizeButton->setText(QString());
#if QT_CONFIG(tooltip)
        maximizeButton->setToolTip(QCoreApplication::translate("TitleBar", "\346\234\200\345\244\247\345\214\226", nullptr));
#endif // QT_CONFIG(tooltip)
        maximizeButton->setText(QString());
#if QT_CONFIG(tooltip)
        closeButton->setToolTip(QCoreApplication::translate("TitleBar", "\345\205\263\351\227\255", nullptr));
#endif // QT_CONFIG(tooltip)
        closeButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TitleBar: public Ui_TitleBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TITLEBAR_H
