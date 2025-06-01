#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include <QObject>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDebug>
#include <QTimer>

class FileUploader : public QObject
{
    Q_OBJECT

public:
    // 构造时指定后端地址
    explicit FileUploader(const QString &uploadUrl,
                         QObject *parent = nullptr);

    // 设置允许的音频后缀（默认包含常见格式）
    void setAllowedExtensions(const QStringList &extensions);

    // 主操作接口
    void upload(const QString &filePath);

signals:
    // 上传进度信号（可选）
    void progress(qint64 bytesSent, qint64 bytesTotal);

    // 结果信号
    void success(const QByteArray &responseData);
    void error(const QString &errorString);

private:
    QString m_uploadUrl;
     QNetworkAccessManager *manager;
    QStringList m_allowedExtensions;
};

#endif // FILEUPLOADER_H
