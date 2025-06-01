#include "mywindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QQuickWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QQuickWidget>
#include "theme.h"
#include <QQmlContext>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    // 设置无边框窗口
     setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
     // 创建自定义标题栏
      m_titleBar = new TitleBar(this);
      m_titleBar->setTitle("mutl_player");

     // ================== 主布局 ==================
     QVBoxLayout *mainLayout = new QVBoxLayout(this);
     mainLayout->setContentsMargins(0, 0, 0, 0);
     mainLayout->setSpacing(0);

     // 添加标题栏到布局顶部
     mainLayout->addWidget(m_titleBar);

     // ================== 内容区域 ==================
     QWidget *contentWidget = new QWidget(this);
     contentWidget->setStyleSheet("background: #F0F0F0;");

     QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
     contentLayout->setContentsMargins(0, 0, 0, 0);
     contentLayout->setSpacing(0);

     // 创建堆叠窗口（父对象必须为contentWidget）
     QStackedWidget *contentStack = new QStackedWidget(contentWidget);
     // 创建并配置QML组件
     qmlWidget = new QQuickWidget(contentStack); // 关键：设置父对象
     qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
     qmlWidget->rootContext()->setContextProperty("backend", this);// // 将当前 Widget 实例暴露给 QML，命名为 "backend"
     qmlWidget->setSource(QUrl("qrc:/qml/mian.qml"));
     // 强制透明背景（必须）
     qmlWidget->setAttribute(Qt::WA_TranslucentBackground);
     qmlWidget->setClearColor(Qt::transparent);
     // 将QML组件添加到堆叠窗口
     contentStack->addWidget(qmlWidget);

     // 将堆叠窗口添加到内容布局
     contentLayout->addWidget(contentStack);

     // 将内容区域添加到主布局
     mainLayout->addWidget(contentWidget, 1);


    // 连接标题栏信号
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &QWidget::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this](){
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_titleBar, &TitleBar::closeClicked, this, &QWidget::close);
    connect(m_titleBar, &TitleBar::doubleClicked, this, [this](){
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_titleBar, &TitleBar::backRequested, this,&Widget::back);

    setMinimumSize(800, 600);
     resize(1024, 768);

}
