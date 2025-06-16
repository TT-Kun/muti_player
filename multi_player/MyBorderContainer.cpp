#include "MyBorderContainer.h"
#include <QMainWindow>

// MyBorder 内部类实现

MyBorderContainer::MyBorder::MyBorder(QWidget *parent, BorderType type, MyBorderContainer *outClass)
    : QLabel(parent)
{
    KeepDrag = false;
    MyType = type;
    container = outClass;
    
    // 根据边框类型设置不同的光标形状
    if (MyType == L_BORDER || MyType == R_BORDER)
    {
        setCursor(Qt::SizeHorCursor);  // 水平调整光标
    }
    else if (MyType == T_BORDER || MyType == B_BORDER)
    {
        setCursor(Qt::SizeVerCursor);  // 垂直调整光标
    }
    else if (MyType == LT_BORDER || MyType == RB_BORDER)
    {
        setCursor(Qt::SizeFDiagCursor); // 左上-右下调整光标
    }
    else if (MyType == LB_BORDER || MyType == RT_BORDER)
    {
        setCursor(Qt::SizeBDiagCursor); // 右上-左下调整光标
    }
    else
    {
        setCursor(Qt::ArrowCursor); // 默认箭头光标
    }
    
    setStyleSheet("QLabel { background-color: rgba(255, 255, 255, 0); }");
}

// 鼠标按下事件
void MyBorderContainer::MyBorder::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        KeepDrag = true;
        mousePointOld = ev->globalPosition().toPoint(); // Qt6使用globalPosition().toPoint()
        
        // 输出调试信息
        QString borderTypeName;
        switch (MyType) {
        case L_BORDER:
            borderTypeName = "左边框";
            break;
        case R_BORDER:
            borderTypeName = "右边框";
            break;
        case T_BORDER:
            borderTypeName = "顶部边框";
            break;
        case B_BORDER:
            borderTypeName = "底部边框";
            break;
        case LT_BORDER:
            borderTypeName = "左上角";
            break;
        case RT_BORDER:
            borderTypeName = "右上角";
            break;
        case LB_BORDER:
            borderTypeName = "左下角";
            break;
        case RB_BORDER:
            borderTypeName = "右下角";
            break;
        default:
            borderTypeName = "未知边框";
            break;
        }
        qDebug() << "点击了" << borderTypeName << "，开始调整窗口大小";
    }
    QLabel::mousePressEvent(ev); // 调用父类方法
}

// 鼠标移动事件
void MyBorderContainer::MyBorder::mouseMoveEvent(QMouseEvent *ev)
{
    if (KeepDrag && container->isResizable())
    {
        const QPoint position = ev->globalPosition().toPoint() - mousePointOld;

        // 根据边框类型调用对应的处理方法
        switch (MyType) {
        case L_BORDER: // 左边框
            container->getLeftScaleEvent(position);
            break;
        case R_BORDER: // 右边框
            container->getRightScaleEvent(position);
            break;
        case T_BORDER: // 上边框
            container->getTopScaleEvent(position);
            break;
        case B_BORDER: // 下边框
            container->getBottomScaleEvent(position);
            break;
        case RB_BORDER: // 右下角
            container->getRBScaleEvent(position);
            break;
        case RT_BORDER: // 右上角
            container->getRTScaleEvent(position);
            break;
        case LB_BORDER: // 左下角
            container->getLBScaleEvent(position);
            break;
        case LT_BORDER: // 左上角
            container->getLTScaleEvent(position);
            break;
        default:
            break;
        }
        mousePointOld = ev->globalPosition().toPoint();
    }
    QLabel::mouseMoveEvent(ev); // 调用父类方法
}

// 鼠标释放事件
void MyBorderContainer::MyBorder::mouseReleaseEvent(QMouseEvent *ev)
{
    if (KeepDrag) {
        KeepDrag = false;
        qDebug() << "释放鼠标，结束调整窗口大小";
    }
    QLabel::mouseReleaseEvent(ev); // 调用父类方法
}

// MyBorderContainer 类实现

MyBorderContainer::MyBorderContainer(QWidget *parent, uint16_t minWindowHeight, uint16_t minWindowWidth, uint16_t borderSize)
    : QObject(nullptr) // QObject 不设置父对象
{
    m_minWindowHeight = minWindowHeight; // 最小窗口高度
    m_minWindowWidth = minWindowWidth;   // 最小窗口宽度
    m_borderSize = borderSize;           // 边框捕获区域大小
    m_resizable = true;                  // 默认启用缩放
    m_centralWidget = nullptr;           // 中央窗口初始为空

    // 创建边框容器
    m_borderContainer = new QWidget(parent);
    m_borderContainer->setObjectName("BorderContainer");
    
    // 设置背景颜色为透明
    m_borderContainer->setStyleSheet("QWidget#BorderContainer { background-color: transparent; }");
    
    // 设置尺寸策略为可扩展
    m_borderContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 设置最小大小
    m_borderContainer->setMinimumSize(m_minWindowWidth, m_minWindowHeight);
    
    // 创建网格布局
    m_gridLayout = new QGridLayout(m_borderContainer);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    
    // 设置行列拉伸因子：让中间区域拉伸，边框区域固定大小
    m_gridLayout->setRowStretch(0, 0);
    m_gridLayout->setRowStretch(1, 1);
    m_gridLayout->setRowStretch(2, 0);
    m_gridLayout->setColumnStretch(0, 0);
    m_gridLayout->setColumnStretch(1, 1);
    m_gridLayout->setColumnStretch(2, 0);
    
    qDebug() << "初始化MyBorderContainer，边框大小:" << m_borderSize 
             << "最小窗口尺寸:" << m_minWindowWidth << "x" << m_minWindowHeight;
    
    // 初始化边框
    initBorder();
    
    // 如果提供了父窗口，将其设为中央窗口
    if (parent) {
        setCentralWidget(parent);
    }
}

void MyBorderContainer::initBorder()
{
    qDebug() << "初始化边框部件";
    
    // 创建8个边框代理
    labelLT = new MyBorder(m_borderContainer, LT_BORDER, this);
    labelTop = new MyBorder(m_borderContainer, T_BORDER, this);
    labelRT = new MyBorder(m_borderContainer, RT_BORDER, this);
    labelLft = new MyBorder(m_borderContainer, L_BORDER, this);
    labelRit = new MyBorder(m_borderContainer, R_BORDER, this);
    labelLB = new MyBorder(m_borderContainer, LB_BORDER, this);
    labelBot = new MyBorder(m_borderContainer, B_BORDER, this);
    labelRB = new MyBorder(m_borderContainer, RB_BORDER, this);
    
    // 将边框添加到网格布局中
    m_gridLayout->addWidget(labelLT, 0, 0); // 左上角
    m_gridLayout->addWidget(labelTop, 0, 1); // 上边框
    m_gridLayout->addWidget(labelRT, 0, 2); // 右上角
    m_gridLayout->addWidget(labelLft, 1, 0); // 左边框
    // 中央窗口将在setCentralWidget中添加到(1, 1)位置
    m_gridLayout->addWidget(labelRit, 1, 2); // 右边框
    m_gridLayout->addWidget(labelLB, 2, 0); // 左下角
    m_gridLayout->addWidget(labelBot, 2, 1); // 下边框
    m_gridLayout->addWidget(labelRB, 2, 2); // 右下角
    
    // 设置边框大小
    updateBorderGeometry();
}

void MyBorderContainer::updateBorderGeometry()
{
    // 设置边框大小为固定值
    labelLT->setFixedSize(m_borderSize, m_borderSize);
    labelTop->setFixedHeight(m_borderSize);
    labelRT->setFixedSize(m_borderSize, m_borderSize);
    labelLft->setFixedWidth(m_borderSize);
    labelRit->setFixedWidth(m_borderSize);
    labelLB->setFixedSize(m_borderSize, m_borderSize);
    labelBot->setFixedHeight(m_borderSize);
    labelRB->setFixedSize(m_borderSize, m_borderSize);
    
    qDebug() << "更新边框尺寸为" << m_borderSize << "像素";
}

void MyBorderContainer::setMinWindowSize(uint16_t minWindowWidth, uint16_t minWindowHeight)
{
    m_minWindowHeight = minWindowHeight;
    m_minWindowWidth = minWindowWidth;
    
    // 更新容器的最小尺寸
    m_borderContainer->setMinimumSize(m_minWindowWidth, m_minWindowHeight);
    qDebug() << "设置最小窗口尺寸为" << m_minWindowWidth << "x" << m_minWindowHeight;
}

void MyBorderContainer::setBorderSize(uint16_t borderSize)
{
    m_borderSize = borderSize;
    updateBorderGeometry();
    qDebug() << "设置边框大小为" << m_borderSize << "像素";
}

void MyBorderContainer::setResizable(bool resizable)
{
    m_resizable = resizable;
    qDebug() << "设置窗口缩放状态为:" << (resizable ? "可调整" : "不可调整");
}

bool MyBorderContainer::isResizable() const
{
    return m_resizable;
}

uint16_t MyBorderContainer::borderSize() const
{
    return m_borderSize;
}

QWidget* MyBorderContainer::getContainer() const
{
    return m_borderContainer;
}

void MyBorderContainer::setCentralWidget(QWidget* centralWidget)
{
    // 如果已经有中央窗口，先移除
    if (m_centralWidget) {
        m_gridLayout->removeWidget(m_centralWidget);
        // 不删除原来的窗口，只是移除关系
        qDebug() << "移除旧的中央窗口";
    }
    
    m_centralWidget = centralWidget;
    
    // 如果提供了新的中央窗口，添加到网格布局中
    if (m_centralWidget) {
        // 中央窗口放在(1, 1)位置
        m_gridLayout->addWidget(m_centralWidget, 1, 1);
        qDebug() << "设置新的中央窗口";
    }
}

// 调整主窗口大小的辅助函数
void MyBorderContainer::adjustParentWindowSize(const QRect& newGeometry)
{
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        // 如果父窗口是QMainWindow，直接调整其大小和位置
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            qDebug() << "调整主窗口大小为:" << newGeometry.size() << "位置:" << newGeometry.topLeft();
            mainWindow->setGeometry(newGeometry);
        }
    }
}

// 左边框拖拽处理
void MyBorderContainer::getLeftScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    QPoint globalPos = m_borderContainer->mapToGlobal(QPoint(0, 0));
    
    // 计算新的左侧边缘位置
    int newLeft = geometry.left() + movPoint.x();
    
    // 计算新的宽度
    int newWidth = geometry.right() - newLeft + 1;
    
    // 检查是否满足最小宽度限制
    if (newWidth < m_minWindowWidth) {
        // 如果太小，固定在最小宽度
        newLeft = geometry.right() - m_minWindowWidth + 1;
        newWidth = m_minWindowWidth;
    }
    
    // 更新边框容器几何信息 - 只调整宽度，保持位置不变
    QRect newGeometry(newLeft, geometry.top(), newWidth, geometry.height());
    
    // 调整主窗口大小和位置
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 计算主窗口的新位置和大小
            QPoint currentPos = mainWindow->pos();
            QSize currentSize = mainWindow->size();
            
            // 新位置：x坐标偏移，y保持不变
            QPoint newPos(currentPos.x() + movPoint.x(), currentPos.y());
            // 新大小：宽度变化，高度保持不变
            QSize newSize(currentSize.width() - movPoint.x(), currentSize.height());
            
            // 检查最小尺寸
            if (newSize.width() < m_minWindowWidth) {
                int diff = m_minWindowWidth - newSize.width();
                newSize.setWidth(m_minWindowWidth);
                newPos.setX(newPos.x() - diff);
            }
            
            // 设置主窗口的新几何信息
            mainWindow->setGeometry(QRect(newPos, newSize));
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "左边框调整: 新位置 X=" << newPos.x() << " 新宽度=" << newSize.width();
        }
    }
}

// 右边框拖拽处理
void MyBorderContainer::getRightScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    
    // 计算新的宽度
    int newWidth = geometry.width() + movPoint.x();
    
    // 检查是否满足最小宽度限制
    if (newWidth < m_minWindowWidth) {
        newWidth = m_minWindowWidth;
    }
    
    // 调整主窗口大小
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 只改变宽度，保持位置不变
            QSize newSize(newWidth, mainWindow->height());
            mainWindow->resize(newSize);
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "右边框调整: 新宽度=" << newWidth;
        }
    }
}

// 底部边框拖拽处理
void MyBorderContainer::getBottomScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    
    // 计算新的高度
    int newHeight = geometry.height() + movPoint.y();
    
    // 检查是否满足最小高度限制
    if (newHeight < m_minWindowHeight) {
        newHeight = m_minWindowHeight;
    }
    
    // 调整主窗口大小
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 只改变高度，保持位置不变
            QSize newSize(mainWindow->width(), newHeight);
            mainWindow->resize(newSize);
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "底部边框调整: 新高度=" << newHeight;
        }
    }
}

// 顶部边框拖拽处理
void MyBorderContainer::getTopScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    
    // 计算新的顶部边缘位置和高度
    int newHeight = geometry.height() - movPoint.y();
    
    // 检查是否满足最小高度限制
    if (newHeight < m_minWindowHeight) {
        newHeight = m_minWindowHeight;
    } else {
        // 调整主窗口大小和位置
        QWidget* parentWidget = m_borderContainer->parentWidget();
        if (parentWidget) {
            QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
            if (mainWindow) {
                // 计算主窗口的新位置和大小
                QPoint currentPos = mainWindow->pos();
                QSize currentSize = mainWindow->size();
                
                // 新位置：y坐标偏移，x保持不变
                QPoint newPos(currentPos.x(), currentPos.y() + movPoint.y());
                // 新大小：高度变化，宽度保持不变
                QSize newSize(currentSize.width(), currentSize.height() - movPoint.y());
                
                // 检查最小尺寸
                if (newSize.height() < m_minWindowHeight) {
                    int diff = m_minWindowHeight - newSize.height();
                    newSize.setHeight(m_minWindowHeight);
                    newPos.setY(newPos.y() - diff);
                }
                
                // 设置主窗口的新几何信息
                mainWindow->setGeometry(QRect(newPos, newSize));
                
                // 调整边框容器以匹配主窗口
                m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
                
                qDebug() << "顶部边框调整: 新位置 Y=" << newPos.y() << " 新高度=" << newSize.height();
            }
        }
    }
}

// 右下角拖拽处理
void MyBorderContainer::getRBScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    
    // 计算新的宽度和高度
    int newWidth = geometry.width() + movPoint.x();
    int newHeight = geometry.height() + movPoint.y();
    
    // 检查是否满足最小宽高限制
    if (newWidth < m_minWindowWidth) {
        newWidth = m_minWindowWidth;
    }
    
    if (newHeight < m_minWindowHeight) {
        newHeight = m_minWindowHeight;
    }
    
    // 调整主窗口大小
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 改变宽度和高度，保持位置不变
            QSize newSize(newWidth, newHeight);
            mainWindow->resize(newSize);
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "右下角调整: 新宽度=" << newWidth << " 新高度=" << newHeight;
        }
    }
}

// 右上角拖拽处理
void MyBorderContainer::getRTScaleEvent(QPoint movPoint)
{
    // 获取当前边框容器的几何信息
    QRect geometry = m_borderContainer->geometry();
    
    // 计算新的宽度
    int newWidth = geometry.width() + movPoint.x();
    
    // 检查是否满足最小宽度限制
    if (newWidth < m_minWindowWidth) {
        newWidth = m_minWindowWidth;
    }
    
    // 调整主窗口大小和位置
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 计算主窗口的新位置和大小
            QPoint currentPos = mainWindow->pos();
            QSize currentSize = mainWindow->size();
            
            // 新位置：y坐标偏移，x保持不变
            QPoint newPos(currentPos.x(), currentPos.y() + movPoint.y());
            // 新大小：宽度增加，高度减少
            QSize newSize(newWidth, currentSize.height() - movPoint.y());
            
            // 检查最小尺寸
            if (newSize.height() < m_minWindowHeight) {
                int diff = m_minWindowHeight - newSize.height();
                newSize.setHeight(m_minWindowHeight);
                newPos.setY(newPos.y() - diff);
            }
            
            // 设置主窗口的新几何信息
            mainWindow->setGeometry(QRect(newPos, newSize));
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "右上角调整: 新位置 Y=" << newPos.y() 
                     << " 新宽度=" << newSize.width() << " 新高度=" << newSize.height();
        }
    }
}

// 左上角拖拽处理
void MyBorderContainer::getLTScaleEvent(QPoint movPoint)
{
    // 调整主窗口大小和位置
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 计算主窗口的新位置和大小
            QPoint currentPos = mainWindow->pos();
            QSize currentSize = mainWindow->size();
            
            // 新位置：x和y坐标都偏移
            QPoint newPos(currentPos.x() + movPoint.x(), currentPos.y() + movPoint.y());
            // 新大小：宽度和高度都减少
            QSize newSize(currentSize.width() - movPoint.x(), currentSize.height() - movPoint.y());
            
            // 检查最小尺寸
            if (newSize.width() < m_minWindowWidth) {
                int diff = m_minWindowWidth - newSize.width();
                newSize.setWidth(m_minWindowWidth);
                newPos.setX(newPos.x() - diff);
            }
            
            if (newSize.height() < m_minWindowHeight) {
                int diff = m_minWindowHeight - newSize.height();
                newSize.setHeight(m_minWindowHeight);
                newPos.setY(newPos.y() - diff);
            }
            
            // 设置主窗口的新几何信息
            mainWindow->setGeometry(QRect(newPos, newSize));
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "左上角调整: 新位置 X=" << newPos.x() << " Y=" << newPos.y() 
                     << " 新宽度=" << newSize.width() << " 新高度=" << newSize.height();
        }
    }
}

// 左下角拖拽处理
void MyBorderContainer::getLBScaleEvent(QPoint movPoint)
{
    // 调整主窗口大小和位置
    QWidget* parentWidget = m_borderContainer->parentWidget();
    if (parentWidget) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parentWidget);
        if (mainWindow) {
            // 计算主窗口的新位置和大小
            QPoint currentPos = mainWindow->pos();
            QSize currentSize = mainWindow->size();
            
            // 新位置：x坐标偏移，y保持不变
            QPoint newPos(currentPos.x() + movPoint.x(), currentPos.y());
            // 新大小：宽度减少，高度增加
            QSize newSize(currentSize.width() - movPoint.x(), currentSize.height() + movPoint.y());
            
            // 检查最小尺寸
            if (newSize.width() < m_minWindowWidth) {
                int diff = m_minWindowWidth - newSize.width();
                newSize.setWidth(m_minWindowWidth);
                newPos.setX(newPos.x() - diff);
            }
            
            if (newSize.height() < m_minWindowHeight) {
                newSize.setHeight(m_minWindowHeight);
            }
            
            // 设置主窗口的新几何信息
            mainWindow->setGeometry(QRect(newPos, newSize));
            
            // 调整边框容器以匹配主窗口
            m_borderContainer->setGeometry(0, 0, newSize.width(), newSize.height());
            
            qDebug() << "左下角调整: 新位置 X=" << newPos.x() 
                     << " 新宽度=" << newSize.width() << " 新高度=" << newSize.height();
        }
    }
} 