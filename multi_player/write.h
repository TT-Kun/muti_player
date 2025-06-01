// Write.h
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <algorithm>
#include <QCoreApplication>  // 添加这行
#include <QFileInfo>         // 路径处理需要
#include <QtDebug>
#include <QDir>
#include <fstream>
#include <iostream>

class Write : public QObject {
    Q_OBJECT
public:
    explicit Write(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE void processData(const QString &basePath, const QString &jsonStr) {


        // 解析JSON数据
         QPair<QJsonObject, QJsonObject> data = parseJsonData(jsonStr);
         // 分别处理不同数据类别
         processDataCategory(data.first, "ennergy.txt", "emotion");
         processDataCategory(data.second, "genre.txt", "probabilities");
    }

private:


    // 分类处理核心逻辑
    void processDataCategory(const QJsonObject &dataObj,
                            const QString &path,
                            const QString &logPrefix) {
        // 1. 转换路径
        QString adjustedPath = convertPath(path);


        // 2. 提取数据
        QMap<QString, double> newData;
        extractData(dataObj, newData); //​​从 JSON 对象中提取数值数据到 Map （newData）

        // 3. 合并写入F:/QT_project/player3.0_1/spark_player_static/file_for_read/ennergy.txt
        mergeAndWriteData(adjustedPath, newData);//这里面会先读取之前的数据
    }


    // JSON解析（返回两个数据对象）
    QPair<QJsonObject, QJsonObject> parseJsonData(const QString &jsonStr) {
                    QJsonParseError error;
                    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

                    if (error.error != QJsonParseError::NoError) {
                        qWarning() << "JSON解析错误:" << error.errorString();
                        return {};
                    }

                    QJsonObject root = doc.object();
                    QJsonObject emotionData;
                    QJsonObject probabilitiesData;

                    // 提取 emotion.energy
                    if (root.contains("analysis")) {
                        QJsonObject analysis = root["analysis"].toObject();
                        if (analysis.contains("emotion")) {
                            QJsonObject emotion = analysis["emotion"].toObject();
                            if (emotion.contains("energy")) {
                                emotionData["energy"] = emotion["energy"];
                            }
                    }

                    // 提取 all_probabilities
                    if (root.contains("prediction")) {
                        QJsonObject prediction = root["prediction"].toObject();
                        if (prediction.contains("all_probabilities")) {
                            probabilitiesData = prediction["all_probabilities"].toObject();
                        }
                    }

                    return {emotionData, probabilitiesData};
                }
     }

    // 路径转换逻辑
    QString convertPath(const QString &originalPath) {
        QString ip;
        QFile file1(":/file_for_read/path.txt");
        if (file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file1);
             ip = in.readLine().trimmed();
             qDebug() << "path频配置文件"<<ip;
        }else{
            qDebug() << "无法读path取配置文件";
        }
            return ip+"/"+originalPath;
    }


    // 数据合并和写入
    void mergeAndWriteData(const QString &path, const QMap<QString, double> &newData) {
        // 读取已有数据
        QMap<QString, double> existingData = readExistingData(path);

        // 合并新数据
        for (auto it = newData.begin(); it != newData.end(); ++it) {
            existingData[it.key()] += it.value();
        }

        // 转换为排序列表
        QList<QPair<double, QString>> sortedList;
        for (auto it = existingData.constBegin(); it != existingData.constEnd(); ++it) {
            sortedList.append(qMakePair(it.value(), it.key()));
        }

        // 降序排序
        std::sort(sortedList.begin(), sortedList.end(),
                 [](const QPair<double, QString>& a, const QPair<double, QString>& b) {
                     return a.first > b.first;
                 });

        qDebug() << path;

        QFileInfo fileInfo(path);
          // 1. 创建父目录（如果不存在）
          QDir parentDir = fileInfo.absoluteDir();
          if (!parentDir.exists()) {
              if (!parentDir.mkpath(".")) {
                  qDebug() << "目录创建失败：" << parentDir.absolutePath();
              }
          }

          // 2. 检查文件是否存在
          if (fileInfo.exists()) {
              qDebug() << "文件已存在：" << path;
          }


         QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setRealNumberPrecision(4);
            out.setRealNumberNotation(QTextStream::FixedNotation);
            for (const auto &pair : sortedList) {
                out << pair.second << "\t"
                    << QString::number(pair.first, 'f', 4) << "\n";
            }
            emit success();
        } else {
            qDebug() << "文件打开失败:" << path << file.errorString();
            emit error(tr("写入失败: ") + file.errorString());
        }
    }

    //  ​​从 JSON 对象中提取数值数据到 Map
    void extractData(const QJsonObject &obj, QMap<QString, double> &data) {
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            if (it.value().isDouble()) {
                QString key = it.key().trimmed();
                double value = it.value().toDouble();
                data[key] += value;
            }
        }
    }

    // 读取已有数据
    QMap<QString, double> readExistingData(const QString &path) {
        QMap<QString, double> data;

        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
               auto line = in.readLine().trimmed();
                if (line.isEmpty()) continue;

                QStringList parts = line.split("\t");
                if (parts.size() == 2) {
                    bool ok;
                    double value = parts[1].toDouble(&ok);
                    if (ok) data[parts[0].trimmed()] = value;
                }
            }
            file.close();
        }
        return data;
    }

signals:
    void success();
    void error(const QString &msg);
};
