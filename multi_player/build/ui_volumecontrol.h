/********************************************************************************
** Form generated from reading UI file 'volumecontrol.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VOLUMECONTROL_H
#define UI_VOLUMECONTROL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_volumeControl
{
public:
    QHBoxLayout *horizontalLayout;
    QPushButton *volumeIcon;
    QSlider *volumeSlider;
    QLabel *volumeValueLabel;

    void setupUi(QWidget *volumeControl)
    {
        if (volumeControl->objectName().isEmpty())
            volumeControl->setObjectName("volumeControl");
        volumeControl->resize(200, 34);
        volumeControl->setStyleSheet(QString::fromUtf8("QWidget#volumeControl { \n"
"  background-color: white; \n"
"  border-radius: 6px; \n"
"}"));
        horizontalLayout = new QHBoxLayout(volumeControl);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        volumeIcon = new QPushButton(volumeControl);
        volumeIcon->setObjectName("volumeIcon");
        volumeIcon->setMinimumSize(QSize(24, 24));
        volumeIcon->setMaximumSize(QSize(24, 24));
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("audio-volume-medium")));
        volumeIcon->setIcon(icon);
        volumeIcon->setFlat(true);

        horizontalLayout->addWidget(volumeIcon);

        volumeSlider = new QSlider(volumeControl);
        volumeSlider->setObjectName("volumeSlider");
        volumeSlider->setMinimumSize(QSize(100, 0));
        volumeSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(volumeSlider);

        volumeValueLabel = new QLabel(volumeControl);
        volumeValueLabel->setObjectName("volumeValueLabel");
        volumeValueLabel->setMinimumSize(QSize(30, 0));
        volumeValueLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(volumeValueLabel);


        retranslateUi(volumeControl);

        QMetaObject::connectSlotsByName(volumeControl);
    } // setupUi

    void retranslateUi(QWidget *volumeControl)
    {
        volumeControl->setWindowTitle(QCoreApplication::translate("volumeControl", "Form", nullptr));
        volumeIcon->setText(QString());
        volumeValueLabel->setText(QCoreApplication::translate("volumeControl", "50", nullptr));
    } // retranslateUi

};

namespace Ui {
    class volumeControl: public Ui_volumeControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VOLUMECONTROL_H
