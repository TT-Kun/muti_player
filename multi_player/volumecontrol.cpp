#include "volumecontrol.h"
#include "ui_volumecontrol.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QMouseEvent>

volumeControl::volumeControl(QWidget *parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
    , ui(new Ui::volumeControl)
    , m_isMuted(false)
{
    ui->setupUi(this);
    
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    
    // 初始化隐藏定时器
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(2000);  // 2秒自动隐藏
    connect(m_hideTimer, &QTimer::timeout, this, &volumeControl::hideVolumeControl);
    
    // 安装事件过滤器
    ui->volumeSlider->installEventFilter(this);
    
    // 设置音量图标
    ui->volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
    
    // 设置初始音量值
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);
    ui->volumeValueLabel->setText(QString::number(50));
    
    // 连接信号槽
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->volumeValueLabel->setText(QString::number(value));
        emit volumeChanged(value);
        
        // 处理图标变化
        if (value == 0) {
            m_isMuted = true;
            ui->volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        } else if (!m_isMuted) {
            ui->volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
        }
    });
    
    // 设置音量图标点击处理
    connect(ui->volumeIcon, &QPushButton::clicked, this, &volumeControl::onVolumeIconClicked);
    
    setupSlider();
    
    hide();
}

volumeControl::~volumeControl()
{
    delete ui;
}

void volumeControl::setupSlider()
{
    ui->volumeSlider->installEventFilter(this);
}

void volumeControl::showVolumeControl(const QPoint &pos)
{
    // 计算弹出位置
    QPoint displayPos = pos;
    displayPos.setY(displayPos.y() - height() - 5);  // 在按钮上方5像素
    
    QScreen *screen = QGuiApplication::screenAt(pos);
    if (screen) {
        QRect screenGeometry = screen->geometry();
        if (displayPos.x() < screenGeometry.left())
            displayPos.setX(screenGeometry.left() + 5);
        if (displayPos.x() + width() > screenGeometry.right())
            displayPos.setX(screenGeometry.right() - width() - 5);
        if (displayPos.y() < screenGeometry.top())
            displayPos.setY(pos.y() + 5);  // 如果上方不够空间，就显示在下方
    }
    
    // 显示
    move(displayPos);
    show();
    setFocus();
    
    m_hideTimer->start();
}

void volumeControl::hideVolumeControl()
{
    hide();
}

int volumeControl::volume() const
{
    return ui->volumeSlider->value();
}

void volumeControl::setVolume(int volume)
{
    ui->volumeSlider->setValue(volume);
}

void volumeControl::setMuted(bool muted)
{
    m_isMuted = muted;
    if (muted) {
        ui->volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        ui->volumeIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

void volumeControl::onVolumeIconClicked()
{
    m_isMuted = !m_isMuted;
    setMuted(m_isMuted);
    emit muteToggled(m_isMuted);
}

void volumeControl::leaveEvent(QEvent *event)
{
    // 鼠标离开时启动隐藏计时器
    m_hideTimer->start();
    QWidget::leaveEvent(event);
}

bool volumeControl::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->volumeSlider) {
        // 重置隐藏计时器
        if (event->type() == QEvent::Enter || 
            event->type() == QEvent::MouseMove || 
            event->type() == QEvent::MouseButtonPress) {
            m_hideTimer->start();
        }
        
        // 处理鼠标点击事件
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                double pos = mouseEvent->position().x() / static_cast<double>(ui->volumeSlider->width());
                int value = pos * (ui->volumeSlider->maximum() - ui->volumeSlider->minimum()) + ui->volumeSlider->minimum();
                ui->volumeSlider->setValue(value);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}
