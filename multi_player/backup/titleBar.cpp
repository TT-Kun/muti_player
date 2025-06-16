#include "titleBar.h"
#include "ui_titlebar.h"
#include <QApplication>
#include <QStyle>
#include <QDebug>
#include <QIcon>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TitleBar),
      m_currentTheme("dark")  // 默认使用深色主题
{
    ui->setupUi(this);
    setupConnections();
    
    // 初始主题
    updateTheme(m_currentTheme);
    
    setMouseTracking(true);
}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::setupConnections()
{
    connect(ui->minimizeButton, &QPushButton::clicked, this, &TitleBar::onMinimizeClicked);
    connect(ui->maximizeButton, &QPushButton::clicked, this, &TitleBar::onMaximizeClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &TitleBar::onCloseClicked);
}

void TitleBar::setTitle(const QString &title)
{
    // 标题过长，截断
    QString displayTitle = title;
    if (displayTitle.length() > 50) {
        displayTitle = displayTitle.left(47) + "...";
    }
    ui->titleLabel->setText(displayTitle);
}

void TitleBar::updateTheme(const QString &theme)
{
    m_currentTheme = theme;
    
    if (theme == "light") {
        // 浅色主题
        setStyleSheet(
            "TitleBar {"
            "   background-color: #ffffff;"
            "   border-top: none;"
            "   border-bottom: 1px solid #e0e0e0;"
            "   margin: 0px;"
            "   padding: 0px;"
            "}"
            "QLabel {"
            "   color: #333333;"
            "}"
            "QPushButton {"
            "   border: none;"
            "   background: transparent;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(0, 0, 0, 0.05);"
            "   border-radius: 4px;"
            "}"
        );
        
        // 关闭按钮样式
        ui->closeButton->setStyleSheet(
            "QPushButton:hover {"
            "   background-color: #ff5f5f;"
            "   border-radius: 4px;"
            "}"
        );
        
        // 设置深色图标
        ui->minimizeButton->setIcon(QIcon(":/icons/window-minimize-dark.svg"));
        ui->closeButton->setIcon(QIcon(":/icons/window-close-dark.svg"));
        
        // 根据窗口状态设置最大化/还原按钮图标
        if (window() && window()->isMaximized()) {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-restore-dark.svg"));
        } else {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-maximize-dark.svg"));
        }
    }
    else {
        // 深色主题
        setStyleSheet(
            "TitleBar {"
            "   background-color: #1e1e1e;"
            "   border-top: none;"
            "   border-bottom: 1px solid #121212;"
            "   margin: 0px;"
            "   padding: 0px;"
            "}"
            "QLabel {"
            "   color: #ffffff;"
            "}"
            "QPushButton {"
            "   border: none;"
            "   background: transparent;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(255, 255, 255, 0.1);"
            "   border-radius: 4px;"
            "}"
        );
        
        // 关闭按钮特殊样式
        ui->closeButton->setStyleSheet(
            "QPushButton:hover {"
            "   background-color: #ff5f5f;"
            "   border-radius: 4px;"
            "}"
        );
        
        // 设置白色图标
        ui->minimizeButton->setIcon(QIcon(":/icons/window-minimize-white.svg"));
        ui->closeButton->setIcon(QIcon(":/icons/window-close-white.svg"));
        
        // 根据窗口状态设置最大化/还原按钮图标
        if (window() && window()->isMaximized()) {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-restore-white.svg"));
        } else {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-maximize-white.svg"));
        }
    }
}

void TitleBar::updateMaximizeButton(bool isMaximized)
{
    // 根据当前主题和窗口状态选择合适的图标
    if (isMaximized) {
        if (m_currentTheme == "light") {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-restore-dark.svg"));
        } else {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-restore-white.svg"));
        }
        ui->maximizeButton->setToolTip("还原");
    } else {
        if (m_currentTheme == "light") {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-maximize-dark.svg"));
        } else {
            ui->maximizeButton->setIcon(QIcon(":/icons/window-maximize-white.svg"));
        }
        ui->maximizeButton->setToolTip("最大化");
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    // 移动窗口时需要考虑窗口是否已经最大化
    // 如果窗口最大化，先还原再移动
    // 计算鼠标位置占窗口宽度的比例
    if (event->buttons() & Qt::LeftButton) {
        if (window()->isMaximized()) {

            double widthRatio = event->position().x() / width();
            
            // 还原窗口
            window()->showNormal();
            updateMaximizeButton(false);
            
            // 计算新的窗口左上角位置，使鼠标保持在标题栏相同的相对位置
            QPoint newPos = event->globalPosition().toPoint() - QPoint(int(window()->width() * widthRatio), 15);
            window()->move(newPos);
            
            // 更新拖拽位置
            m_dragPosition = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        } else {
            // 正常窗口就直接移动了
            window()->move(event->globalPosition().toPoint() - m_dragPosition);
        }
        event->accept();
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 双击与最大化按钮行为一样的
    onMaximizeClicked();
    event->accept();
}

void TitleBar::onMinimizeClicked()
{
    window()->showMinimized();
}

void TitleBar::onMaximizeClicked()
{
    emit maximizeClicked(); // 最大化/还原
    
    if (window()->isMaximized()) {
        window()->showNormal();
    } else {
        window()->showMaximized();
    }
    
    updateMaximizeButton(window()->isMaximized());
}

void TitleBar::onCloseClicked()
{
    window()->close();
}

void TitleBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        // 获取窗口状态
        QWindowStateChangeEvent *stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        Qt::WindowStates state = window()->windowState();
        
        // 更新最大化/还原按钮的图标
        updateMaximizeButton(state & Qt::WindowMaximized);
    }
    
    QWidget::changeEvent(event);
} 
