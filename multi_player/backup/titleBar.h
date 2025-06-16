#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>

namespace Ui {
class TitleBar;
}

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    // 设置标题文本（文件名）
    void setTitle(const QString &title);
    
    // 更新主题
    void updateTheme(const QString &theme);
    
    // 根据窗口状态更新最大化按钮图标
    void updateMaximizeButton(bool isMaximized);

protected:
    // 用于拖动窗口的鼠标事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    
    // 处理窗口状态变化事件
    void changeEvent(QEvent *event) override;

private slots:
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();

signals:
    void maximizeClicked();
    void doubleClicked();

private:
    Ui::TitleBar *ui;
    QPoint m_dragPosition;
    QString m_currentTheme;
    
    // 初始化连接
    void setupConnections();
};

#endif // TITLEBAR_H 