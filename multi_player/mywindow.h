#include <QWidget>
#include <QHBoxLayout>
#include <QQuickWidget>
#include "titlebar.h"
#include <QDebug>  // 必须包含的头文件
#include <QMetaObject>

class Widget : public QWidget {
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    enum ThemeType { Dark, Light };
     Q_ENUM(ThemeType)

signals:
     void back();//切换至主页
    void sizeSyncRequested(const QSize& size);
     void themeChanged(bool type);

public slots:
    void syncSize(const QSize& size) {
        if (m_syncing) return;
                m_syncing = true;
        if (this->size() != size) {
            this->resize(size); // 异步调整
        }
        m_syncing = false;
    }

    void  change_type( bool isDark){
        m_titleBar->updateStyle(!isDark);
        if(isDark){
            emit themeChanged( 1);
            qDebug() << "Value:dui baise";
        }else{
             emit themeChanged(0);
             qDebug() << "Value:cuo";
        }

    }



protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        emit sizeSyncRequested(event->size());
    }
 ///////////////////////  用来实现大小的双向绑定的
private:
     bool m_syncing = false;

    TitleBar *m_titleBar;
    QHBoxLayout *layout;        // 水平布局容器[7](@ref)
    QQuickWidget *qmlWidget;     // QML渲染组件[2](@ref)
    QVBoxLayout *m_mainLayout;
};
