#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QTimer>
#include <QLabel>
#include <QMap>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

// 前向声明
class Player;
class MenuControl;
class PlaylistManager;

class MediaControl : public QObject
{
    Q_OBJECT

public:
    //预览状态枚举
    enum class PreviewState { Hidden, Showing, Visible, Hiding };
    
    explicit MediaControl(QObject *parent = nullptr);
    ~MediaControl();

    // 初始化媒体控制
    void setupMediaControl(Player *player, QMediaPlayer *mediaPlayer, 
                          QAudioOutput *audioOutput, QVideoWidget *videoWidget,
                          MenuControl *menuControl, PlaylistManager *playlistManager);

    // 播放控制
    void play();
    void pause();
    void stop();
    void playFile(const QString& filePath);
    void togglePlayPause();
    void forward(qint64 msec = 10000);
    void backward(qint64 msec = 10000);
    void setPosition(qint64 position);
    void setVolume(int volume);
    void toggleMute();
    void setPlaybackRate(double rate);
    void toggleFullScreen();
    void captureScreenshot();
    void playNext();
    void playPrevious();

    // 缩略图预览
    void showThumbnailPreview(int position);
    void showThumbnailPreview(int position, const QPoint& mousePos, const QPoint& sliderPos);
    void hideThumbnailPreview();

    // 获取当前播放状态
    QMediaPlayer::PlaybackState playbackState() const;
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isMuted() const;
    int volume() const;
    qint64 position() const;
    qint64 duration() const;
    QString formatTime(qint64 milliseconds);


    void setAspectRatio(double ratio);
    

    void handleResize();

    // 获取媒体播放器
    QMediaPlayer* getMediaPlayer() const { return m_mediaPlayer; }
    QAudioOutput* getAudioOutput() const { return m_audioOutput; }
    QVideoWidget* getVideoWidget() const { return m_videoWidget; }

    // 保存和加载设置
    void saveSettings();
    void loadSettings();

    // 获取和设置上次播放位置
    void setLastPosition(const QString &filePath, qint64 position);
    qint64 getLastPosition(const QString &filePath) const;

    // 获取当前预览状态
    PreviewState getPreviewState() const { return m_previewState; }

public slots:
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void updatePlayIcon(QMediaPlayer::PlaybackState state);

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void fullScreenChanged(bool fullScreen);

private:
    Player *m_player;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QVideoWidget *m_videoWidget;
    MenuControl *m_menuControl;
    PlaylistManager *m_playlistManager;

    bool m_isFullScreen;
    QMap<QString, qint64> m_lastPositions;  // 记录每个文件的上次播放位置
    int m_lastVolume;

    QMediaPlayer *m_previewPlayer;
    QVideoWidget *m_previewVideoWidget;
    QLabel *m_thumbnailPreview;
    QLabel *m_previewTimeLabel;
    PreviewState m_previewState = PreviewState::Hidden;

    //初始化预览播放器
    void setupPreviewPlayer();
};

#endif // MEDIACONTROL_H
