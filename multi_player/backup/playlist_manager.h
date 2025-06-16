#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#include <QObject>
#include <QListWidget>
#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

class PlaylistManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistManager(QObject *parent = nullptr);
    ~PlaylistManager();

    // 初始化播放列表UI组件
    void setupPlaylist(QWidget *container, QListWidget *playlist, QPushButton *toggleButton);
    
    // 播放列表操作
    void addToPlaylist();
    void removeFromPlaylist();
    // 已删除保存和加载播放列表功能
    void togglePlaylist();
    void showPlaylistContextMenu(const QPoint &pos);
    
    // 获取播放列表项
    QListWidgetItem* getCurrentItem() const;
    int getPlaylistCount() const;
    QListWidgetItem* getItemAt(int index) const;
    void setCurrentRow(int row);
    int getCurrentRow() const;
    
    // 保存和加载默认播放列表
    void saveDefaultPlaylist();
    void loadDefaultPlaylist();
    
    // 获取UI组件
    QWidget* getPlaylistContainer() const { return playlistContainer; }
    QListWidget* getPlaylistWidget() const { return playlistWidget; }
    
    // 检查播放列表是否可见
    bool isPlaylistVisible() const { return playlistContainer ? playlistContainer->isVisible() : false; }
    
signals:
    void playlistItemDoubleClicked(QListWidgetItem* item);
    void playRequested(const QString &filePath);
    
private:
    QWidget *playlistContainer;        // 播放列表容器
    QListWidget *playlistWidget;       // 播放列表控件
    QPushButton *togglePlaylistButton; // 切换播放列表按钮
    QString currentPlaylistFile;       // 当前播放列表文件路径
    QString defaultPlaylistFile;       // 默认播放列表文件路径
    
    // 设置播放列表样式
    void setupPlaylistStyle();
};

#endif // PLAYLIST_MANAGER_H