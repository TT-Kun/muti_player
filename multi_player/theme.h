#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QMenu>
#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidget>
#include <QSettings>
#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include <QActionGroup>
#include "titlebar.h"

class Theme : public QObject
{
    Q_OBJECT

public:
    static Theme& instance();
    
    enum ThemeType {
        Light,
        Dark,
        System
    };
    
    // 主题应用方法
    void applyTheme(const QString &themeName, QWidget *mainWindow);
    void setupThemeMenu(QMenu *themeMenu, QObject *receiver, const char *slot);
    void setupPlaylistStyle(QListWidget *playlistWidget);
    
    // 视频区域背景处理
    void updateVideoAreaBackground(QVideoWidget *videoWidget, 
                                 QMediaPlayer *mediaPlayer,
                                 const QString &currentTheme);
    
    // 获取当前主题
    QString getCurrentTheme() const { return currentTheme; }
    
    // 系统主题相关函数
    bool isSystemThemeTracking() const { return systemThemeTracking; }
    void enableSystemThemeTracking(QWidget *mainWindow);
    void disableSystemThemeTracking();
    
public slots:
    // 系统主题变化处理槽
    void handleSystemThemeChange(const QString &interfaceName, 
                              const QVariantMap &changedProps,
                              const QStringList &invalidatedProps);
signals:
     void change_tepy(bool isDarkTheme);
public:
         QString getThemeFunction();
private:
    Theme();
    ~Theme();
    Theme(const Theme&) = delete;
    Theme& operator=(const Theme&) = delete;

    QString currentTheme;
    bool systemThemeTracking = false;
    QWidget *targetWindow = nullptr;
    
    // DBus相关方法
    void setupDBusConnection();
    void updateSystemTheme();
    
    // 不同主题的样式表
    QString getLightThemeStyleSheet() const;
    QString getLightThemeStyleSheet_notplay() const;
    QString getDarkThemeStyleSheet() const;
    QString getDarkThemeStyleSheet_notplay() const;
};

#endif // THEME_H
