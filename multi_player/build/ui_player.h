/********************************************************************************
** Form generated from reading UI file 'player.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYER_H
#define UI_PLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "clickableslider.h"

QT_BEGIN_NAMESPACE

class Ui_Player
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QVBoxLayout *videoLayout;
    QWidget *controlWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *playButton;
    QPushButton *stopButton;
    QPushButton *backwardButton;
    QPushButton *forwardButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QLabel *currentTimeLabel;
    ClickableSlider *progressSlider;
    QLabel *totalTimeLabel;
    QPushButton *volumeButton;
    QPushButton *playModeButton;
    QPushButton *togglePlaylistButton;
    QPushButton *settingsButton;
    QMenuBar *menubar;

    void setupUi(QMainWindow *Player)
    {
        if (Player->objectName().isEmpty())
            Player->setObjectName("Player");
        Player->resize(913, 632);
        centralwidget = new QWidget(Player);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        videoLayout = new QVBoxLayout();
        videoLayout->setSpacing(0);
        videoLayout->setObjectName("videoLayout");
        videoLayout->setContentsMargins(-1, -1, -1, 0);

        verticalLayout->addLayout(videoLayout);

        controlWidget = new QWidget(centralwidget);
        controlWidget->setObjectName("controlWidget");
        controlWidget->setMinimumSize(QSize(0, 40));
        controlWidget->setMaximumSize(QSize(16777215, 40));
        controlWidget->setStyleSheet(QString::fromUtf8("QWidget#controlWidget {\n"
"  background-color: rgba(0, 0, 0, 180);\n"
"}\n"
"QLabel {\n"
"  color: white;\n"
"}\n"
"QPushButton {\n"
"  border: none;\n"
"  padding: 1px;\n"
"}\n"
"QPushButton:hover {\n"
"  background-color: rgba(255, 255, 255, 30);\n"
"}\n"
"QSlider::groove:horizontal {\n"
"  height: 3px;\n"
"  background: #999999;\n"
"}\n"
"QSlider::handle:horizontal {\n"
"  background: white;\n"
"  width: 10px;\n"
"  margin: -4px 0;\n"
"  border-radius: 5px;\n"
"}\n"
"QSlider::sub-page:horizontal {\n"
"  background: #2196F3;\n"
"}"));
        horizontalLayout = new QHBoxLayout(controlWidget);
        horizontalLayout->setSpacing(4);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(8, 0, 8, 0);
        playButton = new QPushButton(controlWidget);
        playButton->setObjectName("playButton");
        playButton->setMinimumSize(QSize(32, 32));
        playButton->setMaximumSize(QSize(32, 32));
        playButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(playButton);

        stopButton = new QPushButton(controlWidget);
        stopButton->setObjectName("stopButton");
        stopButton->setMinimumSize(QSize(32, 32));
        stopButton->setMaximumSize(QSize(32, 32));
        stopButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(stopButton);

        backwardButton = new QPushButton(controlWidget);
        backwardButton->setObjectName("backwardButton");
        backwardButton->setMinimumSize(QSize(32, 32));
        backwardButton->setMaximumSize(QSize(32, 32));
        backwardButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(backwardButton);

        forwardButton = new QPushButton(controlWidget);
        forwardButton->setObjectName("forwardButton");
        forwardButton->setMinimumSize(QSize(32, 32));
        forwardButton->setMaximumSize(QSize(32, 32));
        forwardButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(forwardButton);

        previousButton = new QPushButton(controlWidget);
        previousButton->setObjectName("previousButton");
        previousButton->setMinimumSize(QSize(32, 32));
        previousButton->setMaximumSize(QSize(32, 32));
        previousButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(previousButton);

        nextButton = new QPushButton(controlWidget);
        nextButton->setObjectName("nextButton");
        nextButton->setMinimumSize(QSize(32, 32));
        nextButton->setMaximumSize(QSize(32, 32));
        nextButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(nextButton);

        currentTimeLabel = new QLabel(controlWidget);
        currentTimeLabel->setObjectName("currentTimeLabel");
        currentTimeLabel->setMinimumSize(QSize(45, 0));
        currentTimeLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(currentTimeLabel);

        progressSlider = new ClickableSlider(controlWidget);
        progressSlider->setObjectName("progressSlider");
        progressSlider->setMinimumSize(QSize(100, 0));
        progressSlider->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(progressSlider);

        totalTimeLabel = new QLabel(controlWidget);
        totalTimeLabel->setObjectName("totalTimeLabel");
        totalTimeLabel->setMinimumSize(QSize(45, 0));

        horizontalLayout->addWidget(totalTimeLabel);

        volumeButton = new QPushButton(controlWidget);
        volumeButton->setObjectName("volumeButton");
        volumeButton->setMinimumSize(QSize(32, 32));
        volumeButton->setMaximumSize(QSize(32, 32));
        volumeButton->setFocusPolicy(Qt::NoFocus);
        volumeButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                border: none;\n"
"                padding: 1px;\n"
"            }\n"
"            QPushButton:hover {\n"
"                background-color: rgba(255, 255, 255, 30);\n"
"            }"));

        horizontalLayout->addWidget(volumeButton);

        playModeButton = new QPushButton(controlWidget);
        playModeButton->setObjectName("playModeButton");
        playModeButton->setMinimumSize(QSize(32, 32));
        playModeButton->setMaximumSize(QSize(32, 32));
        playModeButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(playModeButton);

        togglePlaylistButton = new QPushButton(controlWidget);
        togglePlaylistButton->setObjectName("togglePlaylistButton");
        togglePlaylistButton->setMinimumSize(QSize(32, 32));
        togglePlaylistButton->setMaximumSize(QSize(32, 32));
        togglePlaylistButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(togglePlaylistButton);

        settingsButton = new QPushButton(controlWidget);
        settingsButton->setObjectName("settingsButton");
        settingsButton->setMinimumSize(QSize(32, 32));
        settingsButton->setMaximumSize(QSize(32, 32));
        settingsButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(settingsButton);


        verticalLayout->addWidget(controlWidget);

        Player->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Player);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 913, 18));
        menubar->setVisible(false);
        Player->setMenuBar(menubar);

        retranslateUi(Player);

        QMetaObject::connectSlotsByName(Player);
    } // setupUi

    void retranslateUi(QMainWindow *Player)
    {
        Player->setWindowTitle(QCoreApplication::translate("Player", "\350\247\206\351\242\221\346\222\255\346\224\276\345\231\250", nullptr));
        playButton->setText(QString());
        stopButton->setText(QString());
        backwardButton->setText(QString());
        forwardButton->setText(QString());
#if QT_CONFIG(tooltip)
        previousButton->setToolTip(QCoreApplication::translate("Player", "\344\270\212\344\270\200\344\270\252", nullptr));
#endif // QT_CONFIG(tooltip)
        previousButton->setText(QString());
#if QT_CONFIG(tooltip)
        nextButton->setToolTip(QCoreApplication::translate("Player", "\344\270\213\344\270\200\344\270\252", nullptr));
#endif // QT_CONFIG(tooltip)
        nextButton->setText(QString());
        currentTimeLabel->setText(QCoreApplication::translate("Player", "00:00", nullptr));
        totalTimeLabel->setText(QCoreApplication::translate("Player", "00:00", nullptr));
#if QT_CONFIG(tooltip)
        volumeButton->setToolTip(QCoreApplication::translate("Player", "\351\235\231\351\237\263", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        playModeButton->setToolTip(QCoreApplication::translate("Player", "\346\222\255\346\224\276\346\250\241\345\274\217", nullptr));
#endif // QT_CONFIG(tooltip)
        playModeButton->setText(QString());
#if QT_CONFIG(tooltip)
        togglePlaylistButton->setToolTip(QCoreApplication::translate("Player", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\222\255\346\224\276\345\210\227\350\241\250", nullptr));
#endif // QT_CONFIG(tooltip)
        togglePlaylistButton->setText(QString());
#if QT_CONFIG(tooltip)
        settingsButton->setToolTip(QCoreApplication::translate("Player", "\350\256\276\347\275\256", nullptr));
#endif // QT_CONFIG(tooltip)
        settingsButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Player: public Ui_Player {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_H
