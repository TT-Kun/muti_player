#ifndef FRAMELESSHELPER_H
#define FRAMELESSHELPER_H

#include <QObject>
#include <QWidget>
#include <QRubberBand>
#include <QMainWindow>
#include <QPoint>
#include <QRect>

// 前向声明
class WidgetData;
class FramelessHelperPrivate;


//无边框窗口辅助类，用于实现窗口边缘拖拽缩放功能
class FramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit FramelessHelper(QObject *parent = nullptr);
    ~FramelessHelper();

    // 在指定窗口上激活边框拖拽功能
    void activateOn(QWidget *topLevelWidget);

    // 从指定窗口移除边框拖拽功能
    void removeFrom(QWidget *topLevelWidget);

    // 设置是否可以调整窗口大小
    void setWidgetResizable(bool resizable);

    // 设置调整大小时是否显示橡皮筋效果
    void setRubberBandOnResize(bool resizable);

    // 设置边框宽度
    void setBorderWidth(uint width);

    // 设置标题栏高度
    void setTitleHeight(uint height);

    // 获取窗口是否可调整大小
    bool widgetResizable() const;

    // 获取调整大小时是否有橡皮筋效果
    bool rubberBandOnResize() const;

    // 获取边框宽度
    uint borderWidth() const;

    // 获取标题栏高度
    uint titleHeight() const;

protected:
    // 事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    FramelessHelperPrivate *d;
    
    // 声明 WidgetData 为友元类，允许其访问私有成员
    friend class WidgetData;
};


//FramelessHelper 的私有实现类
class FramelessHelperPrivate {
public:
    QHash<QWidget*, WidgetData*> m_widgetDataHash;
    bool m_bWidgetResizable = true;
    bool m_bRubberBandOnResize = false;
};

//自定义橡皮筋类，用于显示调整大小时的视觉效果
class CustomRubberBand : public QRubberBand
{
    Q_OBJECT
public:
    explicit CustomRubberBand(Shape shape, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};


//计算鼠标位置的辅助类
class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();

    // 重置状态
    void reset();

    // 重新计算鼠标位置
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

    // 边缘状态标志
    bool m_bOnEdges = false;
    bool m_bOnLeftEdge = false;
    bool m_bOnRightEdge = false;
    bool m_bOnTopEdge = false;
    bool m_bOnBottomEdge = false;
    bool m_bOnTopLeftEdge = false;
    bool m_bOnBottomLeftEdge = false;
    bool m_bOnTopRightEdge = false;
    bool m_bOnBottomRightEdge = false;

    // 边框宽度和标题栏高度
    static int m_nBorderWidth;
    static int m_nTitleHeight;
};

//窗口数据类，每个激活了FramelessHelper的窗口都会创建一个
class WidgetData
{
public:
    explicit WidgetData(FramelessHelperPrivate *d, QWidget *topLevelWidget);
    ~WidgetData();

    // 获取关联的窗口
    QWidget* widget();

    // 处理窗口事件
    void handleWidgetEvent(QEvent *event);

    // 更新橡皮筋状态
    void updateRubberBandStatus();

private:
    // 更新鼠标指针形状
    void updateCursorShape(const QPoint &globalMousePos);

    // 调整窗口大小
    void resizeWidget(const QPoint &globalMousePos);

    // 处理鼠标按下事件
    void handleMousePressEvent(QMouseEvent *event);

    // 处理鼠标释放事件
    void handleMouseReleaseEvent(QMouseEvent *event);

    // 处理鼠标移动事件
    void handleMouseMoveEvent(QMouseEvent *event);

    // 处理鼠标离开事件
    void handleLeaveEvent(QEvent *event);

    // 处理悬停移动事件
    void handleHoverMoveEvent(QHoverEvent *event);

    // 成员变量
    FramelessHelperPrivate *d;
    CustomRubberBand *m_pRubberBand;
    QWidget *m_pWidget;
    QPoint m_ptDragPos;
    CursorPosCalculator m_pressedMousePos;
    CursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    Qt::WindowFlags m_windowFlags;
};

#endif // FRAMELESSHELPER_H
 