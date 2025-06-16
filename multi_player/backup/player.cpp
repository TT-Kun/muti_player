#include "player.h"
#include "./ui_player.h"
#include <QStyle>
#include <QVideoWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QTextStream>
#include <QFile>
#include <QDockWidget>
#include <QShortcut>
#include <QDebug>
#include <QStyle>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QRandomGenerator>
#include <QIcon>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QImage>
#include <QMediaMetaData>
#include <QVideoFrame>
#include <QVideoSink>
#include <QPainter>
#include <QPixmap>
#include <QInputDialog>
#include <QLineEdit>
#include <QTime>
#include <QApplication>
#include <QVBoxLayout>
#include <QWindow>
#include <QWindowStateChangeEvent>
#include "MyBorderContainer.h"
#include <QHBoxLayout>
#include <QGridLayout>

Player::Player(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Player)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/player.svg"));

    QByteArray settingsPathEnv = qgetenv("MULTI_PLAYER_SETTINGS");
    if (!settingsPathEnv.isEmpty()) {
        qDebug() << "设置文件位置:" << settingsPathEnv.constData();
    }
    
    // 无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);
    
    setMinimumSize(800, 400);
    
    // 自定义标题栏
    m_titleBar = new TitleBar(this);
    m_titleBar->setTitle("媒体播放器");
    m_titleBar->setObjectName("titleBar");
    
    // 设置初始窗口大小
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QSize initialSize(screen->availableSize().width() * 0.6, screen->availableSize().height() * 0.7);
        resize(initialSize);
    } else {
        // 如果无法获取屏幕信息，使用固定初始大小
        resize(800, 600);
    }
    
    // 中心内容容器
    QWidget *mainContainerWidget = new QWidget();
    mainContainerWidget->setContentsMargins(0, 0, 0, 0);
    mainContainerWidget->setStyleSheet("border: none; margin: 0; padding: 0;");
    
    // 标题栏和播放区域垂直排列
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainerWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    mainLayout->addWidget(m_titleBar);
    
    // 播放区域容器
    QWidget *playAreaWidget = new QWidget();
    
    // 播放区水平布局（视频区、列表）
    QHBoxLayout *playAreaLayout = new QHBoxLayout(playAreaWidget);
    playAreaLayout->setContentsMargins(0, 0, 0, 0);
    playAreaLayout->setSpacing(0);
    
    // 视频显示区
    QWidget *videoContainer = new QWidget();
    QVBoxLayout *videoContainerLayout = new QVBoxLayout(videoContainer);
    videoContainerLayout->setContentsMargins(0, 0, 0, 0);
    videoContainerLayout->setSpacing(0);
    
    ui->centralwidget->setParent(nullptr);
    videoContainerLayout->addWidget(ui->centralwidget);
    
    videoContainer->setMouseTracking(true);
    videoContainer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    videoContainer->installEventFilter(this);
    
    //将视频容器添加到播放区域布局中
    playAreaLayout->addWidget(videoContainer, 1);
    
    //播放列表区
    QWidget *playlistContainer = new QWidget();
    QVBoxLayout *playlistLayout = new QVBoxLayout(playlistContainer);
    playlistLayout->setContentsMargins(0, 0, 0, 0);
    playlistLayout->setSpacing(0);
    
    //播放列表
    QListWidget *playlistWidget = new QListWidget(playlistContainer);
    playlistLayout->addWidget(playlistWidget);
    

    QScreen *displayScreen = QGuiApplication::primaryScreen();
    if (displayScreen) {
        int screenWidth = displayScreen->availableGeometry().width();
        int minWidth = qBound(200, screenWidth / 9, 400);
        playlistContainer->setMinimumWidth(minWidth);
    }
    
    // 播放列表默认隐藏
    playlistContainer->hide();
    
    // 将播放列表容器添加到播放区域布局
    playAreaLayout->addWidget(playlistContainer);
    
    // 将播放区域容器添加到主布局
    mainLayout->addWidget(playAreaWidget, 1);

    // 创建MyBorderContainer并设置中心内容容器
    m_borderContainer = new MyBorderContainer(nullptr, 400, 800, 4);
    m_borderContainer->setCentralWidget(mainContainerWidget);
    
    // 设置边框容器作为窗口的中央部件
    setCentralWidget(m_borderContainer->getContainer());
    
    setMouseTracking(true);
    
    // 监听窗口状态
    connect(windowHandle(), &QWindow::windowStateChanged, this, &Player::onWindowStateChanged);

    // 初始化视频窗口
    videoWidget = new QVideoWidget(this);
    ui->videoLayout->addWidget(videoWidget);

    videoWidget->setMouseTracking(true);
    videoWidget->installEventFilter(this);

    ui->centralwidget->setMouseTracking(true);
    ui->centralwidget->setAttribute(Qt::WA_MouseTracking, true);

    // 初始化媒体播放器和音频输出
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);

    // 默认一般的音量
    audioOutput->setVolume(0.5);

    // 设置按钮图标
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->backwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->playModeButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->togglePlaylistButton->setIcon(QIcon(":/icons/menu-lines.svg"));
    ui->settingsButton->setIcon(QIcon(":/icons/menu-dots.svg"));
    
    // 按钮样式
    QString buttonStyle = "QPushButton { border: none; padding: 3px; } QPushButton:hover { background-color: rgba(128, 128, 128, 40); border-radius: 3px; }";
    ui->settingsButton->setIconSize(QSize(20, 20));
    ui->settingsButton->setStyleSheet(buttonStyle);
    ui->settingsButton->setToolTip("设置菜单");
    ui->settingsButton->setObjectName("settingsButton");
    
    ui->togglePlaylistButton->setIconSize(QSize(20, 20));
    ui->togglePlaylistButton->setStyleSheet(buttonStyle);
    ui->togglePlaylistButton->setToolTip("播放列表");
    ui->togglePlaylistButton->setObjectName("togglePlaylistButton");
 
    // 音量控制框
    m_volumeControl = new volumeControl();
    m_volumeControl->setVolume(50); // 初始音量设为50%

    // 媒体控制
    mediaControl = new MediaControl(this);

    connect(ui->volumeButton, &QPushButton::clicked, this, [this]() {
        // 获取音量按钮的全局位置
        QPoint volumeBtnPos = ui->volumeButton->mapToGlobal(QPoint(0, 0));
        // 显示音量控制弹出框
        m_volumeControl->showVolumeControl(volumeBtnPos);
    });

    // 连接音量控制弹出框的信号
    connect(m_volumeControl, &volumeControl::volumeChanged, mediaControl, &MediaControl::setVolume);
    connect(m_volumeControl, &volumeControl::muteToggled, mediaControl, [this](bool muted) {
        if (muted) {
            mediaControl->toggleMute(); // 静音
        } else if (mediaControl->isMuted()) {
            mediaControl->toggleMute(); // 取消静音
        }
    });

    QShortcut *muteShortcut = new QShortcut(Qt::Key_M, this);
    connect(muteShortcut, &QShortcut::activated, mediaControl, &MediaControl::toggleMute);

    // 进度条相关连接
    connect(ui->progressSlider, &QSlider::sliderMoved, mediaControl, [this](int position) {
        mediaControl->setPosition(position);
    });

    // 连接MediaControl的信号到Player的槽
    connect(mediaControl, &MediaControl::positionChanged, this, &Player::onPositionChanged);
    connect(mediaControl, &MediaControl::durationChanged, this, &Player::onDurationChanged);
    connect(mediaControl, &MediaControl::playbackStateChanged, this, &Player::onPlaybackStateChanged);
    connect(mediaControl, &MediaControl::volumeChanged, this, &Player::onVolumeChanged);
    connect(mediaControl, &MediaControl::mutedChanged, this, &Player::onMutedChanged);
    connect(mediaControl, &MediaControl::fullScreenChanged, this, &Player::onFullScreenChanged);

    // 创建菜单控制器（仅创建对象，稍后初始化）
    menuControl = new MenuControl(this);

    // 创建播放列表管理器
    playlistManager = new PlaylistManager(this);
    
    // 初始化播放列表管理器
    playlistManager->setupPlaylist(playlistContainer, playlistWidget, ui->togglePlaylistButton);
    
    // 连接播放列表信号
    connect(playlistManager, &PlaylistManager::playlistItemDoubleClicked,
            this, &Player::onPlaylistItemDoubleClicked);
    connect(playlistManager, &PlaylistManager::playRequested,
            this, &Player::onPlayRequested);
            

    menuControl->setupMenuControl(this, ui->settingsButton, mediaPlayer, audioOutput, videoWidget, playlistManager);
    
    // 初始化播放模式
    menuControl->setPlayMode(MenuControl::Sequential);
    
    QString theme = menuControl->getCurrentTheme();
    QString dotsIcon = (theme == "dark") ? ":/icons/menu-dots-white.svg" : ":/icons/menu-dots-dark.svg";
    QString linesIcon = (theme == "dark") ? ":/icons/menu-lines-white.svg" : ":/icons/menu-lines-dark.svg";
    
    ui->togglePlaylistButton->setIcon(QIcon(linesIcon));
    ui->settingsButton->setIcon(QIcon(dotsIcon));
    
    // 现在可以安全地更新播放模式图标
    updatePlayModeIcon();

    // debug错误处理
    connect(mediaPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
        if (error != QMediaPlayer::NoError) {
            QMessageBox::warning(this, "播放错误", "播放出错：" + errorString);
        }
    });

    // debug媒体状态
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        switch (status) {
            case QMediaPlayer::LoadedMedia:
                // 媒体加载完成，可以开始播放
                ui->playButton->setEnabled(true);
                break;
            case QMediaPlayer::InvalidMedia:
                // 无效媒体
                QMessageBox::warning(this, "错误", "无效的媒体文件");
                break;
            default:
                break;
        }
    });
    
    menuBar()->hide();

    // 删除快捷键
    QShortcut *deleteShortcut = new QShortcut(QKeySequence::Delete, playlistManager->getPlaylistWidget());
    connect(deleteShortcut, &QShortcut::activated, playlistManager, &PlaylistManager::removeFromPlaylist);

    // 视频窗口双击
    videoWidget->setMouseTracking(true);
    videoWidget->installEventFilter(this);

    // 设置鼠标跟踪
    videoWidget->setMouseTracking(true);

    // 添加快捷键
    QShortcut *spaceShortcut = new QShortcut(Qt::Key_Space, this);
    connect(spaceShortcut, &QShortcut::activated, this, &Player::on_playButton_clicked);

    QShortcut *leftShortcut = new QShortcut(Qt::Key_Left, this);
    connect(leftShortcut, &QShortcut::activated, this, &Player::on_backwardButton_clicked);

    QShortcut *rightShortcut = new QShortcut(Qt::Key_Right, this);
    connect(rightShortcut, &QShortcut::activated, this, &Player::on_forwardButton_clicked);

    QShortcut *escShortcut = new QShortcut(Qt::Key_Escape, this);
    connect(escShortcut, &QShortcut::activated, this, [this]() {
        if (mediaControl->playbackState() != QMediaPlayer::StoppedState && windowState() & Qt::WindowFullScreen) {
            mediaControl->toggleFullScreen();
        }
    });

    // 初始化MediaControl
    mediaControl->setupMediaControl(this, mediaPlayer, audioOutput, videoWidget, menuControl, playlistManager);

    // 加载设置
    loadSettings();

    // 连接信号
    connect(ui->previousButton, &QPushButton::clicked, mediaControl, &MediaControl::playPrevious);
    connect(ui->nextButton, &QPushButton::clicked, mediaControl, &MediaControl::playNext);
    
    // 更新播放模式
    disconnect(ui->playModeButton, &QPushButton::clicked, nullptr, nullptr);
    connect(ui->playModeButton, &QPushButton::clicked, menuControl, &MenuControl::togglePlayMode);
    connect(menuControl, &MenuControl::playModeChanged, this, &Player::updatePlayModeIcon);

    // 快捷键
    QShortcut *screenshotShortcut = new QShortcut(Qt::Key_S, this);
    connect(screenshotShortcut, &QShortcut::activated, mediaControl, &MediaControl::captureScreenshot);

    QShortcut *volumeUpShortcut = new QShortcut(Qt::Key_Up, this);
    connect(volumeUpShortcut, &QShortcut::activated, this, [this]() {
        int newVolume = qMin(m_volumeControl->volume() + 5, 100);
        m_volumeControl->setVolume(newVolume);
        mediaControl->setVolume(newVolume);
    });

    videoAspectRatio = 0.0;

    QShortcut *volumeDownShortcut = new QShortcut(Qt::Key_Down, this);
    connect(volumeDownShortcut, &QShortcut::activated, this, [this]() {
        int newVolume = qMax(m_volumeControl->volume() - 5, 0);
        m_volumeControl->setVolume(newVolume);
        mediaControl->setVolume(newVolume);
    });

    // 进度条鼠标追踪
    ui->progressSlider->setMouseTracking(true);
    ui->progressSlider->installEventFilter(this);

    // 更新titleBar
    m_titleBar->updateTheme(menuControl->getCurrentTheme());
}

Player::~Player()
{
    delete ui;
}

void Player::on_playButton_clicked()
{
    qDebug() << "播放按钮被点击";

    if (!mediaPlayer->source().isEmpty() || playlistManager->getCurrentItem()) {
        mediaControl->togglePlayPause();
    } else {
        qDebug() << "打开新文件";
        openFile();
    }
}

void Player::on_stopButton_clicked()
{
    mediaControl->stop();
}

void Player::on_forwardButton_clicked()
{
    mediaControl->forward();
}

void Player::on_backwardButton_clicked()
{
    mediaControl->backward();
}

// 响应MediaControl的positionChanged信号
void Player::onPositionChanged(qint64 position)
{
    ui->progressSlider->setValue(position);
    ui->currentTimeLabel->setText(mediaControl->formatTime(position));
}

// 响应MediaControl的durationChanged信号
void Player::onDurationChanged(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);
    ui->totalTimeLabel->setText(mediaControl->formatTime(duration));
}

// 响应MediaControl的volumeChanged信号
void Player::onVolumeChanged(int volume)
{
    // 同步更新音量控制弹出框
    if (m_volumeControl->volume() != volume) {
        m_volumeControl->setVolume(volume);
    }
    
    // 更新音量图标
    if (volume == 0) {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else if (volume < 33) {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        ui->volumeButton->setToolTip(QString("音量: %1%").arg(volume));
    } else if (volume < 66) {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        ui->volumeButton->setToolTip(QString("音量: %1%").arg(volume));
    } else {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        ui->volumeButton->setToolTip(QString("音量: %1%").arg(volume));
    }
}

// 响应MediaControl的playbackStateChanged信号
void Player::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    QPushButton *btn = ui->playButton;
    switch (state) {
        case QMediaPlayer::PlayingState:
            btn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            btn->setToolTip("暂停");
            break;
        case QMediaPlayer::PausedState:
            btn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            btn->setToolTip("继续播放");
            break;
        case QMediaPlayer::StoppedState:
            btn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            btn->setToolTip("播放");
            break;
    }
}

// 响应MediaControl的mutedChanged信号
void Player::onMutedChanged(bool muted)
{
    if (muted) {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

// 响应MediaControl的fullScreenChanged信号，控制全屏，边框事件
void Player::onFullScreenChanged(bool fullScreen)
{
    if (fullScreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);
        ui->controlWidget->hide();  // 隐藏控制栏
        m_titleBar->hide();         // 隐藏标题栏
        playlistManager->getPlaylistContainer()->hide();
        menuBar()->hide();
        // 隐藏禁用边框
        if (m_borderContainer) {
            // 禁用所有边框响应
            m_borderContainer->setResizable(false);
            // 或将边框大小设置为0
            m_borderContainer->setBorderSize(0);
        }
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
        ui->controlWidget->show();  // 显示控制栏
        m_titleBar->show();         // 显示标题栏
        menuBar()->hide();
        if (playlistManager->isPlaylistVisible()) {
            playlistManager->getPlaylistContainer()->show();
        }
        
        // 恢复边框
        if (m_borderContainer) {
            // 重启边框
            m_borderContainer->setResizable(true);
            // 恢复边框
            m_borderContainer->setBorderSize(1); // 恢复之前设置的值
        }
    }
}

void Player::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        "打开媒体文件",
        "",
        "媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.wav);;所有文件 (*.*)");

    if (!fileNames.isEmpty()) {
        // 添加到播放列表
        for (const QString& fileName : fileNames) {
            QListWidgetItem* item = new QListWidgetItem(QFileInfo(fileName).fileName());
            item->setData(Qt::UserRole, fileName);
            playlistManager->getPlaylistWidget()->addItem(item);
        }

        // 保存到列表
        playlistManager->saveDefaultPlaylist();

        // 播放第一个文件
        mediaControl->playFile(fileNames.first());
        playlistManager->setCurrentRow(playlistManager->getPlaylistCount() - fileNames.size());
    }
}

// 播放列表双击播放
void Player::onPlaylistItemDoubleClicked(QListWidgetItem* item)
{
    mediaControl->playFile(item->data(Qt::UserRole).toString());
}

// 处理播放请求
void Player::onPlayRequested(const QString &filePath)
{
    mediaControl->playFile(filePath);
}


bool Player::eventFilter(QObject *obj, QEvent *event)
{
    // 视频窗口双击
    if (obj == videoWidget && event->type() == QEvent::MouseButtonDblClick) {
        // 视频区全屏
        videoFullScreen();
        return true;
    }
    // 处理预览
    if (obj == ui->progressSlider) {
        switch (event->type()) {
            case QEvent::MouseMove: {
                // 由于鼠标拖动会造成频繁刷新，添加防抖逻辑，限制预览更新频率
                static QTime lastUpdateTime = QTime::currentTime();
                if (lastUpdateTime.msecsTo(QTime::currentTime()) < 50) {
                    return true; // 限制更新频率，每50ms更新一次
                }
                lastUpdateTime = QTime::currentTime();
                
                // 只有在隐藏中状态时忽略新的显示请求
                // 其他状态(包括显示中、已显示、已隐藏)都允许更新预览
                if (mediaControl->getPreviewState() == MediaControl::PreviewState::Hiding) {
                    return true;
                }
                
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

                // 计算鼠标位置对应的视频时间
                double ratio = static_cast<double>(mouseEvent->pos().x()) / ui->progressSlider->width();
                qint64 position = ratio * mediaControl->duration();

                // 获取进度条的全局位置
                QPoint sliderPos = ui->progressSlider->mapToGlobal(QPoint(0, 0));

                mediaControl->showThumbnailPreview(position, mouseEvent->globalPosition().toPoint(), sliderPos);
                qDebug("显示预览");
                break;
            }
            case QEvent::Leave:
                qDebug("预览隐藏");
                mediaControl->hideThumbnailPreview();
                break;
            default:
                break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void Player::toggleFullScreen()
{
    if (windowState() & Qt::WindowFullScreen) {
        // 是全屏，还原
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    } else {
        // 不是全屏，切全屏
        setWindowState(windowState() | Qt::WindowFullScreen);
    }
}

void Player::saveSettings()
{
    QSettings settings;

    // 保存窗口状态
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // 保存播放列表状态
    settings.setValue("playlistVisible", playlistManager->isPlaylistVisible());

    // 保存媒体控制相关设置
    mediaControl->saveSettings();
}

void Player::loadSettings()
{
    QSettings settings;

    // 恢复窗口状态
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    // 恢复播放列表显示状态
    bool playlistVisible = settings.value("playlistVisible", true).toBool();
    playlistManager->getPlaylistContainer()->setVisible(playlistVisible);

    // 加载媒体控制设置
    mediaControl->loadSettings();
}

void Player::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QMainWindow::closeEvent(event);
}

// 更新播放模式图标
void Player::updatePlayModeIcon()
{
    QIcon icon;
    QString tooltip;
    
    // 获取当前主题
    QString theme = menuControl->getCurrentTheme();
    bool isDarkTheme = (theme == "dark");
    
    // 根据主题和播放模式选择合适的图标
    QString iconSuffix = isDarkTheme ? "" : "-dark";

    switch (menuControl->getPlayMode()) {
        case MenuControl::Sequential:
            icon = QIcon(QStringLiteral(":/icons/sequential%1.svg").arg(iconSuffix));
            tooltip = "顺序播放";
            break;
        case MenuControl::Loop:
            icon = QIcon(QStringLiteral(":/icons/loop%1.svg").arg(iconSuffix));
            tooltip = "列表循环";
            break;
        case MenuControl::SingleLoop:
            icon = QIcon(QStringLiteral(":/icons/single-loop%1.svg").arg(iconSuffix));
            tooltip = "单曲循环";
            break;
        case MenuControl::Random:
            icon = QIcon(QStringLiteral(":/icons/random%1.svg").arg(iconSuffix));
            tooltip = "随机播放";
            break;
    }

    // 确保图标大小合适
    QSize iconSize(24, 24);
    ui->playModeButton->setIconSize(iconSize);
    ui->playModeButton->setIcon(icon);
    ui->playModeButton->setToolTip(tooltip);
}

// 添加窗口大小变化事件处理
void Player::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    
    mediaControl->handleResize();
}

// 在onFullScreenChanged方法之前的更新窗口标题方法
void Player::updateWindowTitle(const QString &title)
{
    m_titleBar->setTitle(title);
}

void Player::onWindowStateChanged(Qt::WindowState newState)
{
    // 更新最大化按钮图标
    bool isMaximized = newState == Qt::WindowMaximized;
    m_titleBar->updateMaximizeButton(isMaximized);
}

// 播放文件
void Player::playFile(const QString& filePath)
{
    if (!filePath.isEmpty()) {
        mediaControl->playFile(filePath);
    }
}

bool Player::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    return QMainWindow::nativeEvent(eventType, message, result);
}

// 视频区全屏切换
void Player::videoFullScreen()
{
    mediaControl->toggleFullScreen();
}






