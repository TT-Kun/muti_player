QT += core gui multimedia multimediawidgets quickwidgets network
QT +=  widgets dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    fileuploader_av.cpp \
    fileuploader_yin.cpp \
    main.cpp \
    mywindow.cpp \
    player.cpp \
    theme.cpp \
    titlebar.cpp

HEADERS += \
    FileHandler_read.h \
    clickableslider.h \
    fileuploader_av.h \
    fileuploader_yin.h \
    haha.h \
    mywindow.h \
    player.h \
    theme.h \
    titlebar.h \
    write.h

FORMS += \
    player.ui

RESOURCES += \
    qml.qrc \
    resources.qrc

DISTFILES += 