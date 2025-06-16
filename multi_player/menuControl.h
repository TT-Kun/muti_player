#ifndef MENUCONTROL_H
#define MENUCONTROL_H

#include <QObject>
#include <QMenu>
#include <QActionGroup>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QDateTime>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include "playlist_manager.h"


class Player;

struct PlayHistory {
    QString filePath;
    QString fileName;
    QDateTime playTime;
    qint64 duration;
    qint64 lastPosition;
    
    friend QDataStream &operator<<(QDataStream &out, const PlayHistory &history);
    friend QDataStream &operator>>(QDataStream &in, PlayHistory &history);
};

class MenuControl : public QObject
{
    Q_OBJECT

public:
    explicit MenuControl(QObject *parent = nullptr);
    ~MenuControl();
    
    // 初始化菜单控制
    void setupMenuControl(Player *player, QPushButton *settingsButton, 
                         QMediaPlayer *mediaPlayer, QAudioOutput *audioOutput,
                         QVideoWidget *videoWidget, PlaylistManager *playlistManager);
    
    enum PlayMode {
        Sequential,  // 顺序播放
        Loop,       // 列表循环
        SingleLoop, // 单曲循环
        Random      // 随机播放
    };
    
    // 设置和获取播放模式
    void setPlayMode(PlayMode mode);
    PlayMode getPlayMode() const;
    
    // 下一播放模式
    void togglePlayMode();
    
    // 倍速
    void setPlaybackRate(double rate);
    
    // 主题相关
    void applyTheme(const QString &themeName);
    QString getCurrentTheme() const;
    void setupPlaylistStyle();
    
    // 历史记录相关
    void loadPlayHistory();
    void savePlayHistory();
    void addToHistory(const QString &filePath, qint64 duration, qint64 position);
    void clearHistory();
    QList<PlayHistory> getPlayHistory() const;
    
    // 截图
    void captureScreenshot();
    
public slots:
    // 显示设置菜单
    void showSettingsMenu();
    void resetAllSettings();  // 添加重置所有设置的方法
    
private:
    Player *m_player;
    QPushButton *m_settingsButton;
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;
    QVideoWidget *m_videoWidget;
    PlaylistManager *m_playlistManager;
    
    // 菜单相关成员变量
    PlayMode m_playMode;
    QString m_currentTheme;
    
    // 播放历史相关
    QList<PlayHistory> m_playHistory;
    QString m_historyFile;
    
    // 创建各种子菜单
    QMenu* createFileMenu();
    QMenu* createPlayMenu();
    QMenu* createHelpMenu();
    QMenu* createPlaybackRateMenu();
    QMenu* createPlayModeMenu();
    QMenu* createThemeMenu();
    QMenu* createHistoryMenu();

signals:
    // 播放模式改变信号
    void playModeChanged(PlayMode mode);
};

#endif // MENUCONTROL_H
