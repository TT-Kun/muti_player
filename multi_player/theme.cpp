#include "theme.h"
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>

Theme& Theme::instance()
{
    static Theme instance;
    return instance;
}

Theme::Theme() : currentTheme("dark"), systemThemeTracking(false), targetWindow(nullptr)
{

}

Theme::~Theme()
{
}

void Theme::applyTheme(const QString &themeName, QWidget *mainWindow)
{
    QString styleSheet;

    if (themeName == "light") {
        styleSheet = getLightThemeStyleSheet();

        qDebug()<<"using light theme";
    }
    else if (themeName == "dark") {
        styleSheet = getDarkThemeStyleSheet();

        qDebug()<<"using dark theme";
    }
    if (!styleSheet.isEmpty() && mainWindow) {
        mainWindow->setStyleSheet(styleSheet);
        currentTheme = themeName;

        // 保存主题设置
        QSettings settings;
        settings.setValue("theme", themeName);
        
        // 系统主题跟踪设置为false
        if (themeName != "system") {
            settings.setValue("systemThemeTracking", false);
            systemThemeTracking = false;
        }
    }
}

void Theme::setupThemeMenu(QMenu *themeMenu, QObject *receiver, const char *slot)
{
    if (!themeMenu) return;

    QActionGroup *themeGroup = new QActionGroup(themeMenu);
    
    // 从设置中读取当前主题和系统跟踪状态
    QSettings settings;
    QString savedTheme = settings.value("theme", "dark").toString();
    bool isSystemTracking = settings.value("systemThemeTracking", false).toBool();

    // 添加预设主题
    struct ThemeInfo {
        QString name;
        QString id;
    };

    QList<ThemeInfo> themes = {
        {"浅色主题", "light"},
        {"深色主题", "dark"},
        {"跟随系统", "system"}
    };

    for (const auto &theme : themes) {
        QAction *action = themeMenu->addAction(theme.name);
        action->setCheckable(true);
        action->setData(theme.id);
        themeGroup->addAction(action);
        
        // 根据设置选中相应选项
        if ((isSystemTracking && theme.id == "system") || 
            (!isSystemTracking && theme.id == savedTheme)) {
            action->setChecked(true);
        }
    }

    // 连接信号
    QObject::connect(themeGroup, SIGNAL(triggered(QAction*)), 
                    receiver, slot);
}

void Theme::setupPlaylistStyle(QListWidget *playlistWidget)
{
    if (!playlistWidget) return;
    
    playlistWidget->setStyleSheet(R"(
        QListWidget {
            background-color: #2b2b2b;
            border: none;
        }
        QListWidget::item {
            color: #ffffff;
            padding: 4px;
            border-bottom: 1px solid #3a3a3a;
        }
        QListWidget::item:selected {
            background-color: #3a3a3a;
        }
        QListWidget::item:hover {
            background-color: #323232;
        }
    )");
}

void Theme::enableSystemThemeTracking(QWidget *mainWindow)
{
    // 保存目标窗口
    targetWindow = mainWindow;
    systemThemeTracking = true;
    
    // 保存设置
    QSettings settings;
    settings.setValue("systemThemeTracking", true);
    settings.setValue("theme", "system");
    
    // 初始化DBus连接
    setupDBusConnection();
    
    // 立即更新一次主题
    updateSystemTheme();
}

void Theme::disableSystemThemeTracking()
{
    systemThemeTracking = false;
    
    // 保存设置
    QSettings settings;
    settings.setValue("systemThemeTracking", false);
    
    // 可以在这里断开DBus连接，但通常不需要，因为单例实例会保持连接
}

void Theme::setupDBusConnection()
{
    const QString DBUS_SERVICE = "com.deepin.daemon.Appearance";
    const QString DBUS_PATH = "/com/deepin/daemon/Appearance";
    const QString DBUS_INTERFACE = "com.deepin.daemon.Appearance";
    const QString PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";
    const QString PROPERTY_GTK_THEME = "GtkTheme";

    // 连接DBus信号
    QDBusConnection::sessionBus().connect(
        DBUS_SERVICE,
        DBUS_PATH,
        PROPERTIES_INTERFACE,
        "PropertiesChanged",
        this,
        SLOT(handleSystemThemeChange(QString, QVariantMap, QStringList))
    );
}

void Theme::updateSystemTheme()
{
    if (!targetWindow) return;
    
    const QString DBUS_SERVICE = "com.deepin.daemon.Appearance";
    const QString DBUS_PATH = "/com/deepin/daemon/Appearance";
    const QString DBUS_INTERFACE = "com.deepin.daemon.Appearance";
    const QString PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";
    const QString PROPERTY_GTK_THEME = "GtkTheme";

    QDBusMessage msg = QDBusMessage::createMethodCall(
        DBUS_SERVICE,
        DBUS_PATH,
        PROPERTIES_INTERFACE,
        "Get"
    );

    msg << DBUS_INTERFACE << PROPERTY_GTK_THEME;
    QDBusReply<QVariant> reply = QDBusConnection::sessionBus().call(msg);

     qDebug() << "系统主题:啊手动阀玛纳斯，的。 " ;
    if (reply.isValid()) {
        QString theme = reply.value().toString().toLower();
        qDebug() << "系统主题: " << theme;
        
        if(theme.contains("dark")) {
            applyTheme("dark", targetWindow);
            setupPlaylistStyle(targetWindow->findChild<QListWidget*>());
            currentTheme = "dark";


             emit change_tepy(false);
            //zhelizheli

        } else {
            applyTheme("light", targetWindow);
            if (QListWidget* playlist = targetWindow->findChild<QListWidget*>()) {
                playlist->setStyleSheet("");
            }
            //zhelizheli
            currentTheme = "light";
             emit change_tepy(true);

        }
        
        // 重新设置为系统主题标志
        systemThemeTracking = true;
        QSettings settings;
        settings.setValue("systemThemeTracking", true);
        settings.setValue("theme", "system");
    } else {
        qDebug() << "获取系统主题失败: " << reply.error().message();
    }
}

void Theme::handleSystemThemeChange(const QString &interfaceName, 
                                  const QVariantMap &changedProps,
                                  const QStringList &invalidatedProps)
{
    qDebug() << "系统主题变化:" << interfaceName;
    qDebug() << "变化属性:" << changedProps.keys();
    
    const QString DBUS_INTERFACE = "com.deepin.daemon.Appearance";
    const QString PROPERTY_GTK_THEME = "GtkTheme";
    
    if (interfaceName == DBUS_INTERFACE && 
        changedProps.contains(PROPERTY_GTK_THEME) &&
        systemThemeTracking) {
        qDebug() << "检测到系统主题变化，更新主题";
        updateSystemTheme();
    }
}

QString Theme::getThemeFunction()
{
    return currentTheme;
}

QString Theme::getLightThemeStyleSheet() const
{
    return R"(
        QMainWindow {
            background-color:rgb(255, 255, 255);
            color: #000000;
        }
        
        QMainWindow::separator {
            width: 0px;
            height: 0px;
            background-color: transparent;
        }
        
        QDockWidget {
            border: none;
        }
        
        QMenuBar {
            background-color:rgb(230, 230, 230);
            color: #000000;
        }
        QSlider::handle:horizontal {
            background: #2196F3;
            border-radius: 7px;
        }
        QSlider::groove:horizontal {
            background:rgb(111, 255, 147);
            height: 3px;
        }
        QPushButton {
            background-color:rgb(212, 212, 212);
            border: 1px solid #dddddd;
            padding: 5px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #f5f5f5;
        }

        /* 播放时的视频区域背景色 */
        QVideoWidget {
            background-color: rgb(255, 255, 255);  
        }
        QListWidget {
            background-color: rgb(200, 200, 200);
            border: 0px solid;
        }

        /* 标题栏样式 - 浅色主题 */
        QMainWindow::title {
            background-color: rgb(230, 230, 230);
            color: #000000;
            padding: 5px;
        }

        /* 底部控制栏 - 浅色主题 */
        QWidget#controlWidget {
            background-color: rgba(200, 200, 200, 30);
        }
        QWidget#controlWidget QLabel {
            color: white;
        }
        QWidget#controlWidget QPushButton {
            border: none;
            padding: 1px;
        }
        QWidget#controlWidget QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
        QWidget#controlWidget QSlider::groove:horizontal {
            height: 3px;
            background: #999999;
        }
        QWidget#controlWidget QSlider::handle:horizontal {
            background: white;
            width: 10px;
            margin: -4px 0;
            border-radius: 5px;
        }
        QWidget#controlWidget QSlider::sub-page:horizontal {
            background: #2196F3;
        }
    )";
}

QString Theme::getDarkThemeStyleSheet() const
{
    return R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        
        QMenuBar {
            background-color: #333333;
            color: #ffffff;
        }
        QMenu {
            background-color: #333333;
            color: #ffffff;
            border: 1px solid #555555;
        }
        QMenu::item:selected {
            background-color: #444444;
        }
        QSlider::handle:horizontal {
            background: #4a9eff;
            border-radius: 7px;
        }
        QSlider::groove:horizontal {
            background: #555555;
            height: 3px;
        }
        QPushButton {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 5px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #444444;
        }
        /* 播放时的视频区域背景色 */
        QVideoWidget {
            background-color: rgb(0, 0, 0);  
            border: 1px solid rgb(134, 209, 255);
        }
        /* 标题栏样式 - 深色主题 */
        QMainWindow::title {
            background-color: #333333;
            color: #ffffff;
            padding: 5px;
        }
        /* 底部控制栏 - 深色主题 */
        QWidget#controlWidget {
            background-color: rgb(48, 48, 48);
        }
        QWidget#controlWidget QLabel {
            color: white;
        }
        QWidget#controlWidget QPushButton {
            border: none;
            padding: 1px;
        }
        QWidget#controlWidget QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
        QWidget#controlWidget QSlider::groove:horizontal {
            height: 3px;
            background: #999999;
        }
        QWidget#controlWidget QSlider::handle:horizontal {
            background: white;
            width: 10px;
            margin: -4px 0;
            border-radius: 5px;
        }
        QWidget#controlWidget QSlider::sub-page:horizontal {
            background: #2196F3;
        }
    )";
}
