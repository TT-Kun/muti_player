#ifndef MYBORDERCONTAINER_H
#define MYBORDERCONTAINER_H

#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QMouseEvent>
#include <QGridLayout>
#include <QDebug>

// 边框类型枚举
enum BorderType
{
    NULL_BORDER = 0,    // 无
    L_BORDER,           // 左
    R_BORDER,           // 右
    T_BORDER,           // 上
    B_BORDER,           // 下
    LT_BORDER,          // 左上
    LB_BORDER,          // 左下
    RT_BORDER,          // 右上
    RB_BORDER,          // 右下
};

/**
 * @brief 边框容器类，用于实现无边框窗口的缩放功能
 * 实现原理：使用GridLayout九宫格布局，将窗口内容放在中间，周围放置边框代理
 */
class MyBorderContainer : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口，即需要添加缩放功能的窗口
     * @param minWindowHeight 最小窗口高度
     * @param minWindowWidth 最小窗口宽度
     * @param borderSize 边框捕获区域大小
     */
    MyBorderContainer(QWidget *parent = nullptr, uint16_t minWindowHeight = 50, uint16_t minWindowWidth = 50, uint16_t borderSize = 5);
    
    /**
     * @brief 设置窗口最小尺寸
     * @param minWindowWidth 最小窗口宽度
     * @param minWindowHeight 最小窗口高度
     */
    void setMinWindowSize(uint16_t minWindowWidth, uint16_t minWindowHeight);
    
    /**
     * @brief 设置边框捕获区域大小
     * @param borderSize 边框捕获区域大小(像素)
     */
    void setBorderSize(uint16_t borderSize);
    
    /**
     * @brief 设置是否启用缩放功能
     * @param resizable 是否启用缩放
     */
    void setResizable(bool resizable);
    
    /**
     * @brief 获取是否启用缩放功能
     * @return 是否启用缩放
     */
    bool isResizable() const;
    
    /**
     * @brief 获取边框捕获区域大小
     * @return 边框捕获区域大小
     */
    uint16_t borderSize() const;
    
    /**
     * @brief 获取边框容器
     * @return 边框容器
     */
    QWidget* getContainer() const;
    
    /**
     * @brief 设置中央窗口
     * @param centralWidget 中央窗口
     */
    void setCentralWidget(QWidget* centralWidget);

protected:
    /**
     * @brief 初始化边框
     * 创建GridLayout布局和9个区域
     */
    void initBorder();
    
    /**
     * @brief 更新边框大小
     */
    void updateBorderGeometry();
    
    /**
     * @brief 调整父窗口大小以适应边框容器
     * @param newGeometry 边框容器的新几何信息
     */
    void adjustParentWindowSize(const QRect& newGeometry);
    
private:
    // 处理左边框拖拽事件
    void getLeftScaleEvent(QPoint movPoint);
    // 处理右边框拖拽事件
    void getRightScaleEvent(QPoint movPoint);
    // 处理底部边框拖拽事件
    void getBottomScaleEvent(QPoint movPoint);
    // 处理顶部边框拖拽事件
    void getTopScaleEvent(QPoint movPoint);
    // 处理右下角拖拽事件
    void getRBScaleEvent(QPoint movPoint);
    // 处理右上角拖拽事件
    void getRTScaleEvent(QPoint movPoint);
    // 处理左上角拖拽事件
    void getLTScaleEvent(QPoint movPoint);
    // 处理左下角拖拽事件
    void getLBScaleEvent(QPoint movPoint);
    
    // 边框代理类，用于捕获鼠标事件
    class MyBorder : public QLabel
    {
    public:
        // 设置边框属性
        MyBorder(QWidget *parent, BorderType type, MyBorderContainer *outClass);
        
    protected:
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseMoveEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
        
    private:
        MyBorderContainer *container; // 保存外部类指针
        BorderType MyType;            // 边框类型
        QPoint mousePointOld;         // 鼠标上一次位置
        bool KeepDrag;                // 是否保持拖拽
    };

    uint16_t m_borderSize;           // 边框捕获区大小
    uint16_t m_minWindowHeight;      // 最小窗口高度
    uint16_t m_minWindowWidth;       // 最小窗口宽度
    bool m_resizable;                // 是否启用缩放

    QWidget *m_centralWidget;        // 中央窗口
    QWidget *m_borderContainer;      // 边框容器
    QGridLayout *m_gridLayout;       // 网格布局
    
    // 边框代理控件
    MyBorder *labelLft;  // 左边框
    MyBorder *labelRit;  // 右边框
    MyBorder *labelBot;  // 底部边框
    MyBorder *labelTop;  // 顶部边框
    MyBorder *labelRB;   // 右下角
    MyBorder *labelRT;   // 右上角
    MyBorder *labelLB;   // 左下角
    MyBorder *labelLT;   // 左上角
};

#endif // MYBORDERCONTAINER_H 
