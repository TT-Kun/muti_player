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
#include <QTime>
#include <QIcon>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QImage>
#include <QMenu>
#include <QMediaMetaData>
#include <QVideoFrame>
#include <QDataStream>
#include <QPainter>
#include <QPixmap>
#include <QInputDialog>
#include <QLineEdit>
#include <QVideoProbe>
#include "theme.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QVariant>
#include <QDebug>
#include "titlebar.h"
#include "mywindow.h"

Player::Player(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Player)
{
    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);

    ui->setupUi(this);
    // 隐藏原有菜单栏
    ui->menubar->setVisible(false);

    // 创建自定义标题栏
    m_titleBar = new TitleBar(this);
    m_titleBar->setTitle("mutl_player");

     QFile file(":/file_for_read/http_yin.txt");
       if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
           QTextStream in(&file);
           QString ip = in.readLine().trimmed();
           QString url = QString("http://%1/analyze").arg(ip);
           uploader = new FileUploader(url, this);//音频上传
            qDebug() << "频配置文件"<<url;
       }else{
           qDebug() << "无法读取音频配置文件";
       }

    // 连接标题栏信号
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &QMainWindow::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this](){
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });

    connect(m_titleBar, &TitleBar::closeClicked, this, &QMainWindow::close);
    connect(m_titleBar, &TitleBar::doubleClicked, this, [this](){
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });

    connect(m_titleBar, &TitleBar::backRequested, this,&Player::back);


    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建标题栏和菜单栏容器
    QWidget* topContainer = new QWidget(this);
    QVBoxLayout* topLayout = new QVBoxLayout(topContainer);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    topLayout->addWidget(m_titleBar);
    topLayout->addWidget(ui->menubar);

    // 将标题栏容器添加到主布局
    mainLayout->addWidget(topContainer);

    // 创建内容区域（视频 + 播放列表）
    QWidget *contentWidget = new QWidget(this);
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // 添加视频区域
    QWidget *centralWidgetContent = ui->centralwidget;
    centralWidgetContent->setParent(nullptr);
    contentLayout->addWidget(centralWidgetContent, 1); // 视频区域占主要空间

    // 创建播放列表并添加到右侧区域
    playlistDock = new QDockWidget(this);
    playlistDock->setTitleBarWidget(new QWidget()); // 隐藏标题栏
    playlistWidget = new QListWidget(playlistDock);
    playlistDock->setWidget(playlistWidget);
    
    // 设置播放列表最小宽度
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        int screenWidth = screen->availableGeometry().width();
        int minWidth = qBound(200, screenWidth / 6, 400);
        playlistDock->setMinimumWidth(minWidth);
    }

    // 设置播放列表样式
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

    // 添加播放列表到内容布局
    contentLayout->addWidget(playlistDock);

    // 将内容区域添加到主布局
    mainLayout->addWidget(contentWidget, 1);

    // 设置新的中央部件
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    // 显示菜单栏
    ui->menubar->setVisible(true);

    // Initialize random seed for qrand
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    // 设置应用图标
    setWindowIcon(QIcon(":/icons/player.svg"));

    // 初始化视频窗口
    videoWidget = new QVideoWidget(this);
    ui->videoLayout->addWidget(videoWidget);

    // 初始化音频格式
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // 创建音频输出设备
    audioOutput = new QAudioOutput(format, this);

    // 初始化媒体播放器
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setVideoOutput(videoWidget);

    // 设置初始音量
    mediaPlayer->setVolume(ui->volumeSlider->value());

    // 设置按钮图标
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->backwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->playModeButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    updatePlayModeIcon();  // 更新播放模式图标


    // 连接音量按钮点击信号
    connect(ui->volumeButton, &QPushButton::clicked, this, &Player::toggleMute);

    // 修改静音快捷键连接
    QShortcut *muteShortcut = new QShortcut(Qt::Key_M, this);
    connect(muteShortcut, &QShortcut::activated, this, &Player::toggleMute);

    // 进度条相关连接
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &Player::updatePosition);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &Player::updateDuration);
    connect(ui->progressSlider, &QSlider::sliderMoved, this, &Player::setPosition);

    // 音量控制相关连接
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &Player::setVolume);

    // 播放状态改变时更新图标
    connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &Player::updatePlayIcon);

    // 初始化菜单指针
    playbackRateMenu = nullptr;
    rateGroup = nullptr;

    // 创建播放列表切换按钮
    togglePlaylistButton = new QPushButton(this);
    togglePlaylistButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    togglePlaylistButton->setToolTip("显示/隐藏播放列表");
    togglePlaylistButton->setFixedSize(32, 32);
    togglePlaylistButton->setFocusPolicy(Qt::NoFocus);
    togglePlaylistButton->setStyleSheet(R"(
        QPushButton {
            border: none;
            padding: 1px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
    )");

    // 将按钮添加到控制栏
    ui->horizontalLayout->addWidget(togglePlaylistButton);

    connect(togglePlaylistButton, &QPushButton::clicked, this, &Player::togglePlaylist);

    // 连接播放列表信号
    connect(playlistWidget, &QListWidget::itemDoubleClicked,
            this, &Player::playlistItemDoubleClicked);

    // 设置默认播放列表路径
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!appDataDir.exists()) {
        appDataDir.mkpath(".");
    }
    defaultPlaylistFile = appDataDir.filePath("default.m3u");

    // 加载默认播放列表
    loadDefaultPlaylist();

    createMenus();
    createPlaybackRateMenu();

    // 添加错误处理连接
    connect(mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, [this](QMediaPlayer::Error error) {
                QMessageBox::warning(this, "播放错误", "播放出错：" + mediaPlayer->errorString());
            });

    // 添加媒体状态变化处理
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

    // 添加删除快捷键
    QShortcut *deleteShortcut = new QShortcut(QKeySequence::Delete, playlistWidget);
    connect(deleteShortcut, &QShortcut::activated, this, &Player::removeFromPlaylist);

    // 设置播放列表右键菜单
    playlistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(playlistWidget, &QWidget::customContextMenuRequested,
            this, &Player::showPlaylistContextMenu);

    isFullScreen = false;

    // 为视频窗口添加双击事件
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
        if (isFullScreen) {
            toggleFullScreen();
        }
    });

    // 启用拖拽排序
    playlistWidget->setDragDropMode(QAbstractItemView::InternalMove);
    playlistWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // 添加播放列表项变化监听
    connect(playlistWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &Player::saveDefaultPlaylist);

    // 初始化播放模式
    playMode = Sequential;

    // 加载设置
    loadSettings();

    //    // 添加播放模式菜单
    //    QMenu *playMenu = ui->menubar->addMenu("播放(&P)");


    // 媒体播放结束时的处理
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            switch (playMode) {
            case SingleLoop:
                mediaPlayer->setPosition(0);
                 qDebug() << "开始播放啦啦啦啦"<<playlistWidget->currentItem()->data(Qt::UserRole).toString();
                    uploader->upload(playlistWidget->currentItem()->data(Qt::UserRole).toString());
                mediaPlayer->play();
                break;
            case Loop:
            case Sequential:
                playNext();
                break;
            case Random:
                if (playlistWidget->count() > 0) {
                    int randomRow = qrand() % playlistWidget->count(); // Use qrand() for Qt5
                    playlistWidget->setCurrentRow(randomRow);
                    playFile(playlistWidget->currentItem()->data(Qt::UserRole).toString());
                }
                break;
            }
        }
    });

    // 连接信号
    connect(ui->previousButton, &QPushButton::clicked, this, &Player::playPrevious);
    connect(ui->nextButton, &QPushButton::clicked, this, &Player::playNext);
    connect(ui->playModeButton, &QPushButton::clicked, this, &Player::togglePlayMode);

    // 设置播放列表接受拖放
    playlistWidget->setAcceptDrops(true);
    playlistWidget->viewport()->installEventFilter(this);

    // 添加更多快捷键
    QShortcut *screenshotShortcut = new QShortcut(Qt::Key_S, this);
    connect(screenshotShortcut, &QShortcut::activated, this, &Player::captureScreenshot);

    QShortcut *volumeUpShortcut = new QShortcut(Qt::Key_Up, this);
    connect(volumeUpShortcut, &QShortcut::activated, this, [this]() {
        int newVolume = qMin(ui->volumeSlider->value() + 5, 100);
        ui->volumeSlider->setValue(newVolume);
    });

    QShortcut *volumeDownShortcut = new QShortcut(Qt::Key_Down, this);
    connect(volumeDownShortcut, &QShortcut::activated, this, [this]() {
        int newVolume = qMax(ui->volumeSlider->value() - 5, 0);
        ui->volumeSlider->setValue(newVolume);
    });

    // 设置历史记录文件路径
    QDir appDataDir2(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!appDataDir2.exists()) {
        appDataDir2.mkpath(".");
    }
    historyFile = appDataDir2.filePath("history.dat");

    // 加载历史记录
    loadPlayHistory();

    // 初始化预览播放器
    previewPlayer = new QMediaPlayer(this);
    previewVideoWidget = new QVideoWidget;
    previewPlayer->setVideoOutput(previewVideoWidget);

    // 创建预览容器
    thumbnailPreview = new QLabel(nullptr);  // 改为顶层窗口
    thumbnailPreview->setFixedSize(160, 90);  // 16:9 缩略图
    thumbnailPreview->setStyleSheet(
        "QLabel {"
        "   background-color: black;"
        "   border: 1px solid #666;"
        "   padding: 1px;"
        "}"
        );
    thumbnailPreview->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);  // 设置为工具提示窗口

    // 创建时间标签
    previewTimeLabel = new QLabel(thumbnailPreview);
    previewTimeLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   background-color: rgba(0, 0, 0, 128);"
        "   padding: 2px;"
        "}"
        );
    previewTimeLabel->setAlignment(Qt::AlignCenter);

    thumbnailPreview->hide();

    // 为进度条添加鼠标追踪
    ui->progressSlider->setMouseTracking(true);
    ui->progressSlider->installEventFilter(this);

    // 初始化预览关闭定时器
    previewHideTimer = new QTimer(this);
    previewHideTimer->setSingleShot(true);  // 设置为单次触发
    previewHideTimer->setInterval(2000);    // 2秒后触发
    connect(previewHideTimer, &QTimer::timeout, thumbnailPreview, &QLabel::hide);

    // 设置流媒体历史记录文件路径
    streamHistoryFile = appDataDir.filePath("streams.dat");

    // 加载流媒体历史记录
    loadStreamHistory();

    // 读取并加载主题设置
        QSettings settings;
        currentTheme = settings.value("theme", "dark").toString();
        bool isSystemTracking = settings.value("systemThemeTracking", false).toBool();
        qDebug() << "加载的主题设置: " << currentTheme << "系统主题跟踪: " << isSystemTracking;

        if (isSystemTracking) {
            // 如果启用了系统主题跟踪，先检查当前系统主题
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

            if (reply.isValid()) {
                QString theme = reply.value().toString().toLower();
                bool isDarkTheme = theme.contains("dark");

                // 更新标题栏样式
                m_titleBar->updateStyle(isDarkTheme);



                emit change_tepy(false);


                // 启用系统主题跟踪
                Theme::instance().enableSystemThemeTracking(this);
            } else {
                qDebug() << "获取系统主题失败，使用默认深色主题";
                m_titleBar->updateStyle(true);
                Theme::instance().applyTheme("dark", this);
                emit change_tepy(false);
            }
        } else {
            // 根据保存的主题设置处理
            if (currentTheme == "light") {
                // 处理浅色主题
                Theme::instance().applyTheme("light", this);
                m_titleBar->updateStyle(false);  // 设置浅色标题栏
                playlistWidget->setStyleSheet(""); // 清除播放列表深色样式
                emit change_tepy(true);
            } else {
                // 处理深色主题或默认
                Theme::instance().applyTheme("dark", this);
                m_titleBar->updateStyle(true);  // 设置深色标题栏
                Theme::instance().setupPlaylistStyle(playlistWidget);
                emit change_tepy(false);
            }
        }


    // 初始化视频探针
    videoProbe = new QVideoProbe(this);
    //videoProbe->setSource(mediaPlayer);
    if (!videoProbe->setSource(mediaPlayer)) {
        qDebug() << "Failed to initialize video probe!";
    }
}

Player::~Player()
{
    delete ui;
}

//点击播放键
void Player::on_playButton_clicked()
{
    qDebug() << "播放按钮被点击，当前状态:" << mediaPlayer->state();

    if (!mediaPlayer->currentMedia().isNull() || playlistWidget->currentItem()) {
        switch (mediaPlayer->state()) {
        case QMediaPlayer::PlayingState:
            mediaPlayer->pause();
            break;
        case QMediaPlayer::PausedState:
        case QMediaPlayer::StoppedState:
            if (mediaPlayer->currentMedia().isNull() && playlistWidget->currentItem()) {

                playFile(playlistWidget->currentItem()->data(Qt::UserRole).toString());
            } else {
                 qDebug() << "开始播放啦啦啦啦~~~~~kaishi上传"<<playlistWidget->currentItem()->data(Qt::UserRole).toString();
                 uploader->upload(playlistWidget->currentItem()->data(Qt::UserRole).toString());
                mediaPlayer->play();
            }
            break;
        }
    } else {
        openFile();
    }
}

//点击停止播放键
void Player::on_stopButton_clicked()
{
    mediaPlayer->stop();
    ui->progressSlider->setValue(0);
    ui->currentTimeLabel->setText("00:00");
}

//点击向前快进10秒处理函数
void Player::on_forwardButton_clicked()
{
    // 快进10秒
    qint64 position = mediaPlayer->position() + 10000;
    mediaPlayer->setPosition(position);
}

//点击向后快退10秒处理函数
void Player::on_backwardButton_clicked()
{
    // 快退10秒
    qint64 position = mediaPlayer->position() - 10000;
    if (position < 0) position = 0;
    mediaPlayer->setPosition(position);
}

//更新播放进度显示和当前位置
void Player::updatePosition(qint64 position)
{
    ui->progressSlider->setValue(position);
    ui->currentTimeLabel->setText(formatTime(position));

    // 保存当前播放位置
    if (!mediaPlayer->media().isNull()) {
        QUrl mediaUrl = mediaPlayer->media().canonicalUrl();
        lastPositions[mediaUrl.toString()] = position;
    }
}

// 更新总时长显示
void Player::updateDuration(qint64 duration)
{
    ui->progressSlider->setRange(0, duration);
    ui->totalTimeLabel->setText(formatTime(duration));
}

// 设置播放位置
void Player::setPosition(int position)
{
    if (mediaPlayer->isSeekable()) {
        mediaPlayer->setPosition(position);
    }
}

// 设置音量大小并更新图标状态
void Player::setVolume(int volume)
{
    mediaPlayer->setVolume(volume);  // 直接设置播放器音量

    // 更新音量图标状态
    if (volume == 0) {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
    ui->volumeButton->setToolTip(QString("音量: %1%").arg(volume));
}

// 更新播放按钮图标状态
void Player::updatePlayIcon(QMediaPlayer::State state)
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

QString Player::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

// 创建菜单栏和各项菜单
void Player::createMenus()
{
    QMenu *fileMenu = ui->menubar->addMenu("文件(&F)");

    QAction *openAct = new QAction("打开(&O)", this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setStatusTip("打开媒体文件");
    connect(openAct, &QAction::triggered, this, &Player::openFile);

    QAction *addToPlaylistAct = new QAction("添加到播放列表(&A)", this);
    connect(addToPlaylistAct, &QAction::triggered, this, &Player::addToPlaylist);

    QAction *removeFromPlaylistAct = new QAction("从播放列表移除(&R)", this);
    connect(removeFromPlaylistAct, &QAction::triggered, this, &Player::removeFromPlaylist);

    QAction *savePlaylistAct = new QAction("保存播放列表(&S)", this);
    connect(savePlaylistAct, &QAction::triggered, this, &Player::savePlaylist);

    QAction *loadPlaylistAct = new QAction("加载播放列表(&L)", this);
    connect(loadPlaylistAct, &QAction::triggered, this, &Player::loadPlaylist);

    fileMenu->addAction(openAct);
    fileMenu->addAction(addToPlaylistAct);
    fileMenu->addAction(removeFromPlaylistAct);
    fileMenu->addSeparator();
    fileMenu->addAction(savePlaylistAct);
    fileMenu->addAction(loadPlaylistAct);

    // 添加播放菜单
    QMenu *playMenu = ui->menubar->addMenu("播放(&P)");
    playbackRateMenu = playMenu->addMenu("播放速度(&R)");
    QMenu *playModeMenu = playMenu->addMenu("播放模式(&M)");

    QActionGroup *modeGroup = new QActionGroup(this);
    QStringList modeNames = {"顺序播放", "列表循环", "单曲循环", "随机播放"};
    for (int i = 0; i < modeNames.size(); ++i) {
        QAction *action = new QAction(modeNames[i], this);
        action->setCheckable(true);
        action->setData(i);
        modeGroup->addAction(action);
        playModeMenu->addAction(action);
        if (i == playMode) action->setChecked(true);
    }

    connect(modeGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        setPlayMode(static_cast<PlayMode>(action->data().toInt()));
    });



    // 添加视频菜单
    QMenu *videoMenu = ui->menubar->addMenu("视频(&V)");

    // 添加宽高比菜单
    aspectRatioMenu = videoMenu->addMenu("宽高比(&A)");
    QActionGroup *ratioGroup = new QActionGroup(this);

    struct AspectRatio {
        QString name;
        double ratio;
    };

    QList<AspectRatio> ratios = {
        {"自动", 0.0},
        {"4:3", 4.0/3.0},
        {"16:9", 16.0/9.0},
        {"1:1", 1.0},
        {"2.35:1", 2.35}
    };

    for (const auto &ratio : ratios) {
        QAction *action = aspectRatioMenu->addAction(ratio.name);
        action->setCheckable(true);
        action->setData(ratio.ratio);
        ratioGroup->addAction(action);
        if (ratio.ratio == 0.0) action->setChecked(true);
    }

    connect(ratioGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        setAspectRatio(action->data().toDouble());
    });

    // 添加当前播放信息选项
    QAction *infoAct = videoMenu->addAction("当前播放信息(&I)");
    infoAct->setShortcut(Qt::CTRL + Qt::Key_I);
    connect(infoAct, &QAction::triggered, this, &Player::showCurrentMediaInfo);

    // 添加截图功能
    QAction *screenshotAct = videoMenu->addAction("截图(&S)");
    screenshotAct->setShortcut(Qt::Key_S);
    connect(screenshotAct, &QAction::triggered, this, &Player::captureScreenshot);

    // 添加帮助菜单
    QMenu *helpMenu = ui->menubar->addMenu("帮助(&H)");
    // 添加主题菜单到帮助菜单
    themeMenu = helpMenu->addMenu("主题(&T)");

    // 创建主题选择组
    themeGroup = new QActionGroup(this);

    // 使用Theme类设置主题菜单
    Theme::instance().setupThemeMenu(themeMenu, this, SLOT(onThemeChanged(QAction*)));

    helpMenu->addSeparator();  // 添加分隔线
    QAction *shortcutsAct = new QAction("快捷键(&K)", this);
    connect(shortcutsAct, &QAction::triggered, this, [this]() {
        QString shortcuts =
            "播放/暂停: Space\n"
            "快进: →\n"
            "快退: ←\n"
            "退出全屏: Esc\n"
            "静音: M\n"
            "删除播放列表项: Delete\n"
            "打开文件: Ctrl+O";
        QMessageBox::information(this, "快捷键", shortcuts);
    });

    QAction *aboutAct = new QAction("关于(&A)", this);
    connect(aboutAct, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于播放器",
                           "播放器 v1.0\n\n"
                           "一个简单的媒体播放器，支持常见的音视频格式。\n"
                           "基于 Qt6.5.3 开发。");
    });

    helpMenu->addAction(shortcutsAct);
    helpMenu->addAction(aboutAct);


    fileMenu->addSeparator();

    // 添加历史菜单到播放菜单
    QMenu *historyMenu = playMenu->addMenu("播放历史(&H)");
    connect(historyMenu, &QMenu::aboutToShow, this, [this, historyMenu]() {
        historyMenu->clear();

        if (playHistory.isEmpty()) {
            QAction *emptyAct = historyMenu->addAction("暂无播放历史");
            emptyAct->setEnabled(false);
        } else {
            // 添加最近播放的文件
            for (const auto &history : playHistory) {
                QString timeStr = history.playTime.toString("yyyy-MM-dd hh:mm");
                QString text = QString("%1 (%2)").arg(history.fileName, timeStr);

                QAction *action = historyMenu->addAction(text);
                connect(action, &QAction::triggered, this, [this, history]() {
                    playFile(history.filePath);
                    // 恢复上次播放位置
                    QTimer::singleShot(100, this, [this, history]() {
                        mediaPlayer->setPosition(history.lastPosition);
                    });
                });
            }

            historyMenu->addSeparator();
            QAction *clearAct = historyMenu->addAction("清除历史记录");
            connect(clearAct, &QAction::triggered, this, &Player::clearHistory);
        }
    });

    QMenu *streamMenu = playMenu->addMenu("网络流(&N)");

    QAction *openStreamAct = new QAction("打开网络流(&O)", this);
    connect(openStreamAct, &QAction::triggered, this, &Player::openStreamUrl);

    QAction *addStreamAct = new QAction("添加到播放列表(&A)", this);
    connect(addStreamAct, &QAction::triggered, this, &Player::addStreamToPlaylist);

    // 添加最近播放的流媒体子菜单
    QMenu *recentStreamsMenu = streamMenu->addMenu("最近播放(&R)");
    connect(recentStreamsMenu, &QMenu::aboutToShow, this, [this, recentStreamsMenu]() {
        recentStreamsMenu->clear();
        for (const QString &url : recentStreams) {
            QAction *action = recentStreamsMenu->addAction(url);
            connect(action, &QAction::triggered, this, [this, url]() {

                mediaPlayer->setMedia(QUrl(url));
                 qDebug() << "开始播放啦啦啦啦"<<QUrl(url);
                   uploader->upload(url);
                mediaPlayer->play();
            });
        }
        if (!recentStreams.isEmpty()) {
            recentStreamsMenu->addSeparator();
            QAction *clearAct = recentStreamsMenu->addAction("清除历史记录");
            connect(clearAct, &QAction::triggered, this, [this]() {
                recentStreams.clear();
                saveStreamHistory();
            });
        }
    });

    streamMenu->addAction(openStreamAct);
    streamMenu->addAction(addStreamAct);
    streamMenu->addSeparator();
    streamMenu->addMenu(recentStreamsMenu);


}

// 创建播放速度菜单
void Player::createPlaybackRateMenu()
{
    if (!playbackRateMenu) return;

    rateGroup = new QActionGroup(this);

    QList<double> rates = {0.5, 0.75, 1.0, 1.25, 1.5, 2.0};
    QList<QString> rateTexts = {"0.5x", "0.75x", "1.0x (正常)", "1.25x", "1.5x", "2.0x"};

    for (int i = 0; i < rates.size(); ++i) {
        QAction *rateAction = new QAction(rateTexts[i], this);
        rateAction->setCheckable(true);
        rateAction->setData(rates[i]);
        if (qFuzzyCompare(rates[i], 1.0)) {
            rateAction->setChecked(true);
        }
        rateGroup->addAction(rateAction);
        playbackRateMenu->addAction(rateAction);
    }

    connect(rateGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        double rate = action->data().toDouble();
        setPlaybackRate(rate);
    });
}

// 设置播放速率
void Player::setPlaybackRate(double rate)
{
    if (mediaPlayer) {
        mediaPlayer->setPlaybackRate(rate);
    }
}

// 打开文件对话框并选择媒体文件
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
            playlistWidget->addItem(item);
        }

        // 自动保存到默认播放列表
        saveDefaultPlaylist();

        // 播放第一个文件
        playFile(fileNames.first());
        playlistWidget->setCurrentRow(playlistWidget->count() - fileNames.size());
    }
}

// 播放指定文件路径的媒体
void Player::playFile(const QString& filePath)
{
    if (!filePath.isEmpty()) {
        // 使用Qt5兼容的媒体加载方式
        mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(filePath)));

        // 恢复上次播放位置
        if (lastPositions.contains(filePath)) {
            mediaPlayer->setPosition(lastPositions[filePath]);
        }
        qDebug() << "开始播放啦啦啦啦"<<QUrl(filePath);
          uploader->upload(filePath);
        mediaPlayer->play();
        setWindowTitle("- 拉不拉几播放器 - " + QFileInfo(filePath).fileName());

        // 高亮当前播放项
        for (int i = 0; i < playlistWidget->count(); ++i) {
            QListWidgetItem* item = playlistWidget->item(i);
            if (item->data(Qt::UserRole).toString() == filePath) {
                playlistWidget->setCurrentItem(item);
                break;
            }
        }

        // 添加到历史记录
        addToHistory(filePath);
    }
}

// 添加文件到播放列表
void Player::addToPlaylist()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      "添加到播放列表",
                                                      "",
                                                      "媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.wav);;所有文件 (*.*)");

    for (const QString& file : files) {
        QListWidgetItem* item = new QListWidgetItem(QFileInfo(file).fileName());
        item->setData(Qt::UserRole, file);
        playlistWidget->addItem(item);
    }

    // 自动保存到默认播放列表
    saveDefaultPlaylist();
}

// 处理播放列表项双击事件
void Player::playlistItemDoubleClicked(QListWidgetItem* item)
{
    playFile(item->data(Qt::UserRole).toString());
}

// 保存播放列表到文件
void Player::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存播放列表",
                                                    "",
                                                    "播放列表 (*.m3u)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (int i = 0; i < playlistWidget->count(); ++i) {
                out << playlistWidget->item(i)->data(Qt::UserRole).toString() << "\n";
            }
            currentPlaylistFile = fileName;
        }
    }
}

// 从文件加载播放列表
void Player::loadPlaylist()
{
    QString fileName = currentPlaylistFile;
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getOpenFileName(this,
                                                "加载播放列表",
                                                "",
                                                "播放列表 (*.m3u)");
    }

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            playlistWidget->clear();
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString filePath = in.readLine().trimmed();
                if (!filePath.isEmpty() && QFile::exists(filePath)) {
                    QListWidgetItem* item = new QListWidgetItem(QFileInfo(filePath).fileName());
                    item->setData(Qt::UserRole, filePath);
                    playlistWidget->addItem(item);
                }
            }
            currentPlaylistFile = fileName;
        }
    }
}

// 从播放列表移除选中项
void Player::removeFromPlaylist()
{
    QList<QListWidgetItem*> items = playlistWidget->selectedItems();
    for (QListWidgetItem* item : items) {
        delete playlistWidget->takeItem(playlistWidget->row(item));
    }

    // 自动保存到默认播放列表
    saveDefaultPlaylist();
}

// 切换播放列表显示状态
void Player::togglePlaylist()
{
    playlistDock->setVisible(!playlistDock->isVisible());
    togglePlaylistButton->setIcon(style()->standardIcon(
        playlistDock->isVisible() ? QStyle::SP_FileDialogDetailedView : QStyle::SP_FileDialogListView
        ));
}

// 显示播放列表右键上下文菜单
void Player::showPlaylistContextMenu(const QPoint &pos)
{
    QList<QListWidgetItem*> selectedItems = playlistWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    QMenu contextMenu(this);

    // 播放选中项
    if (selectedItems.count() == 1) {
        QAction *playAction = new QAction("播放", this);
        connect(playAction, &QAction::triggered, this, [this, selectedItems]() {
            playFile(selectedItems.first()->data(Qt::UserRole).toString());
        });
        contextMenu.addAction(playAction);
        contextMenu.addSeparator();
    }

    // 删除选中项
    QAction *deleteAction = new QAction("删除选中项", this);
    connect(deleteAction, &QAction::triggered, this, [this, selectedItems]() {
        QString message = selectedItems.count() == 1
                              ? "确定要删除选中的文件吗？"
                              : QString("确定要删除选中的 %1 个文件吗？").arg(selectedItems.count());

        if (QMessageBox::question(this, "确认删除", message) == QMessageBox::Yes) {
            for (QListWidgetItem* item : selectedItems) {
                delete playlistWidget->takeItem(playlistWidget->row(item));
            }
            saveDefaultPlaylist();  // 保存更改
        }
    });
    contextMenu.addAction(deleteAction);

    // 添加排序菜单
    contextMenu.addSeparator();
    QMenu *sortMenu = contextMenu.addMenu("排序");

    QAction *sortByNameAct = new QAction("按名称排序", this);
    connect(sortByNameAct, &QAction::triggered, this, [this]() {
        playlistWidget->sortItems(Qt::AscendingOrder);
        saveDefaultPlaylist();
    });

    QAction *sortByTimeAct = new QAction("按添加时间排序", this);
    connect(sortByTimeAct, &QAction::triggered, this, [this]() {
        // 恢复到添加顺序
        loadDefaultPlaylist();
    });

    sortMenu->addAction(sortByNameAct);
    sortMenu->addAction(sortByTimeAct);

    // 显示文件信息
    if (selectedItems.count() == 1) {
        contextMenu.addSeparator();
        QAction *infoAction = new QAction("文件信息", this);
        connect(infoAction, &QAction::triggered, this, [this, selectedItems]() {
            QFileInfo fileInfo(selectedItems.first()->data(Qt::UserRole).toString());
            QString info = QString("文件名：%1\n大小：%2 KB\n路径：%3")
                               .arg(fileInfo.fileName())
                               .arg(fileInfo.size() / 1024)
                               .arg(fileInfo.absolutePath());
            QMessageBox::information(this, "文件信息", info);
        });
        contextMenu.addAction(infoAction);
    }

    contextMenu.exec(playlistWidget->mapToGlobal(pos));
}


// 添加事件过滤器，事件过滤器处理视频窗口和进度条事件
bool Player::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == videoWidget && event->type() == QEvent::MouseButtonDblClick) {
        toggleFullScreen();
        return true;
    }
    if (obj == playlistWidget->viewport()) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent->mimeData()->hasUrls()) {
                dragEvent->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
            QList<QUrl> urls = dropEvent->mimeData()->urls();
            for (const QUrl &url : urls) {
                QString filePath = url.toLocalFile();
                if (QFileInfo(filePath).exists()) {
                    QListWidgetItem* item = new QListWidgetItem(QFileInfo(filePath).fileName());
                    item->setData(Qt::UserRole, filePath);
                    playlistWidget->addItem(item);
                }
            }
            saveDefaultPlaylist();
            return true;
        }
    }
    if (obj == ui->progressSlider) {
        switch (event->type()) {
        case QEvent::MouseMove: {
            // 停止定时器（如果正在运行）
            previewHideTimer->stop();

            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QPoint globalPos = mouseEvent->globalPos();  // 使用globalPos()

            // 计算鼠标位置对应的视频时间
            double ratio = static_cast<double>(globalPos.x()) / ui->progressSlider->width();
            qint64 position = ratio * mediaPlayer->duration();

            // 获取进度条的全局位置
            QPoint sliderPos = ui->progressSlider->mapToGlobal(QPoint(0, 0));

            // 计算预览窗口位置
            int previewX = globalPos.x() - thumbnailPreview->width() / 2;
            int previewY = sliderPos.y() - thumbnailPreview->height() - 5; // 始终在进度条上方5像素

            // 确保预览窗口不会超出屏幕左右边界
            QScreen *screen = QGuiApplication::screenAt(globalPos);
            if (screen) {
                QRect screenGeometry = screen->geometry();

                // 只调整水平方向
                if (previewX < screenGeometry.left()) {
                    previewX = screenGeometry.left();
                } else if (previewX + thumbnailPreview->width() > screenGeometry.right()) {
                    previewX = screenGeometry.right() - thumbnailPreview->width();
                }
            }

            // 更新预览位置
            thumbnailPreview->move(previewX, previewY);

            // 显示预览
            showThumbnailPreview(position);
            break;
        }
        case QEvent::Leave:
            // 启动定时器，2秒后隐藏预览
            previewHideTimer->start();
            break;
        default:
            break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// 切换全屏显示模式
void Player::toggleFullScreen()
{
    if (!isFullScreen) {
        // 进入全屏
        isFullScreen = true;
        setWindowState(windowState() | Qt::WindowFullScreen);
        ui->controlWidget->hide();  // 隐藏控制栏
        playlistDock->hide();
        menuBar()->hide();
    } else {
        // 退出全屏
        isFullScreen = false;
        setWindowState(windowState() & ~Qt::WindowFullScreen);
        ui->controlWidget->show();  // 显示控制栏
        menuBar()->show();
        if (playlistDock->isVisible()) {
            playlistDock->show();
        }
    }
}

// 添加保存默认播放列表函数，保存默认播放列表到本地文件
void Player::saveDefaultPlaylist()
{
    QFile file(defaultPlaylistFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < playlistWidget->count(); ++i) {
            out << playlistWidget->item(i)->data(Qt::UserRole).toString() << "\n";
        }
    }
}

// 添加加载默认播放列表函数，从本地文件加载默认播放列表
void Player::loadDefaultPlaylist()
{
    QFile file(defaultPlaylistFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        playlistWidget->clear();
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString filePath = in.readLine().trimmed();
            if (!filePath.isEmpty() && QFile::exists(filePath)) {
                QListWidgetItem* item = new QListWidgetItem(QFileInfo(filePath).fileName());
                item->setData(Qt::UserRole, filePath);
                playlistWidget->addItem(item);
            }
        }
    }
}

// 保存应用程序设置
void Player::saveSettings()
{
    QSettings settings;

    // 保存窗口状态
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // 保存音量
    settings.setValue("volume", ui->volumeSlider->value());

    // 保存播放模式
    settings.setValue("playMode", playMode);

    // 保存播放列表状态
    settings.setValue("playlistVisible", playlistDock->isVisible());

    // 保存最后播放的文件和位置
    if (!mediaPlayer->media().isNull()) {
        QUrl mediaUrl = mediaPlayer->media().canonicalUrl();
        settings.setValue("lastFile", mediaUrl.toString());
        settings.setValue("lastPosition", mediaPlayer->position());
    }
}

// 加载应用程序设置
void Player::loadSettings()
{
    QSettings settings;

    // 恢复窗口状态
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    // 恢复音量
    ui->volumeSlider->setValue(settings.value("volume", 50).toInt());

    // 恢复播放模式
    playMode = static_cast<PlayMode>(settings.value("playMode", Sequential).toInt());

    // 恢复播放列表显示状态
    playlistDock->setVisible(settings.value("playlistVisible", true).toBool());

    // 恢复最后播放的文件
    QString lastFile = settings.value("lastFile").toString();
    if (!lastFile.isEmpty() && QFile::exists(lastFile)) {
        lastPlayedFile = lastFile;
        QTimer::singleShot(0, this, [this, &settings]() {
            playFile(lastPlayedFile);
            mediaPlayer->setPosition(settings.value("lastPosition", 0).toLongLong());
            mediaPlayer->pause();
        });
    }

    // 检查是否启用系统主题跟踪
    bool isSystemTracking = settings.value("systemThemeTracking", false).toBool();
    if (isSystemTracking) {
        Theme::instance().enableSystemThemeTracking(this);
    }
}


void Player::closeEvent(QCloseEvent *event)
{
    saveSettings();
    savePlayHistory();
    QMainWindow::closeEvent(event);
}

// 播放下一曲目
void Player::playNext()
{
    if (playlistWidget->count() > 0) {
        int nextRow = playlistWidget->currentRow() + 1;
        if (nextRow >= playlistWidget->count()) {
            nextRow = playMode == Loop ? 0 : -1;
        }
        if (nextRow >= 0) {
            playlistWidget->setCurrentRow(nextRow);
            playFile(playlistWidget->currentItem()->data(Qt::UserRole).toString());
        }
    }
}

// 播放上一曲目
void Player::playPrevious()
{
    if (playlistWidget->count() > 0) {
        int prevRow = playlistWidget->currentRow() - 1;
        if (prevRow < 0) {
            prevRow = playMode == Loop ? playlistWidget->count() - 1 : -1;
        }
        if (prevRow >= 0) {
            playlistWidget->setCurrentRow(prevRow);
            playFile(playlistWidget->currentItem()->data(Qt::UserRole).toString());
        }
    }
}

// 设置播放模式（顺序/循环/随机等）
void Player::setPlayMode(PlayMode mode)
{
    playMode = mode;
    updatePlayModeIcon();
}

// 添加切换播放模式的函数
void Player::togglePlayMode()
{
    // 循环切换播放模式
    playMode = static_cast<PlayMode>((static_cast<int>(playMode) + 1) % 4);
    updatePlayModeIcon();
}

// 更新播放模式按钮图标
void Player::updatePlayModeIcon()
{
    QIcon icon;
    QString tooltip;

    switch (playMode) {
    case Sequential:
        icon = QIcon(QStringLiteral(":/icons/sequential.svg"));
        tooltip = "顺序播放";
        break;
    case Loop:
        icon = QIcon(QStringLiteral(":/icons/loop.svg"));
        tooltip = "列表循环";
        break;
    case SingleLoop:
        icon = QIcon(QStringLiteral(":/icons/single-loop.svg"));
        tooltip = "单曲循环";
        break;
    case Random:
        icon = QIcon(QStringLiteral(":/icons/random.svg"));
        tooltip = "随机播放";
        break;
    }

    // 确保图标大小合适
    QSize iconSize(24, 24);
    ui->playModeButton->setIconSize(iconSize);
    ui->playModeButton->setIcon(icon);
    ui->playModeButton->setToolTip(tooltip);
}

// 设置视频宽高比
void Player::setAspectRatio(double ratio)
{
    videoAspectRatio = ratio;
    if (ratio == 0.0) {
        videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    } else {
        if (mediaPlayer->isMetaDataAvailable()) {
            QSize videoSize = mediaPlayer->metaData("Resolution").value<QSize>();
            if (!videoSize.isEmpty()) {
                int w = videoWidget->width();
                int h = int(w / ratio);

                // 确保高度不超过视频窗口
                if (h > videoWidget->height()) {
                    h = videoWidget->height();
                    w = int(h * ratio);
                }

                // 计算居中位置
                int x = (videoWidget->width() - w) / 2;
                int y = (videoWidget->height() - h) / 2;

                // 设置视频显示区域
                videoWidget->setGeometry(x, y, w, h);
            }
        }
    }
}

// 添加窗口大小变化事件处理
void Player::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (videoAspectRatio != 0.0) {
        setAspectRatio(videoAspectRatio);
    }
    if (!event->size().isEmpty()) {
            emit sizeSyncRequested(event->size());
        }
}

// ... existing code ...

// void Player::setAspectRatio(double ratio)
// {
//     videoAspectRatio = ratio;
//     if (qFuzzyCompare(ratio, 0.0)) {
//         // 自动模式：保持视频原始宽高比
//         videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
//     } else {
//         // Qt5兼容的宽高比设置方式
//         videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        
//         // 通过调整布局实现居中
//         QLayout* layout = ui->videoLayout->layout();
//         if (layout) {
//             layout->setAlignment(videoWidget, Qt::AlignCenter);
//         }
        
//         // 强制更新布局
//         ui->videoLayout->update();
//     }
// }

// void Player::resizeEvent(QResizeEvent *event)
// {
//     QMainWindow::resizeEvent(event);
//     if (!qFuzzyCompare(videoAspectRatio, 0.0)) {
//         // 计算并设置视频尺寸
//         QSize containerSize = ui->videoLayout->size();
//         QSize videoSize = containerSize;
        
//         if (videoAspectRatio > 0) {
//             videoSize.setHeight(containerSize.width() / videoAspectRatio);
//             if (videoSize.height() > containerSize.height()) {
//                 videoSize.setHeight(containerSize.height());
//                 videoSize.setWidth(containerSize.height() * videoAspectRatio);
//             }
//         }
        
//         videoWidget->resize(videoSize);
//         videoWidget->move((containerSize.width() - videoSize.width())/2,
//                          (containerSize.height() - videoSize.height())/2);
//     }
    
//     // 保持控制栏可见性一致
//     if (isFullScreen) {
//         ui->controlWidget->hide();
//     } else {
//         ui->controlWidget->show();
//     }
// }

// ... existing code ...


//截图功能
void Player::captureScreenshot()
{
    // 移除元数据检查，直接使用视频探针
    if (videoProbe && videoProbe->isActive()) {
        // 使用QVideoProbe捕获帧
        qDebug() << "catch success\n";
        connect(videoProbe, &QVideoProbe::videoFrameProbed, this, [=](const QVideoFrame &frame) {
            // 确保只捕获一次
            videoProbe->disconnect(this);
            qDebug() << "connect success\n";

            QVideoFrame cloneFrame(frame);
            qDebug() << "1";
            if (cloneFrame.map(QAbstractVideoBuffer::ReadOnly)) {
                qDebug() << "2";
                QImage image(
                    cloneFrame.bits(),
                    cloneFrame.width(),
                    cloneFrame.height(),
                    cloneFrame.bytesPerLine(),
                    // 转换像素格式到RGB32
                    QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat())
                    );
                qDebug() << "3";
                if (image.format() != QImage::Format_Invalid) {
                    // 转换为RGB32格式确保兼容性
                    if (image.format() != QImage::Format_RGB32) {
                        image = image.convertToFormat(QImage::Format_RGB32);
                    }
                    image = image.mirrored(false, true);
                    qDebug() << "4";

                    // 使用 QMetaObject::invokeMethod 确保在主线程中调用
                    QMetaObject::invokeMethod(this, [this, image]() {
                        QString fileName = QFileDialog::getSaveFileName(this,
                                                                        "保存截图",
                                                                        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                                                                            + "/screenshot.png",
                                                                        "图片文件 (*.png *.jpg)",
                                                                        nullptr,
                                                                        QFileDialog::DontUseNativeDialog); // 不使用系统原生对话框
                        qDebug() << "5";
                        if (!fileName.isEmpty()) {
                            image.save(fileName);
                        }
                    }, Qt::QueuedConnection);
                }
                qDebug() << "6";
                cloneFrame.unmap();
            }
        });
    }
}

//加载播放历史
void Player::loadPlayHistory()
{
    QFile file(historyFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_12);  // 设置Qt5数据流版本

        // 清除现有历史记录
        playHistory.clear();

        // 读取记录数量
        qint32 count;
        in >> count;

        // 读取每条记录
        for (qint32 i = 0; i < count; ++i) {
            PlayHistory history;
            in >> history;
            playHistory.append(history);
        }
    }
}

//保存播放历史
void Player::savePlayHistory()
{
    QFile file(historyFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_12);  // 设置Qt5数据流版本

        // 写入记录数量
        out << qint32(playHistory.size());

        // 写入每条记录
        for (const auto &history : playHistory) {
            out << history;
        }
    }
}

//添加播放历史
void Player::addToHistory(const QString &filePath)
{
    // 检查是否已存在
    auto it = std::find_if(playHistory.begin(), playHistory.end(),
                           [&filePath](const PlayHistory &h) { return h.filePath == filePath; });

    PlayHistory history;
    history.filePath = filePath;
    history.fileName = QFileInfo(filePath).fileName();
    history.playTime = QDateTime::currentDateTime();
    history.duration = mediaPlayer->duration();
    history.lastPosition = mediaPlayer->position();

    if (it != playHistory.end()) {
        // 更新现有记录
        *it = history;
    } else {
        // 添加新记录
        playHistory.prepend(history);
        // 限制历史记录数量
        while (playHistory.size() > 50) {
            playHistory.removeLast();
        }
    }

    savePlayHistory();
}

// 显示播放历史菜单
void Player::showHistoryMenu()
{
    QMenu historyMenu(this);

    // 添加最近播放的文件
    for (const auto &history : playHistory) {
        QString timeStr = history.playTime.toString("yyyy-MM-dd hh:mm");
        QString text = QString("%1 (%2)").arg(history.fileName, timeStr);

        QAction *action = historyMenu.addAction(text);
        connect(action, &QAction::triggered, this, [this, history]() {
            playFile(history.filePath);
            // 恢复上次播放位置
            QTimer::singleShot(100, this, [this, history]() {
                mediaPlayer->setPosition(history.lastPosition);
            });
        });
    }

    if (!playHistory.isEmpty()) {
        historyMenu.addSeparator();
        QAction *clearAct = historyMenu.addAction("清除历史记录");
        connect(clearAct, &QAction::triggered, this, &Player::clearHistory);
    }

    historyMenu.exec(QCursor::pos());
}

// 清除播放历史记录
void Player::clearHistory()
{
    if (QMessageBox::question(this, "确认", "确定要清除所有播放历史记录吗？")
        == QMessageBox::Yes) {
        playHistory.clear();
        savePlayHistory();
    }
}

// 显示进度条缩略图预览
void Player::showThumbnailPreview(int position)
{
    if (!mediaPlayer || mediaPlayer->media().isNull()) {
        thumbnailPreview->hide();
        return;
    }

    // 使用QVideoProbe替代QVideoSink
    QVideoProbe *previewProbe = new QVideoProbe(this);
    if (previewProbe->setSource(previewPlayer)) {
        connect(previewProbe, &QVideoProbe::videoFrameProbed, this, [=](const QVideoFrame &frame){
            // 处理视频帧...
            previewProbe->deleteLater();
        });
    }
}

// 打开网络流媒体URL
void Player::openStreamUrl()
{
    bool ok;
    QString url = QInputDialog::getText(this, "打开网络流",
                                        "请输入流媒体URL：",
                                        QLineEdit::Normal,
                                        "http://", &ok);

    if (ok && !url.isEmpty()) {
        // 验证URL格式
        QUrl streamUrl(url);
        if (!streamUrl.isValid()) {
            QMessageBox::warning(this, "错误", "无效的URL格式");
            return;
        }

        // 添加到最近播放列表
        if (!recentStreams.contains(url)) {
            recentStreams.prepend(url);
            // 限制最近播放列表大小
            while (recentStreams.size() > 10) {
                recentStreams.removeLast();
            }
            saveStreamHistory();
        }

        // 播放流媒体
        mediaPlayer->setMedia(QUrl(url));
         qDebug() << "开始播放啦啦啦啦"<<QUrl(url);
          uploader->upload(url);
        mediaPlayer->play();

        // 更新界面
        setWindowTitle("视频播放器 - " + url);
    }
}

// 添加网络流到播放列表
void Player::addStreamToPlaylist()
{
    bool ok;
    QString url = QInputDialog::getText(this, "添加网络流",
                                        "请输入流媒体URL：",
                                        QLineEdit::Normal,
                                        "http://", &ok);

    if (ok && !url.isEmpty()) {
        QUrl streamUrl(url);
        if (!streamUrl.isValid()) {
            QMessageBox::warning(this, "错误", "无效的URL格式");
            return;
        }

        // 添加到播放列表
        QListWidgetItem* item = new QListWidgetItem(url);
        item->setData(Qt::UserRole, url);
        playlistWidget->addItem(item);

        // 保存播放列表
        saveDefaultPlaylist();
    }
}

// 保存流媒体播放历史
void Player::saveStreamHistory()
{
    QFile file(streamHistoryFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_12);  // 设置Qt5数据流版本
        out << recentStreams;
    }
}

// 加载流媒体播放历史
void Player::loadStreamHistory()
{
    QFile file(streamHistoryFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_12);  // 设置Qt5数据流版本
        in >> recentStreams;
    }
}



void Player::onThemeChanged(QAction* action)
{
    if (!action) return;

    QString theme = action->data().toString();
    
    if(theme == "system") {
        Theme::instance().enableSystemThemeTracking(this);
        QString theme = Theme::instance().getThemeFunction();

        bool isDarkTheme = (theme == "dark");
        m_titleBar->updateStyle(isDarkTheme);
        emit change_tepy(!isDarkTheme);
    } else {
        Theme::instance().disableSystemThemeTracking();
        Theme::instance().applyTheme(theme, this);
        
        // 更新标题栏样式
        m_titleBar->updateStyle(theme == "dark");

        // 更新播放列表样式
        if (theme == "dark") {
            Theme::instance().setupPlaylistStyle(playlistWidget);
        } else {
            playlistWidget->setStyleSheet("");
        }
        
        // 更新视频窗口背景颜色
        QColor bgColor = (theme == "dark") ? Qt::black : Qt::white;
        
        // 通过调整窗口尺寸强制重绘
        QSize tmpSize = videoWidget->size();
        videoWidget->resize(tmpSize + QSize(1, 1));
        videoWidget->resize(tmpSize);
        
        ui->controlWidget->style()->unpolish(ui->controlWidget);
        ui->controlWidget->style()->polish(ui->controlWidget);
        ui->controlWidget->update();

        //发射给另一个文件  主页那里的信号
        bool isDarkTheme = (theme == "dark");
        m_titleBar->updateStyle(isDarkTheme);
        emit change_tepy(!isDarkTheme);
    }
}


// 添加toggleMute函数实现
void Player::toggleMute()
{
    if (ui->volumeSlider->value() > 0) {
        // 保存当前音量并静音
        lastVolume = ui->volumeSlider->value();
        ui->volumeSlider->setValue(0);
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        // 恢复之前保存的音量
        ui->volumeSlider->setValue(lastVolume > 0 ? lastVolume : 50);
        ui->volumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

// 添加showCurrentMediaInfo函数实现
void Player::showCurrentMediaInfo()
{
    // 检查当前是否有媒体播放
    if (mediaPlayer->media().isNull()) {
        QMessageBox::information(this, "文件信息", "当前没有播放任何文件");
        return;
    }

    // 获取当前播放的媒体URL
    QUrl mediaUrl = mediaPlayer->media().canonicalUrl();
    QString filePath = mediaUrl.isLocalFile() ? mediaUrl.toLocalFile() : mediaUrl.toString();

    if (mediaUrl.isLocalFile()) {
        // 本地文件信息
        QFileInfo fileInfo(filePath);
        QString info = QString("文件名：%1\n大小：%2 KB\n路径：%3")
                           .arg(fileInfo.fileName())
                           .arg(fileInfo.size() / 1024)
                           .arg(fileInfo.absolutePath());

        // 尝试获取媒体元数据
        if (mediaPlayer->isMetaDataAvailable()) {
            // 分辨率
            QSize resolution = mediaPlayer->metaData("Resolution").value<QSize>();
            if (!resolution.isEmpty()) {
                info += QString("\n分辨率：%1x%2").arg(resolution.width()).arg(resolution.height());
            }

            // 视频时长
            qint64 duration = mediaPlayer->duration();
            if (duration > 0) {
                info += QString("\n时长：%1").arg(formatTime(duration));
            }

            // 视频编解码器
            QString videoCodec = mediaPlayer->metaData("VideoCodec").toString();
            if (!videoCodec.isEmpty()) {
                info += QString("\n视频编码：%1").arg(videoCodec);
            }

            // 音频编解码器
            QString audioCodec = mediaPlayer->metaData("AudioCodec").toString();
            if (!audioCodec.isEmpty()) {
                info += QString("\n音频编码：%1").arg(audioCodec);
            }
        }

        QMessageBox::information(this, "文件信息", info);
    } else {
        // 网络流信息
        QString info = QString("URL：%1").arg(mediaUrl.toString());

        // 尝试获取流媒体元数据
        if (mediaPlayer->isMetaDataAvailable()) {
            // 视频时长
            qint64 duration = mediaPlayer->duration();
            if (duration > 0) {
                info += QString("\n时长：%1").arg(formatTime(duration));
            }
        }

        QMessageBox::information(this, "流媒体信息", info);
    }
}
