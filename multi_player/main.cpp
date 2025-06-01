#include "player.h"
#include <QApplication>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QTimer>
#include <QQuickWidget>
#include "FileHandler_read.h"
#include "mywindow.h"
#include "fileuploader_av.h"
#include "write.h"

class FloatWindow : public QQuickWidget {
public:
    FloatWindow(QWidget *parent = nullptr); // 修正构造函数声明
};


int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true); // 必须放在最前面
    QApplication a(argc, argv);

    // 读文件注册文件处理类
    qmlRegisterType<FileHandler>("Custom.IO", 1, 0, "FileIO");

    // 注册上传视频api
    qmlRegisterType<fileuploader_av>("av_upload", 1, 0, "FileUploader");

    qmlRegisterType<Write>("Database", 1, 0, "SqlHandler");


//   ///////////////////
//    const QString jsonData =  R"(  {
//                                  "analysis": {
//                                      "basic": {
//                                          "acousticness": 1.0,
//                                          "danceability": 1.7246,
//                                          "duration_ms": 344253.3333,
//                                          "energy": 0.1725,
//                                          "instrumentalness": -0.0,
//                                          "liveness": 0.09,
//                                          "loudness": -59.0197,
//                                          "speechiness": 1866.5144,
//                                          "tempo": 92.2852,
//                                          "valence": 2088.9172
//                                      },
//                                      "dynamics": {
//                                          "energy_curve": 0.463,
//                                          "tempo_variation": 0.0
//                                      },
//                                      "emotion": {
//                                          "energy": 0.1725,
//                                          "intensity": 1044.5448,
//                                          "type": "平和",
//                                          "valence": 2088.9172
//                                      }
//                                  },
//                                  "learning_status": {
//                                      "samples": 124,
//                                      "updated": "2025-03-27 00:23:47",
//                                      "version": 131899012033
//                                  },
//                                  "prediction": {
//                                      "all_probabilities": {
//                                          "blues": 0.0402,
//                                          "classical": 0.0094,
//                                          "country": 0.0301,
//                                          "disco": 0.0083,
//                                          "hip-hop": 0.0127,
//                                          "jazz": 0.0211,
//                                          "monk": 0.0079,
//                                          "pop": 0.0432,
//                                          "reggae": 0.0113,
//                                          "rock": 0.8159
//                                      },
//                                      "confidence": 0.8159,
//                                      "genre": "rock"
//                                  },
//                                  "status": "success"
//                              })";
//        Write ajk;
//        ajk.processData("file_for_rfadsfead/ennergy.txt",jsonData);
//      ///////////////////

    const QSize windowSize(800, 600);


    Widget mainWindow;
    Player w;
    mainWindow.setWindowFlags(mainWindow.windowFlags() | Qt::FramelessWindowHint);
    w.setWindowFlags(w.windowFlags() | Qt::FramelessWindowHint);


    // 初始位置统一设置为相同坐标
    const QPoint initialPos(100, 100);
    mainWindow.resize(windowSize);
    w.resize(windowSize);
    mainWindow.move(initialPos);
    w.move(initialPos);  // 保持初始位置一致

    // 默认显示w，隐藏mainWindow
    w.show();           // 修改：初始显示w
    mainWindow.hide();  // 修改：隐藏mainWindow

    // 直接切换实现
    auto switchToPlayer = [&]() {
        // 同步位置后切换显示
        w.move(mainWindow.pos());
        mainWindow.hide();
        w.show();
    };

    auto switchToMain = [&]() {
        // 同步位置后切换显示
        mainWindow.move(w.pos());
        w.hide();
        mainWindow.show();
    };

    // 保留尺寸同步逻辑
    QObject::connect(&mainWindow, &Widget::sizeSyncRequested, &w, &Player::syncSize);
    QObject::connect(&w, &Player::sizeSyncRequested, &mainWindow, &Widget::syncSize);

    // 连接切换信号
    QObject::connect(&mainWindow, &Widget::back, switchToPlayer);
    QObject::connect(&w, &Player::back, switchToMain);

    QObject::connect(&w, &Player::change_tepy,&mainWindow,&Widget::change_type);


    return a.exec();


}
