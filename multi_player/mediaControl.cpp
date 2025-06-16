#include "mediaControl.h"
#include "player.h"
#include "menuControl.h"
#include "playlist_manager.h"
#include <QSettings>
#include <QStyle>
#include <QRandomGenerator>
#include <QScreen>
#include <QGuiApplication>
#include <QFileInfo>
#include <QDebug>

MediaControl::MediaControl(QObject *parent)
    : QObject(parent),
      m_player(nullptr),
      m_mediaPlayer(nullptr),
      m_audioOutput(nullptr),
      m_videoWidget(nullptr),
      m_menuControl(nullptr),
      m_playlistManager(nullptr),
      m_isFullScreen(false),
      m_lastVolume(0),
      m_previewPlayer(nullptr),
      m_previewVideoWidget(nullptr),
      m_thumbnailPreview(nullptr),
      m_previewTimeLabel(nullptr),
      m_previewState(PreviewState::Hidden)
{
}

MediaControl::~MediaControl()
{
    // 安全地删除没有父对象的控件
    if (m_thumbnailPreview) {
        m_thumbnailPreview->deleteLater();
    }
}

void MediaControl::setupMediaControl(Player *player, QMediaPlayer *mediaPlayer, 
                                    QAudioOutput *audioOutput, QVideoWidget *videoWidget,
                                    MenuControl *menuControl, PlaylistManager *playlistManager)
{
    m_player = player;
    m_mediaPlayer = mediaPlayer;
    m_audioOutput = audioOutput;
    m_videoWidget = videoWidget;
    m_menuControl = menuControl;
    m_playlistManager = playlistManager;

    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &MediaControl::updatePosition);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &MediaControl::updateDuration);
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &MediaControl::updatePlayIcon);

    // 媒体播放结束时的处理
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            switch (m_menuControl->getPlayMode()) {
                case MenuControl::SingleLoop:
                    m_mediaPlayer->setPosition(0);
                    m_mediaPlayer->play();
                    break;
                case MenuControl::Loop:
                case MenuControl::Sequential:
                    playNext();
                    break;
                case MenuControl::Random:
                    if (m_playlistManager->getPlaylistCount() > 0) {
                        int randomRow = QRandomGenerator::global()->bounded(m_playlistManager->getPlaylistCount());
                        m_playlistManager->setCurrentRow(randomRow);
                        QListWidgetItem* item = m_playlistManager->getCurrentItem();
                        if (item) {
                            playFile(item->data(Qt::UserRole).toString());
                        }
                    }
                    break;
            }
        }
    });

    // 设置预览播放器
    setupPreviewPlayer();
}

void MediaControl::setupPreviewPlayer()
{
    // 初始化预览播放器
    m_previewPlayer = new QMediaPlayer(this);
    m_previewVideoWidget = new QVideoWidget();
    m_previewPlayer->setVideoOutput(m_previewVideoWidget);

    // 创建预览容器
    m_thumbnailPreview = new QLabel(nullptr);  // 改为顶层窗口
    m_thumbnailPreview->setFixedSize(160, 90);  // 16:9缩略图
    m_thumbnailPreview->setStyleSheet(
        "QLabel {"
        "   background-color: black;"
        "   border: 1px solid #666;"
        "   padding: 1px;"
        "}"
    );
    m_thumbnailPreview->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);  // 设置为工具提示窗口
    
    // 将预览视频小部件设置为m_thumbnailPreview的子对象
    m_previewVideoWidget->setParent(m_thumbnailPreview);
    m_previewVideoWidget->resize(m_thumbnailPreview->size());

    // 创建时间标签
    m_previewTimeLabel = new QLabel(m_thumbnailPreview);
    m_previewTimeLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   background-color: rgba(0, 0, 0, 128);"
        "   padding: 2px;"
        "}"
    );
    m_previewTimeLabel->setAlignment(Qt::AlignCenter);

    m_thumbnailPreview->hide();
}

void MediaControl::play()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->play();
    }
}

void MediaControl::pause()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->pause();
    }
}

void MediaControl::stop()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
        emit positionChanged(0);
    }
}

void MediaControl::togglePlayPause()
{
    if (!m_mediaPlayer->source().isEmpty() || m_playlistManager->getCurrentItem()) {
        switch (m_mediaPlayer->playbackState()) {
            case QMediaPlayer::PlayingState:
                pause();
                break;
            case QMediaPlayer::PausedState:
                play();
                break;
            case QMediaPlayer::StoppedState:
                if (m_mediaPlayer->source().isEmpty() && m_playlistManager->getCurrentItem()) {
                    QListWidgetItem* item = m_playlistManager->getCurrentItem();
                    if (item) {
                        playFile(item->data(Qt::UserRole).toString());
                    }
                } else {
                    m_mediaPlayer->setPosition(0);
                    play();
                }
                break;
        }
    } else {
        // 打开新文件的逻辑需要在Player类中实现
        QMetaObject::invokeMethod(m_player, "openFile");
    }
}

void MediaControl::forward(qint64 msec)
{
    if (m_mediaPlayer) {
        qint64 position = m_mediaPlayer->position() + msec;
        m_mediaPlayer->setPosition(position);
    }
}

void MediaControl::backward(qint64 msec)
{
    if (m_mediaPlayer) {
        qint64 position = m_mediaPlayer->position() - msec;
        if (position < 0) position = 0;
        m_mediaPlayer->setPosition(position);
    }
}

void MediaControl::setPosition(qint64 position)
{
    if (m_mediaPlayer && m_mediaPlayer->isSeekable()) {
        m_mediaPlayer->setPosition(position);
    }
}

void MediaControl::setVolume(int volume)
{
    if (m_audioOutput) {
        m_audioOutput->setVolume(volume / 100.0);
        // 若音量不为0，更新lastVolume
        if (volume > 0) {
            m_lastVolume = volume;
            m_audioOutput->setMuted(false);
        }
        
        emit volumeChanged(volume);
    }
}

void MediaControl::toggleMute()
{
    if (m_audioOutput) {
        if (m_audioOutput->isMuted()) {
            // 取消静音
            m_audioOutput->setMuted(false);
            emit volumeChanged(m_lastVolume);
        } else {
            // 静音
            m_lastVolume = volume();
            m_audioOutput->setMuted(true);
            emit volumeChanged(0);
        }
        emit mutedChanged(m_audioOutput->isMuted());
    }
}

void MediaControl::setPlaybackRate(double rate)
{
    if (m_mediaPlayer) {
        m_mediaPlayer->setPlaybackRate(rate);
    }
}

void MediaControl::toggleFullScreen()
{
    m_isFullScreen = !m_isFullScreen;
    emit fullScreenChanged(m_isFullScreen);
}

void MediaControl::captureScreenshot()
{
    if (!m_mediaPlayer || !isPlaying()) return;

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

void MediaControl::playNext()
{
    if (m_playlistManager->getPlaylistCount() > 0) {
        int nextRow = m_playlistManager->getCurrentRow() + 1;
        if (nextRow >= m_playlistManager->getPlaylistCount()) {
            nextRow = m_menuControl->getPlayMode() == MenuControl::Loop ? 0 : -1;
        }
        if (nextRow >= 0) {
            m_playlistManager->setCurrentRow(nextRow);
            QListWidgetItem* item = m_playlistManager->getCurrentItem();
            if (item) {
                playFile(item->data(Qt::UserRole).toString());
            }
        }
    }
}

void MediaControl::playPrevious()
{
    if (m_playlistManager->getPlaylistCount() > 0) {
        int prevRow = m_playlistManager->getCurrentRow() - 1;
        if (prevRow < 0) {
            prevRow = m_menuControl->getPlayMode() == MenuControl::Loop ? m_playlistManager->getPlaylistCount() - 1 : -1;
        }
        if (prevRow >= 0) {
            m_playlistManager->setCurrentRow(prevRow);
            QListWidgetItem* item = m_playlistManager->getCurrentItem();
            if (item) {
                playFile(item->data(Qt::UserRole).toString());
            }
        }
    }
}

void MediaControl::showThumbnailPreview(int position)
{
    // 记录当前状态
    PreviewState prevState = m_previewState;
    qDebug("基本预览请求 位置:%lld ms 当前状态:%d", position, static_cast<int>(prevState));
    
    if (!m_mediaPlayer || m_mediaPlayer->source().isEmpty()) {
        qDebug("预览请求失败 - 媒体源无效");
        hideThumbnailPreview(); // 隐藏
        return;
    }

    // 修改逻辑：允许在任何状态下更新预览内容，除了正在隐藏状态
    if (m_previewState != PreviewState::Hiding) {
        // 如果已经是显示中或可见状态，不需要重新设置为显示中
        if (m_previewState == PreviewState::Hidden) {
            m_previewState = PreviewState::Showing;  // 只有从隐藏状态才标记为显示中
            qDebug("基本预览 - 状态从隐藏变为显示中(%d)", static_cast<int>(m_previewState));
        } else {
            qDebug("基本预览 - 保持原状态(%d)更新内容", static_cast<int>(m_previewState));
        }
        
        // 确保预览播放器设置正确
        if (m_previewPlayer->source() != m_mediaPlayer->source()) {
            m_previewPlayer->setSource(m_mediaPlayer->source());
            m_previewPlayer->pause();  // 保持暂停状态
        }

        // 设置预览位置
        m_previewPlayer->setPosition(position);

        // 等待一小段时间让预览帧准备好
        QTimer::singleShot(50, this, [this, position]() {
            // 状态已变为隐藏中，跳过显示
            if (m_previewState == PreviewState::Hiding) {
                qDebug("基本预览回调 - 已进入隐藏状态，跳过显示");
                return;
            }
            
            // 获取解码帧
            if (QVideoSink *sink = m_previewPlayer->videoSink()) {
                QVideoFrame frame = sink->videoFrame();
                if (frame.isValid()) {
                    QImage image = frame.toImage();
                    if (!image.isNull()) {
                        // 缩放到预览大小
                        image = image.scaled(m_thumbnailPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

                        // 创建预览图像
                        QImage previewImage(m_thumbnailPreview->size(), QImage::Format_ARGB32);
                        previewImage.fill(Qt::black);

                        // 居中绘制
                        QPainter painter(&previewImage);
                        int x = (previewImage.width() - image.width()) / 2;
                        int y = (previewImage.height() - image.height()) / 2;
                        painter.drawImage(x, y, image);

                        // 显示时间标签
                        QString timeStr = formatTime(position);
                        m_previewTimeLabel->setText(timeStr);
                        m_previewTimeLabel->adjustSize();
                        m_previewTimeLabel->move(
                            (m_thumbnailPreview->width() - m_previewTimeLabel->width()) / 2,
                            m_thumbnailPreview->height() - m_previewTimeLabel->height() - 2
                        );

                        // 居中显示预览窗口
                        if (m_player) {
                            QPoint center = m_player->rect().center();
                            QPoint global = m_player->mapToGlobal(center);
                            m_thumbnailPreview->move(
                                global.x() - m_thumbnailPreview->width() / 2,
                                global.y() - m_thumbnailPreview->height() / 2
                            );
                        }
                        
                        m_thumbnailPreview->setPixmap(QPixmap::fromImage(previewImage));
                        m_thumbnailPreview->show();
                        
                        // 更新状态为可见
                        PreviewState oldState = m_previewState;
                        m_previewState = PreviewState::Visible;
                        qDebug("基本预览显示完成 - 状态从 %d 变为 %d(Visible)", static_cast<int>(oldState), static_cast<int>(m_previewState));
                    }
                }
            }
        });
    } else {
        qDebug("基本预览请求 - 正在隐藏中，忽略请求");
    }
}

void MediaControl::showThumbnailPreview(int position, const QPoint& mousePos, const QPoint& sliderPos)
{
    // 记录当前状态
    PreviewState prevState = m_previewState;
    qDebug("位置预览请求 位置:%lld ms 当前状态:%d 鼠标位置:%d,%d", position, static_cast<int>(prevState), mousePos.x(), mousePos.y());
    
    if (!m_mediaPlayer || m_mediaPlayer->source().isEmpty()) {
        qDebug("位置预览请求失败 - 媒体源无效");
        hideThumbnailPreview(); // 确保隐藏
        return;
    }

    // 修改逻辑：允许在任何状态下更新预览内容，除了正在隐藏状态
    if (m_previewState != PreviewState::Hiding) {
        // 如果已经是显示中或可见状态，不需要重新设置为显示中
        if (m_previewState == PreviewState::Hidden) {
            m_previewState = PreviewState::Showing;  // 只有从隐藏状态才标记为显示中
            qDebug("位置预览 - 状态从隐藏变为显示中(%d)", static_cast<int>(m_previewState));
        } else {
            qDebug("位置预览 - 保持原状态(%d)更新内容", static_cast<int>(m_previewState));
        }
        
        // 确保预览播放器设置正确
        if (m_previewPlayer->source() != m_mediaPlayer->source()) {
            m_previewPlayer->setSource(m_mediaPlayer->source());
            m_previewPlayer->pause();  // 保持暂停状态
        }

        // 设置预览位置
        m_previewPlayer->setPosition(position);

        // 等待一小段时间让预览帧准备好
        QTimer::singleShot(50, this, [this, position, mousePos, sliderPos]() {
            // 状态已变为隐藏中，跳过显示
            if (m_previewState == PreviewState::Hiding) {
                qDebug("位置预览回调 - 已进入隐藏状态，跳过显示");
                return;
            }
            
            // 获取预览帧
            if (QVideoSink *sink = m_previewPlayer->videoSink()) {
                QVideoFrame frame = sink->videoFrame();
                if (frame.isValid()) {
                    QImage image = frame.toImage();
                    if (!image.isNull()) {
                        // 缩放到预览大小
                        image = image.scaled(m_thumbnailPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

                        // 创建预览图像
                        QImage previewImage(m_thumbnailPreview->size(), QImage::Format_ARGB32);
                        previewImage.fill(Qt::black);

                        // 居中绘制
                        QPainter painter(&previewImage);
                        int x = (previewImage.width() - image.width()) / 2;
                        int y = (previewImage.height() - image.height()) / 2;
                        painter.drawImage(x, y, image);

                        // 显示时间标签
                        QString timeStr = formatTime(position);
                        m_previewTimeLabel->setText(timeStr);
                        m_previewTimeLabel->adjustSize();
                        m_previewTimeLabel->move(
                            (m_thumbnailPreview->width() - m_previewTimeLabel->width()) / 2,
                            m_thumbnailPreview->height() - m_previewTimeLabel->height() - 2
                        );

                        // 设置预览窗口位置
                        int previewX = mousePos.x() - m_thumbnailPreview->width() / 2;
                        int previewY = sliderPos.y() - m_thumbnailPreview->height() - 10; // 在滑块上方10像素
                        
                        // 确保预览窗口不超出屏幕边界
                        QScreen *screen = QGuiApplication::screenAt(mousePos);
                        if (screen) {
                            QRect screenGeometry = screen->geometry();
                            if (previewX < screenGeometry.left())
                                previewX = screenGeometry.left();
                            if (previewX + m_thumbnailPreview->width() > screenGeometry.right())
                                previewX = screenGeometry.right() - m_thumbnailPreview->width();
                            if (previewY < screenGeometry.top())
                                previewY = sliderPos.y() + 20; // 如果上方显示不下，则显示在滑块下方
                        }
                        
                        m_thumbnailPreview->move(previewX, previewY);
                        m_thumbnailPreview->setPixmap(QPixmap::fromImage(previewImage));
                        m_thumbnailPreview->show();
                        
                        // 更新状态为可见
                        PreviewState oldState = m_previewState;
                        m_previewState = PreviewState::Visible;
                        qDebug("位置预览显示完成 - 状态从 %d 变为 %d(Visible) 位置:[%d,%d]", 
                               static_cast<int>(oldState), static_cast<int>(m_previewState), previewX, previewY);
                    }
                }
            }
        });
    } else {
        qDebug("位置预览请求 - 正在隐藏中，忽略请求");
    }
}

void MediaControl::hideThumbnailPreview()
{
    // 记录当前状态
    PreviewState prevState = m_previewState;

    if (m_previewState == PreviewState::Hidden) {
        qDebug("预览隐藏请求 - 已经是隐藏状态，无需操作");
        return;
    }
    
    // 改状态正在隐藏
    m_previewState = PreviewState::Hiding;
    qDebug("预览隐藏 - 状态从 %d 变为 %d(Hiding)", static_cast<int>(prevState), static_cast<int>(m_previewState));
    
    // 立即隐藏预览窗口
    m_thumbnailPreview->hide();
    
    // 定时器延迟更新状态为 Hidden（延迟 200ms，确保异步显示已完成）
    QTimer::singleShot(200, this, [this]() {
        // debug当前状态
        if (m_previewState != PreviewState::Hiding) {
            qDebug("预览隐藏 - 状态已变更，跳过延迟更新");
            return;
        }
        
        // 更新为隐藏状态
        m_previewState = PreviewState::Hidden;
        qDebug("预览隐藏完成 - 状态更新为 %d(Hidden)，耗时 200ms", static_cast<int>(m_previewState));
    });
}

QMediaPlayer::PlaybackState MediaControl::playbackState() const
{
    return m_mediaPlayer ? m_mediaPlayer->playbackState() : QMediaPlayer::StoppedState;
}

bool MediaControl::isPlaying() const
{
    return m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
}

bool MediaControl::isPaused() const
{
    return m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState;
}

bool MediaControl::isStopped() const
{
    return m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::StoppedState;
}

bool MediaControl::isMuted() const
{
    return m_audioOutput && m_audioOutput->isMuted();
}

int MediaControl::volume() const
{
    return m_audioOutput ? static_cast<int>(m_audioOutput->volume() * 100) : 0;
}

qint64 MediaControl::position() const
{
    return m_mediaPlayer ? m_mediaPlayer->position() : 0;
}

qint64 MediaControl::duration() const
{
    return m_mediaPlayer ? m_mediaPlayer->duration() : 0;
}

QString MediaControl::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void MediaControl::setAspectRatio(double ratio)
{
    if (m_videoWidget) {
        if (ratio == 0.0) {
            // 自动模式
            m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
        } else {
            // 固定宽高比模式
            QSize videoSize = m_mediaPlayer->metaData().value(QMediaMetaData::Resolution).toSize();
            if (!videoSize.isEmpty()) {
                int w = m_videoWidget->width();
                int h = int(w / ratio);

                // 确保高度不超过视频窗口
                if (h > m_videoWidget->height()) {
                    h = m_videoWidget->height();
                    w = int(h * ratio);
                }

                // 计算居中位置
                int x = (m_videoWidget->width() - w) / 2;
                int y = (m_videoWidget->height() - h) / 2;

                // 设置视频显示区域
                m_videoWidget->setGeometry(x, y, w, h);
            }
        }
    }
}

void MediaControl::saveSettings()
{
    QSettings settings;

    // 保存音量
    settings.setValue("volume", volume());

    // 保存播放模式
    settings.setValue("playMode", static_cast<int>(m_menuControl->getPlayMode()));

    // 保存最后播放的文件和位置
    if (!m_mediaPlayer->source().isEmpty()) {
        settings.setValue("lastFile", m_mediaPlayer->source().toString());
        settings.setValue("lastPosition", m_mediaPlayer->position());
    }
}

void MediaControl::loadSettings()
{
    QSettings settings;

    // 恢复音量
    setVolume(settings.value("volume", 50).toInt());

    // 恢复播放模式
    if (m_menuControl) {
        m_menuControl->setPlayMode(static_cast<MenuControl::PlayMode>(settings.value("playMode", static_cast<int>(MenuControl::Sequential)).toInt()));
    }

    // 恢复最后播放的文件
    QString lastFile = settings.value("lastFile").toString();
    if (!lastFile.isEmpty() && QFile::exists(lastFile) && m_mediaPlayer) {
        // 使用值捕获而不是引用捕获，避免悬挂引用问题
        qint64 lastPosition = settings.value("lastPosition", 0).toLongLong();
        QTimer::singleShot(0, this, [this, lastPosition, lastFile]() {
            if (m_mediaPlayer) {  // 再次检查，以防对象已被销毁
                playFile(lastFile);
                m_mediaPlayer->setPosition(lastPosition);
                m_mediaPlayer->pause();
            }
        });
    }
}

void MediaControl::setLastPosition(const QString &filePath, qint64 position)
{
    m_lastPositions[filePath] = position;
}

qint64 MediaControl::getLastPosition(const QString &filePath) const
{
    return m_lastPositions.value(filePath, 0);
}

void MediaControl::playFile(const QString& filePath)
{
    if (!filePath.isEmpty()) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));

        // 恢复上次播放位置
        if (m_lastPositions.contains(filePath)) {
            m_mediaPlayer->setPosition(m_lastPositions[filePath]);
        }

        m_mediaPlayer->play();
        
        // 更新标题栏和窗口标题
        QString filename = QFileInfo(filePath).fileName();
        m_player->updateWindowTitle("媒体播放器 - " + filename);

        // 高亮当前播放项
        QListWidget* playlistWidget = m_playlistManager->getPlaylistWidget();
        for (int i = 0; i < playlistWidget->count(); ++i) {
            QListWidgetItem* item = playlistWidget->item(i);
            if (item->data(Qt::UserRole).toString() == filePath) {
                m_playlistManager->setCurrentRow(i);
                break;
            }
        }

        // 添加到历史记录
        m_menuControl->addToHistory(filePath, m_mediaPlayer->duration(), m_mediaPlayer->position());
    }
}

void MediaControl::updatePosition(qint64 position)
{
    // 保存当前播放位置
    if (!m_mediaPlayer->source().isEmpty()) {
        m_lastPositions[m_mediaPlayer->source().toString()] = position;
    }
    
    emit positionChanged(position);
}

void MediaControl::updateDuration(qint64 duration)
{
    emit durationChanged(duration);
}

void MediaControl::updatePlayIcon(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);
}

void MediaControl::handleResize()
{
    // 处理窗口大小变化时的视频显示调整
    if (m_videoWidget && m_player) {
        double ratio = m_player->videoAspectRatio;
        if (ratio != 0.0) {
            setAspectRatio(ratio);
        }
    }
}
