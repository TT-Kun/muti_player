#include "titlebar.h"
#include <QApplication>
#include <QStyle>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , m_isPressed(false)
    , m_isMaximized(false)
{
    setAttribute(Qt::WA_StyledBackground); // 新增：启用样式表绘制，增加这个就可以了
    setupUI();
    connectSignals();
    // 默认使用深色主题
    //updateStyle(true);
}

TitleBar::~TitleBar()
{

}

void TitleBar::setupUI()
{
    // 设置固定高度
    setFixedHeight(32);
    
    // 创建标题标签
    m_titleLabel = new QLabel("mutl_player", this);
   m_titleLabel->setAlignment(Qt::AlignCenter);
    
    // 创建控制按钮
    m_minButton = new QPushButton("—", this);
    m_maxButton = new QPushButton("□", this);
    m_closeButton = new QPushButton("×", this);
    
    // 设置按钮大小
    m_minButton->setFixedSize(32, 32);
    m_maxButton->setFixedSize(32, 32);
    m_closeButton->setFixedSize(32, 32);
    
    // 设置对象名，用于样式表
    setObjectName("titleBar");
    m_titleLabel->setObjectName("titleLabel");
    m_minButton->setObjectName("minButton");
    m_maxButton->setObjectName("maxButton");
    m_closeButton->setObjectName("closeButton");
    
    // 设置工具提示
    m_minButton->setToolTip(tr("最小化"));
    m_maxButton->setToolTip(tr("最大化"));
    m_closeButton->setToolTip(tr("关闭"));

    ///////////////////////////////////////////////////
    backBtn = new QPushButton( this);
    backBtn->setFixedSize(30, 30);

    backBtn->setIcon(QIcon(":/pitrue/home.png")); // 替换为实际资源路径
    backBtn->setIconSize(QSize(24, 24)); // 图标尺寸建议为容器大小的60%

    backBtn->setStyleSheet(R"(
        QPushButton {
            padding: 8px;
        }

        QPushButton:hover {

            border-color: #ced4da;
        }

        QPushButton:pressed {
            background-color: #dee2e6;
            border-color: #adb5bd;
            padding: 9px 7px 7px 9px; /* 模拟按下凹陷效果 */
        }

        QPushButton:focus {
            outline: none; /* 移除默认焦点虚线框 */
        }
    )");


    
    // 创建布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 5, 0);
    layout->setSpacing(0);
    
    // 添加组件到布局
    layout->addWidget(backBtn);
    layout->addSpacing(10);   // 固定间距
    layout->addWidget(m_titleLabel);
    layout->addStretch();
    layout->addWidget(m_minButton);
    layout->addWidget(m_maxButton);
    layout->addWidget(m_closeButton);
}

void TitleBar::connectSignals()
{
    // 连接按钮信号
    connect(m_minButton, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    connect(m_maxButton, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBar::closeClicked);
    connect(backBtn, &QPushButton::clicked, this, &TitleBar::backRequested);
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitleBar::updateStyle(bool isDarkTheme)
{
    QString baseStyle = QString(R"(
        #titleBar {
            background-color: %1;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }


        #titleLabel {
            color: %2;
            font-size: 14px;
            font-weight: bold;
        }
        
        QPushButton {
            color: %2;
           background: transparent;
            border: none;
            font-size: 16px;
        }
        #minButton:hover, #maxButton:hover {
            background-color: %3;
        }
        
        #closeButton:hover {
            background-color: #E81123;
            color: white;
        }                               
    )");
    
    if (isDarkTheme) {
        // 深色主题
        setStyleSheet(baseStyle.arg("#333333", "#FFFFFF", "#555555"));
    } else {
        // 浅色主题
        setStyleSheet(baseStyle.arg("#E1E1E1", "#000000", "#CCCCCC"));
    }
}



void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        m_startPos = event->globalPos();
    }
    
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed) {
        QPoint movePoint = event->globalPos() - m_startPos;
        
        // 获取主窗口
        QWidget *window = this->window();
        if (window->isMaximized()) {
            // 如果窗口最大化，则恢复正常大小并移动到适当位置
            window->showNormal();
            m_isMaximized = false;
            m_maxButton->setText("□");
            
            // 更新拖动起始位置比例，使鼠标位于点击的相对位置
            double widthRatio = (double)event->pos().x() / width();
            QPoint newPos = QPoint(window->width() * widthRatio, event->pos().y());
            m_startPos = event->globalPos() - newPos;
            movePoint = QPoint(0, 0);
        }
        
        // 移动窗口
        window->move(window->pos() + movePoint);
        m_startPos = event->globalPos();
    }
    
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
        
        // 更新最大化状态
        m_isMaximized = !m_isMaximized;
        m_maxButton->setText(m_isMaximized ? "❐" : "□");
    }

    QWidget::mouseDoubleClickEvent(event);
}
