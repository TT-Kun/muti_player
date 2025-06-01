#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    // 设置标题文本
    void setTitle(const QString& title);
    
    // 更新标题栏样式（深色/浅色主题）
    void updateStyle(bool isDarkTheme);
    void updateStyle_ha(bool isDarkTheme);

    void testSkyBlueBackground();  // 测试函数

signals:
    void minimizeClicked();      // 最小化按钮点击信号
    void maximizeClicked();      // 最大化按钮点击信号
    void closeClicked();         // 关闭按钮点击信号
    void doubleClicked();        // 双击标题栏信号
    void backRequested();////

protected:
    // 拖动相关事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void setupUI();          // 设置UI组件
    void connectSignals();   // 连接信号槽

    QLabel *m_titleLabel;    // 标题标签
    QPushButton *m_minButton;    // 最小化按钮
    QPushButton *m_maxButton;    // 最大化按钮
    QPushButton *m_closeButton;  // 关闭按钮
    
    QPoint m_startPos;       // 拖动起始位置
    bool m_isPressed;        // 鼠标按下状态
    bool m_isMaximized;      // 窗口是否最大化

    QPushButton *backBtn;//////////
};

#endif // TITLEBAR_H
