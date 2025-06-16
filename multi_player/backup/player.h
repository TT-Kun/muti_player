#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QFileDialog>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QMap>
#include <QStandardPaths>
#include <QDir>
#include <QMediaMetaData>
#include <QDateTime>
#include <QLabel>
#include <QSlider>
#include <QWheelEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QShortcut>
#include <QCloseEvent>
#include <QWindow>
#include <QScreen>
#include <QSettings>
#include <QDrag>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "playlist_manager.h"
#include "menuControl.h"
#include "mediaControl.h"
#include "volumecontrol.h"
#include "titleBar.h"
#include "MyBorderContainer.h"
#include "clickableslider.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QMainWindow
{
    Q_OBJECT

public:
    Player(QWidget *parent = nullptr);
    ~Player();
    void closeEvent(QCloseEvent *event) override;
    
    // 播放文件的公共方法
    void playFile(const QString& filePath);

private slots:
    void on_playButton_clicked();
    void on_stopButton_clicked();
    void on_forwardButton_clicked();
    void on_backwardButton_clicked();
    void openFile();
    void onPlaylistItemDoubleClicked(QListWidgetItem* item); // 处理播放列表双击事件
    void onPlayRequested(const QString &filePath); // 处理播放请求
    
    // 响应MediaControl的信号
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onVolumeChanged(int volume);
    void onMutedChanged(bool muted);
    void onFullScreenChanged(bool fullScreen);
    
    // 处理窗口状态变化
    void onWindowStateChanged(Qt::WindowState newState);
    void toggleFullScreen(); // 整个窗口的全屏切换
    void videoFullScreen();  // 仅视频区域的全屏切换

private:
    Ui::Player *ui;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;
    
    // 将FramelessHelper替换为MyBorderContainer
    MyBorderContainer *m_borderContainer;
    
    // 自定义标题栏
    TitleBar *m_titleBar;
    
    // 播放列表管理器
    PlaylistManager *playlistManager;
    
    // 菜单控制器
    MenuControl *menuControl;
    
    // 媒体控制器
    MediaControl *mediaControl;
    
    // 音量控制弹出框
    volumeControl *m_volumeControl;
    
    void loadSettings();
    void saveSettings();
    
    // 更新播放模式图标（供MediaControl调用）
    void updatePlayModeIcon();
    
    // 更新标题栏
    void updateWindowTitle(const QString &title);
    
    QString lastPlayedFile;
    double videoAspectRatio = 0.0; // 视频宽高比，0.0表示自动
    bool isFullScreen = false; // 是否全屏

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    
    // 友元类，允许MediaControl和MenuControl访问Player的私有成员
    friend class MediaControl;
    friend class MenuControl;
};

#endif // PLAYER_H
