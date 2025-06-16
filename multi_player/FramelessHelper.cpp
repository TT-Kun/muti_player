#include "FramelessHelper.h"
#include <QDebug>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <QApplication>

// 初始化静态成员变量
int CursorPosCalculator::m_nBorderWidth = 8;
int CursorPosCalculator::m_nTitleHeight = 30;

/***** 自定义橡皮筋 *****/
CustomRubberBand::CustomRubberBand(Shape shape, QWidget *parent)
    : QRubberBand(shape, parent)
{
    QPalette palette;
    palette.setBrush(QPalette::WindowText, QBrush(Qt::darkGray));
    setPalette(palette);
}

void CustomRubberBand::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStylePainter painter(this);
    QStyleOptionFocusRect option;
    option.initFrom(this);
    
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    pen.setColor(QColor(Qt::darkGray));
    painter.setPen(pen);
    painter.drawControl(QStyle::CE_FocusFrame, option);
}

/***** CursorPosCalculator *****/
CursorPosCalculator::CursorPosCalculator()
{
    reset();
}

void CursorPosCalculator::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge = false;
    m_bOnBottomRightEdge = false;
}

void CursorPosCalculator::recalculate(const QPoint &gMousePos, const QRect &frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();
    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX >= frameX &&
                    globalMouseX <= frameX + m_nBorderWidth);

    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth &&
                     globalMouseX <= frameX + frameWidth);

    m_bOnTopEdge = (globalMouseY >= frameY &&
                   globalMouseY <= frameY + m_nBorderWidth);

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth &&
                      globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

/***** WidgetData *****/
WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *pTopLevelWidget)
{
    d = _d;
    m_pWidget = pTopLevelWidget;
    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_pRubberBand = nullptr;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

WidgetData::~WidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);

    delete m_pRubberBand;
    m_pRubberBand = nullptr;
}

QWidget* WidgetData::widget()
{
    return m_pWidget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(event);
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    default:
        break;
    }
}

void WidgetData::updateRubberBandStatus()
{
    if (d->m_bRubberBandOnResize)
    {
        if (m_pRubberBand == nullptr) {
            m_pRubberBand = new CustomRubberBand(QRubberBand::Rectangle, m_pWidget);
        }
    }
    else
    {
        delete m_pRubberBand;
        m_pRubberBand = nullptr;
    }
}

void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    m_moveMousePos.recalculate(gMousePos, m_pWidget->frameGeometry());

    if(m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge)
    {
        m_pWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge)
    {
        m_pWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge)
    {
        m_pWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge)
    {
        m_pWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    }
    else
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void WidgetData::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize && m_pRubberBand)
        origRect = m_pRubberBand->frameGeometry();
    else
        origRect = m_pWidget->frameGeometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();

    if (m_pressedMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }

        if (d->m_bRubberBandOnResize && m_pRubberBand)
        {
            m_pRubberBand->setGeometry(newRect);
        }
        else
        {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;

        QRect frameRect = m_pWidget->frameGeometry();
        m_pressedMousePos.recalculate(event->globalPosition().toPoint(), frameRect);

        m_ptDragPos = event->globalPosition().toPoint() - frameRect.topLeft();

        if (m_pressedMousePos.m_bOnEdges)
        {
            if (d->m_bRubberBandOnResize && m_pRubberBand)
            {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
            }
        }
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        m_pressedMousePos.reset();
        
        if (m_pRubberBand && m_pRubberBand->isVisible())
        {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
        }
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bLeftButtonPressed)
    {
        // 当鼠标形状已经改变时(即在边缘区域)，执行缩放操作
        if (d->m_bWidgetResizable && (m_pressedMousePos.m_bOnEdges || m_bCursorShapeChanged))
        {
            resizeWidget(event->globalPosition().toPoint());
        }
    }
    else if (d->m_bWidgetResizable)
    {
        updateCursorShape(event->globalPosition().toPoint());
    }
}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed)
    {
        m_pWidget->unsetCursor();
        m_bCursorShapeChanged = false;
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if (d->m_bWidgetResizable)
    {
        updateCursorShape(m_pWidget->mapToGlobal(event->position().toPoint()));
    }
}

/***** FramelessHelper *****/
FramelessHelper::FramelessHelper(QObject *parent)
    : QObject(parent)
    , d(new FramelessHelperPrivate())
{
}

FramelessHelper::~FramelessHelper()
{
    QList<QWidget*> keys = d->m_widgetDataHash.keys();
    for (auto widget : keys) {
        delete d->m_widgetDataHash.take(widget);
    }
    delete d;
}

bool FramelessHelper::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (widget && d->m_widgetDataHash.contains(widget))
    {
        WidgetData *data = d->m_widgetDataHash.value(widget);
        if (data)
        {
            data->handleWidgetEvent(event);
        }
    }
    return QObject::eventFilter(obj, event);
}

void FramelessHelper::activateOn(QWidget *topLevelWidget)
{
    if (!d->m_widgetDataHash.contains(topLevelWidget))
    {
        WidgetData *data = new WidgetData(d, topLevelWidget);
        d->m_widgetDataHash.insert(topLevelWidget, data);
        topLevelWidget->installEventFilter(this);

        qDebug() << "FramelessHelper activated on" << topLevelWidget->objectName();
    }
}

void FramelessHelper::removeFrom(QWidget *topLevelWidget)
{
    WidgetData *data = d->m_widgetDataHash.take(topLevelWidget);
    if (data)
    {
        topLevelWidget->removeEventFilter(this);
        delete data;
        
        qDebug() << "FramelessHelper removed from" << topLevelWidget->objectName();
    }
}

void FramelessHelper::setWidgetResizable(bool resizable)
{
    d->m_bWidgetResizable = resizable;
}

void FramelessHelper::setRubberBandOnResize(bool resizable)
{
    d->m_bRubberBandOnResize = resizable;
    QList<WidgetData*> list = d->m_widgetDataHash.values();
    foreach (WidgetData *data, list)
    {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setBorderWidth(uint width)
{
    if (width > 0)
    {
        CursorPosCalculator::m_nBorderWidth = width;
    }
}

void FramelessHelper::setTitleHeight(uint height)
{
    if (height > 0)
    {
        CursorPosCalculator::m_nTitleHeight = height;
    }
}

bool FramelessHelper::widgetResizable() const
{
    return d->m_bWidgetResizable;
}

bool FramelessHelper::rubberBandOnResize() const
{
    return d->m_bRubberBandOnResize;
}

uint FramelessHelper::borderWidth() const
{
    return CursorPosCalculator::m_nBorderWidth;
}

uint FramelessHelper::titleHeight() const
{
    return CursorPosCalculator::m_nTitleHeight;
} 