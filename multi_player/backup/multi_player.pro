QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 添加Windows库链接（合并为一个win32块）
win32 {
    LIBS += -luser32 -lgdi32
    LIBS += -ldwmapi
}

SOURCES += \
    MyBorderContainer.cpp \
    main.cpp \
    mediaControl.cpp \
    menuControl.cpp \
    player.cpp \
    playlist_manager.cpp \
    titleBar.cpp \
    volumecontrol.cpp

HEADERS += \
    MyBorderContainer.h \
    clickableslider.h \
    mediaControl.h \
    menuControl.h \
    player.h \
    playlist_manager.h \
    titleBar.h \
    volumecontrol.h

FORMS += \
    player.ui \
    titlebar.ui \
    volumecontrol.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc 