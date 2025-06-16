#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class volumeControl;
}

class volumeControl : public QWidget
{
    Q_OBJECT

public:
    explicit volumeControl(QWidget *parent = nullptr);
    ~volumeControl();
    
    // 显示/隐藏音量控制
    void showVolumeControl(const QPoint &pos);
    void hideVolumeControl();
    
    // 音量相关方法
    int volume() const;
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    // 音量变化信号
    void volumeChanged(int volume);
    void muteToggled(bool muted);

protected:
    // 事件处理
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onVolumeIconClicked();

private:
    Ui::volumeControl *ui;
    bool m_isMuted;
    QTimer *m_hideTimer;
    
    // 初始化滑块
    void setupSlider();
};

#endif // VOLUMECONTROL_H
