#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QFileDialog>
#include <QMenu>
#include <QActionGroup>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QMap>
#include <QStandardPaths>
#include <QDir>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QDateTime>
#include <QLabel>
#include <QSettings>
#include <QIcon>
#include <QTime>
#include <QPixmap>
#include <QInputDialog>
#include <QLineEdit>
#include <QVideoProbe>
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QVariantMap>
#include "titlebar.h"
#include <QQuickWidget>
#include <QVBoxLayout>

#include "fileuploader_yin.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QMainWindow
{
    Q_OBJECT

public:
    Player(QWidget *parent = nullptr);
    ~Player();
    void  closeEvent(QCloseEvent *event) override;
private slots:
    // 处理播放按钮点击事件
    void on_playButton_clicked();
    // 处理停止按钮点击事件
    void on_stopButton_clicked();
    // 处理快进按钮点击事件
    void on_forwardButton_clicked();
    // 处理快退按钮点击事件
    void on_backwardButton_clicked();
    // 更新播放进度显示
    void updatePosition(qint64 position);
    // 更新总时长显示
    void updateDuration(qint64 duration);
    // 设置播放位置
    void setPosition(int position);
    // 设置音量
    void setVolume(int volume);
    // 更新播放按钮图标状态
    void updatePlayIcon(QMediaPlayer::State state);
    // 打开媒体文件
    void openFile();
    // 设置播放速度
    void setPlaybackRate(double rate);
    void addToPlaylist();                    // 添加到播放列表
    void removeFromPlaylist();               // 从播放列表移除
    void playlistItemDoubleClicked(QListWidgetItem* item); // 双击播放列表项
    void savePlaylist();                     // 保存播放列表
    void loadPlaylist();                     // 加载播放列表
    void togglePlaylist();  // 添加这行
    void showPlaylistContextMenu(const QPoint &pos);  // 保留这个
    // 切换全屏模式
    void toggleFullScreen();  // 添加这行
    // 截图功能
    void captureScreenshot();  // 截图功能
    // 打开网络流媒体
    void openStreamUrl();
    // 添加流媒体到播放列表
    void addStreamToPlaylist();
    // 切换静音状态
    void toggleMute();
    void showCurrentMediaInfo();
    void onThemeChanged(QAction* action);

  ///////////////////////  用来实现大小的双向绑定的
public slots:
    void syncSize(const QSize& size) {
        if (this->size() != size) {
            this->resize(size); // 异步调整
        }
    }
signals:
     void back();//切换至主页
     void sizeSyncRequested(const QSize& size);
     void change_tepy(bool isDarkTheme);

 /////////////////////////  ///
private:
    Ui::Player *ui;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;
    QString formatTime(qint64 milliseconds);
    void createMenus();
    void createPlaybackRateMenu();
    void playFile(const QString& filePath);  // 播放指定文件

    QMenu *playbackRateMenu;
    QActionGroup *rateGroup;
    QDockWidget *playlistDock;              // 播放列表停靠窗口
    QListWidget *playlistWidget;            // 播放列表控件
    QString currentPlaylistFile;             // 当前播放列表文件路径
    QPushButton *togglePlaylistButton;  // 添加这行
    bool isFullScreen;  // 添加这行
    QMap<QString, qint64> lastPositions;  // 记录每个文件的上次播放位置
    QString defaultPlaylistFile;  // 添加默认播放列表文件路径
    void saveDefaultPlaylist();   // 保存默认播放列表
    void loadDefaultPlaylist();   // 加载默认播放列表
    void saveSettings();
    void loadSettings();

    // 播放模式
    enum PlayMode {
        Sequential,  // 顺序播放
        Loop,       // 列表循环
        SingleLoop, // 单曲循环
        Random      // 随机播放
    };

    PlayMode playMode;
    QAction *playModeAction;
    void setPlayMode(PlayMode mode);
    void playNext();
    void playPrevious();
    void togglePlayMode();      // 添加这行
    void updatePlayModeIcon();  // 添加这行

    QString lastPlayedFile;

    // 添加成员
    QMenu *aspectRatioMenu;
    double videoAspectRatio = 0.0;
    void setAspectRatio(double ratio);

    // 播放历史记录结构
    struct PlayHistory {
        QString filePath;
        QString fileName;
        QDateTime playTime;
        qint64 duration;
        qint64 lastPosition;
    };
    // 在 PlayHistory 结构体定义后添加
    friend QDataStream &operator<<(QDataStream &out, const PlayHistory &history)
    {
        out << history.filePath
            << history.fileName
            << history.playTime
            << history.duration
            << history.lastPosition;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, PlayHistory &history)
    {
        in >> history.filePath
            >> history.fileName
            >> history.playTime
            >> history.duration
            >> history.lastPosition;
        return in;
    }

    QList<PlayHistory> playHistory;
    QString historyFile;  // 历史记录文件路径

    void savePlayHistory();
    void loadPlayHistory();
    void addToHistory(const QString &filePath);
    void showHistoryMenu();
    void clearHistory();

    void generateThumbnail();
    void showThumbnailPreview(int position);
    QLabel *thumbnailPreview;

    // 添加预览相关成员
    QMediaPlayer *previewPlayer;
    QVideoWidget *previewVideoWidget;
    QLabel *previewTimeLabel;

    // 添加预览关闭定时器
    QTimer *previewHideTimer;

    // 网络流媒体相关
    QList<QString> recentStreams;
    void saveStreamHistory();
    void loadStreamHistory();
    QString streamHistoryFile;  // 流媒体历史记录文件路径

    // 修改主题相关成员
    QMenu *themeMenu = nullptr;
    QActionGroup *themeGroup = nullptr;  // 添加主题选择组
    QString currentTheme;

    int lastVolume = 0;  // 用于存储静音前的音量

    QVideoProbe *videoProbe;  // 用于截图功能

    QGraphicsView *graphicsView;
    QGraphicsVideoItem *videoItem;

    int videoRotation;

    void updateVideoAreaBackground();
    
    // 添加系统主题相关成员和方法
    bool systemThemeTracking = false;
    
    TitleBar *m_titleBar;
   //QQuickWidget *m_qmlWidget; // 新增QML视图控件,悬浮小人

   FileUploader *uploader;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateVideoRotation();
};

#endif // PLAYER_H 
