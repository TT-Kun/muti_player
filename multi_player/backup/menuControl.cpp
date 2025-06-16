#include "menuControl.h"
#include "player.h" // 包含Player类以便访问其方法
#include <QStyle>
#include <QMessageBox>
#include <QFileInfo>
#include <QStandardPaths>
#include <QFileDialog>
#include <QTimer>
#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QIcon>

// 实现PlayHistory的流操作符
QDataStream &operator<<(QDataStream &out, const PlayHistory &history)
{
    out << history.filePath
        << history.fileName
        << history.playTime
        << history.duration
        << history.lastPosition;
    return out;
}

QDataStream &operator>>(QDataStream &in, PlayHistory &history)
{
    in >> history.filePath
       >> history.fileName
       >> history.playTime
       >> history.duration
       >> history.lastPosition;
    return in;
}

MenuControl::MenuControl(QObject *parent)
    : QObject(parent),
      m_player(nullptr),
      m_settingsButton(nullptr),
      m_mediaPlayer(nullptr),
      m_audioOutput(nullptr),
      m_videoWidget(nullptr),
      m_playlistManager(nullptr),
      m_playMode(Sequential),
      m_currentTheme("dark")
{
    // 设置历史记录文件路径
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!appDataDir.exists()) {
        appDataDir.mkpath(".");
    }
    m_historyFile = appDataDir.filePath("history.dat");
}

MenuControl::~MenuControl()
{
    // 保存历史记录
    savePlayHistory();
}

void MenuControl::setupMenuControl(Player *player, QPushButton *settingsButton, 
                                  QMediaPlayer *mediaPlayer, QAudioOutput *audioOutput,
                                  QVideoWidget *videoWidget, PlaylistManager *playlistManager)
{
    m_player = player;
    m_settingsButton = settingsButton;
    m_mediaPlayer = mediaPlayer;
    m_audioOutput = audioOutput;
    m_videoWidget = videoWidget;
    m_playlistManager = playlistManager;
    
    // 连接设置按钮点击信号
    connect(m_settingsButton, &QPushButton::clicked, this, &MenuControl::showSettingsMenu);
    
    // 加载历史记录
    loadPlayHistory();
    
    // 加载主题
    QSettings settings;
    QString savedTheme = settings.value("theme", "dark").toString();
    if (savedTheme != "light" && savedTheme != "dark") {
        savedTheme = "dark";  //默认深色主题
    }
    applyTheme(savedTheme);
}

void MenuControl::showSettingsMenu()
{
    // 创建设置菜单
    QMenu settingsMenu(m_player);
    
    // 添加子菜单
    settingsMenu.addMenu(createFileMenu());
    settingsMenu.addMenu(createPlayMenu());
    settingsMenu.addMenu(createHelpMenu());
    
    // 菜单显示
    QPoint pos = m_settingsButton->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() - settingsMenu.sizeHint().height());
    

    QScreen *screen = QGuiApplication::screenAt(pos);
    if (screen) {
        QRect screenGeometry = screen->geometry();
        if (pos.y() < screenGeometry.top()) {
            pos = m_settingsButton->mapToGlobal(QPoint(0, m_settingsButton->height()));
        }
    }
    
    settingsMenu.exec(pos);
}

QMenu* MenuControl::createFileMenu()
{
    QMenu *fileMenu = new QMenu("文件(&F)", m_player);
    
    QAction *openAct = new QAction("打开(&O)", m_player);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip("打开媒体文件");
    // 调用private方法
    connect(openAct, &QAction::triggered, m_player, [this]() {
        QMetaObject::invokeMethod(m_player, "openFile");
    });
    
    QAction *addToPlaylistAct = new QAction("添加到播放列表(&A)", m_player);
    connect(addToPlaylistAct, &QAction::triggered, m_playlistManager, &PlaylistManager::addToPlaylist);
    
    QAction *removeFromPlaylistAct = new QAction("从播放列表移除(&R)", m_player);
    connect(removeFromPlaylistAct, &QAction::triggered, m_playlistManager, &PlaylistManager::removeFromPlaylist);
    
    fileMenu->addAction(openAct);
    fileMenu->addAction(addToPlaylistAct);
    fileMenu->addAction(removeFromPlaylistAct);
    fileMenu->addSeparator();
    
    // 添加播放历史子菜单
    fileMenu->addMenu(createHistoryMenu());
    
    return fileMenu;
}

QMenu* MenuControl::createPlayMenu()
{
    QMenu *playMenu = new QMenu("播放(&P)", m_player);
    
    // 添加播放速度子菜单
    playMenu->addMenu(createPlaybackRateMenu());
    
    // 添加播放模式子菜单
    playMenu->addMenu(createPlayModeMenu());
    
    // 分隔线
    playMenu->addSeparator();
    
    // 截图
    QAction *screenshotAct = playMenu->addAction("截图(&S)");
    screenshotAct->setShortcut(Qt::Key_S);
    connect(screenshotAct, &QAction::triggered, this, &MenuControl::captureScreenshot);
    
    return playMenu;
}

QMenu* MenuControl::createHelpMenu()
{
    QMenu *helpMenu = new QMenu("帮助(&H)", m_player);
    
    // 添加主题子菜单
    helpMenu->addMenu(createThemeMenu());
    
    helpMenu->addSeparator();  // 添加分隔线
    
    // 添加快捷键菜单项
    QAction *shortcutsAct = new QAction("快捷键(&K)", m_player);
    connect(shortcutsAct, &QAction::triggered, m_player, [this]() {
        QString shortcuts =
            "播放/暂停: Space\n"
            "快进: →\n"
            "快退: ←\n"
            "退出全屏: Esc\n"
            "静音: M\n"
            "删除播放列表项: Delete\n"
            "打开文件: Ctrl+O";
        QMessageBox::information(m_player, "快捷键", shortcuts);
    });
    
    // 添加关于菜单项
    QAction *aboutAct = new QAction("关于(&A)", m_player);
    connect(aboutAct, &QAction::triggered, m_player, [this]() {
        QMessageBox::about(m_player, "关于播放器",
            "媒体播放器 版本v1.0 开发版本Qt6.5.3\n\n"
            "使用Qt6开发的多媒体播放器，支持播放常见格式。\n"
            "涵盖播放器常用功能\n"
            "开发人员：量子飞跃团队");
    });
    
    helpMenu->addAction(shortcutsAct);
    helpMenu->addAction(aboutAct);
    
    // 添加重置功能
    helpMenu->addSeparator();
    QAction* resetAction = new QAction("恢复初始状态", m_player);
    connect(resetAction, &QAction::triggered, this, &MenuControl::resetAllSettings);
    helpMenu->addAction(resetAction);
    
    return helpMenu;
}

QMenu* MenuControl::createPlaybackRateMenu()
{
    QMenu *playbackRateMenu = new QMenu("播放速度(&R)", m_player);
    QList<double> rates = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    QList<QString> rateTexts = {"0.5x", "0.75x", "1.0x (正常)", "1.25x", "1.5x", "2.0x"};
    
    QActionGroup *rateGroup = new QActionGroup(m_player);
    for (int i = 0; i < rates.size(); ++i) {
        QAction *action = new QAction(rateTexts[i], m_player);
        action->setCheckable(true);
        action->setData(rates[i]);
        rateGroup->addAction(action);
        playbackRateMenu->addAction(action);
        
        // 检查当前播放速度并设置选中状态
        if (qFuzzyCompare(m_mediaPlayer->playbackRate(), rates[i])) {
            action->setChecked(true);
        }
        
        connect(action, &QAction::triggered, this, [this, rates, i]() {
            setPlaybackRate(rates[i]);
        });
    }
    
    return playbackRateMenu;
}

QMenu* MenuControl::createPlayModeMenu()
{
    QMenu *playModeMenu = new QMenu("播放模式(&M)", m_player);
    QActionGroup *modeGroup = new QActionGroup(m_player);
    QStringList modeNames = {"顺序播放", "列表循环", "单曲循环", "随机播放"};
    
    for (int i = 0; i < modeNames.size(); ++i) {
        QAction *action = new QAction(modeNames[i], m_player);
        action->setCheckable(true);
        action->setData(i);
        modeGroup->addAction(action);
        playModeMenu->addAction(action);
        if (i == m_playMode) action->setChecked(true);
    }
    
    connect(modeGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        setPlayMode(static_cast<PlayMode>(action->data().toInt()));
    });
    
    return playModeMenu;
}

QMenu* MenuControl::createThemeMenu()
{
    QMenu *themeMenu = new QMenu("主题(&T)", m_player);
    
    QActionGroup *themeGroup = new QActionGroup(m_player);
    QStringList themes = {"浅色", "深色"};
    QStringList themeValues = {"light", "dark"};
    
    for (int i = 0; i < themes.size(); ++i) {
        QAction *action = themeMenu->addAction(themes[i]);
        action->setCheckable(true);
        action->setData(themeValues[i]);
        themeGroup->addAction(action);
        if (themeValues[i] == m_currentTheme) action->setChecked(true);
    }
    
    connect(themeGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        applyTheme(action->data().toString());
    });
    
    return themeMenu;
}

QMenu* MenuControl::createHistoryMenu()
{
    QMenu *historyMenu = new QMenu("播放历史(&H)", m_player);
    
    if (m_playHistory.isEmpty()) {
        QAction *emptyAct = historyMenu->addAction("暂无播放历史");
        emptyAct->setEnabled(false);
    } else {
        // 添加最近播放的文件
        for (const auto &history : m_playHistory) {
            QString timeStr = history.playTime.toString("yyyy-MM-dd hh:mm");
            QString text = QString("%1 (%2)").arg(history.fileName, timeStr);
            
            QAction *action = historyMenu->addAction(text);
            connect(action, &QAction::triggered, m_player, [this, history]() {
                // 调用Player的playFile方法
                QMetaObject::invokeMethod(m_player, "playFile", Q_ARG(QString, history.filePath));
                
                // 恢复上次播放位置
                QTimer::singleShot(100, m_player, [this, history]() {
                    m_mediaPlayer->setPosition(history.lastPosition);
                });
            });
        }
        
        historyMenu->addSeparator();
        QAction *clearAct = historyMenu->addAction("清除历史记录");
        connect(clearAct, &QAction::triggered, this, &MenuControl::clearHistory);
    }
    
    return historyMenu;
}

void MenuControl::setPlayMode(PlayMode mode)
{
    qDebug() << "MenuControl::setPlayMode - 设置模式为:" << static_cast<int>(mode);
    
    if (m_playMode != mode) {
    m_playMode = mode;
        //模式改变信号
        emit playModeChanged(m_playMode);
    // 保存设置
    QSettings settings;
    settings.setValue("playMode", m_playMode);
    }
}

void MenuControl::togglePlayMode()
{
    // 循环切换播放模式
    PlayMode newMode = static_cast<PlayMode>((static_cast<int>(m_playMode) + 1) % 4);
    qDebug() << "MenuControl::togglePlayMode - 从模式" << static_cast<int>(m_playMode) 
             << "切换到模式" << static_cast<int>(newMode);
    
    setPlayMode(newMode);
}

MenuControl::PlayMode MenuControl::getPlayMode() const
{
    return m_playMode;
}

void MenuControl::setPlaybackRate(double rate)
{
    if (m_mediaPlayer) {
        m_mediaPlayer->setPlaybackRate(rate);
    }
}

void MenuControl::applyTheme(const QString &themeName)
{
    QString styleSheet;

    if (themeName == "light") {
        styleSheet = R"(
            QMainWindow {
                background-color: #f0f0f0;
                color: #000000;
            }
            QMenuBar {
                background-color: #ffffff;
                color: #000000;
            }
            QSlider::handle:horizontal {
                background: #2196F3;
                border-radius: 7px;
            }
            QSlider::groove:horizontal {
                background: #e0e0e0;
                height: 3px;
            }
            QPushButton {
                background-color: #ffffff;
                border: 1px solid #dddddd;
                padding: 5px;
                border-radius: 3px;
            }
            QPushButton:hover {
                background-color: #f5f5f5;
            }
        )";
    }
    else if (themeName == "dark") {
        styleSheet = R"(
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

        )";
    }

    m_player->setStyleSheet(styleSheet);
    m_currentTheme = themeName;

    // 更新播放列表样式
    if (themeName == "dark") {
        setupPlaylistStyle();
    } else {
        m_playlistManager->getPlaylistWidget()->setStyleSheet("");
    }

    // 更新titlebar主题
    if (m_player->m_titleBar) {
        m_player->m_titleBar->updateTheme(themeName);
    }
    
    QMetaObject::invokeMethod(m_player, [this, themeName]() {
        QString dotsIcon = (themeName == "dark") ? ":/icons/menu-dots-white.svg" : ":/icons/menu-dots-dark.svg";
        QString linesIcon = (themeName == "dark") ? ":/icons/menu-lines-white.svg" : ":/icons/menu-lines-dark.svg";
        
        // 重设图标
        QPushButton* toggleBtn = m_player->findChild<QPushButton*>("togglePlaylistButton");
        QPushButton* settingsBtn = m_player->findChild<QPushButton*>("settingsButton");
        
        if (toggleBtn) toggleBtn->setIcon(QIcon(linesIcon));
        if (settingsBtn) settingsBtn->setIcon(QIcon(dotsIcon));
        
        // 更新播放模式图标
        m_player->updatePlayModeIcon();
        
        qDebug() << "已更新图标颜色为" << (themeName == "dark" ? "白色" : "深灰色");
    }, Qt::QueuedConnection);

    // 保存主题设置
    QSettings settings;
    settings.setValue("theme", themeName);
}

QString MenuControl::getCurrentTheme() const
{
    return m_currentTheme;
}

void MenuControl::setupPlaylistStyle()
{
    m_playlistManager->getPlaylistWidget()->setStyleSheet(R"(
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

void MenuControl::loadPlayHistory()
{
    QFile file(m_historyFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_6_0);

        // 清除现有历史记录
        m_playHistory.clear();

        // 读取记录数量
        qint32 count;
        in >> count;

        // 读取每条记录
        for (qint32 i = 0; i < count; ++i) {
            PlayHistory history;
            in >> history;
            m_playHistory.append(history);
        }
    }
}

void MenuControl::savePlayHistory()
{
    QFile file(m_historyFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_6_0);

        // 写入记录数量
        out << qint32(m_playHistory.size());

        // 写入每条记录
        for (const auto &history : m_playHistory) {
            out << history;
        }
    }
}

void MenuControl::addToHistory(const QString &filePath, qint64 duration, qint64 position)
{
    // 检查是否已存在
    auto it = std::find_if(m_playHistory.begin(), m_playHistory.end(),
        [&filePath](const PlayHistory &h) { return h.filePath == filePath; });

    PlayHistory history;
    history.filePath = filePath;
    history.fileName = QFileInfo(filePath).fileName();
    history.playTime = QDateTime::currentDateTime();
    history.duration = duration;
    history.lastPosition = position;

    if (it != m_playHistory.end()) {
        // 更新现有记录
        *it = history;
    } else {
        // 添加新记录
        m_playHistory.prepend(history);
        // 限制历史记录数量
        while (m_playHistory.size() > 50) {
            m_playHistory.removeLast();
        }
    }

    savePlayHistory();
}

void MenuControl::clearHistory()
{
    if (QMessageBox::question(m_player, "确认", "确定要清除所有播放历史记录吗？")
        == QMessageBox::Yes) {
        m_playHistory.clear();
        savePlayHistory();
    }
}

QList<PlayHistory> MenuControl::getPlayHistory() const
{
    return m_playHistory;
}

void MenuControl::captureScreenshot()
{
    if (!m_mediaPlayer || !m_mediaPlayer->isPlaying()) return;

    // 获取视频原始分辨率
    QSize videoSize = m_mediaPlayer->metaData().value(QMediaMetaData::Resolution).toSize();
    if (videoSize.isEmpty()) return;

    // 创建视频帧抓取器
    QVideoSink *videoSink = m_mediaPlayer->videoSink();
    if (!videoSink) return;

    QVideoFrame frame = videoSink->videoFrame();
    if (!frame.isValid()) return;

    // 将视频帧转换为图像
    QImage image = frame.toImage();
    if (image.isNull()) return;

    QString fileName = QFileDialog::getSaveFileName(m_player,
        "保存截图",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
            + "/screenshot.png",
        "图片文件 (*.png *.jpg)");

    if (!fileName.isEmpty()) {
        // 保存原始分辨率的图像
        image.save(fileName);
    }
}

// 初始化设置
void MenuControl::resetAllSettings()
{
    QMessageBox::StandardButton reply = QMessageBox::question(m_player,
        "恢复初始状态",
        "这将删除所有设置、播放历史和播放列表。确定要继续吗？",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // 1. 清空QSettings
    QSettings settings;
    settings.clear();
    
    // 2. 删除播放列表文件
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    QFile playlistFile(appDataDir.filePath("default.m3u"));
    if (playlistFile.exists()) {
        playlistFile.remove();
    }
    
    // 3. 删除历史记录文件
    QFile historyFile(appDataDir.filePath("history.dat"));
    if (historyFile.exists()) {
        historyFile.remove();
    }
    
    // 4. 清空播放列表UI
    if (m_playlistManager) {
        QListWidget* playlistWidget = m_playlistManager->getPlaylistWidget();
        if (playlistWidget) {
            playlistWidget->clear();
        }
    }
    
    // 5. 恢复默认音量和播放模式
    if (m_audioOutput) {
        m_audioOutput->setVolume(0.5);
        m_audioOutput->setMuted(false);
    }
    setPlayMode(MenuControl::Sequential);
    
    // 6. 应用默认主题
    applyTheme("dark");
    
    QMessageBox::information(m_player, "恢复完成", 
        "所有设置已恢复为初始状态。\n请重启应用以完全应用更改。");
}
