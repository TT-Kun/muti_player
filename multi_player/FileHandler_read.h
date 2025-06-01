#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtDebug>

class FileHandler : public QObject {
    Q_OBJECT

public:
    // 暴露给 QML 调用的方法
    Q_INVOKABLE QString readFile(const QString &path) {
        // 处理 qrc 资源路径
        QString adjustedPath = path;
        if(path.startsWith("qrc:/")) {
            adjustedPath = ":" + path.mid(4); // 转换为 Qt 资源系统路径
        }

        QFile file(adjustedPath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "文件打开失败:" << file.errorString();
            emit errorOccurred(file.errorString());
            return "";
        }

        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString content = in.readAll();
        file.close();

        emit fileReadSuccess();
        return content;
    }

signals:
    // 操作结果信号
    void fileReadSuccess();
    void errorOccurred(const QString &msg);
};
