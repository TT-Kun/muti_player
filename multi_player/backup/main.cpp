#include "player.h"
#include <QApplication>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QCoreApplication::setOrganizationName("MultiPlayer");
    QCoreApplication::setApplicationName("MultiPlayer");
    
    // QSettings存到.ini文件
    QString settingsPath = QCoreApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
    
    // 配置文件路径
    qputenv("MULTI_PLAYER_SETTINGS", settingsPath.toUtf8());
    
    Player w;
    w.show();
    return a.exec();
} 
