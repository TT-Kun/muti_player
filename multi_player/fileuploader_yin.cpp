#include "fileuploader_yin.h"
#include "write.h"

#include <QTextCodec>  // 添加这行

FileUploader::FileUploader(const QString &uploadUrl, QObject *parent)
    :manager(new QNetworkAccessManager(this)),
      QObject(parent),
      m_uploadUrl(uploadUrl),
      m_allowedExtensions{"mp3", "wav", "ogg", "flac", "aac", "m4a", "wma"}

{
}

void FileUploader::setAllowedExtensions(const QStringList &extensions)
{
    m_allowedExtensions = extensions;
}

void FileUploader::upload(const QString &filePath)
{
    QFileInfo info(filePath);
    QString suffix = info.suffix().toLower();

    // 验证音频文件类型
    if (!m_allowedExtensions.contains(suffix)) {
        //QtDebug()<<"文件类型错误"
         qDebug() << "开始播放啦啦啦啦  但是文件类型错了";
        emit error(tr("Unsupported aufile type: %1").arg(suffix));
        return;
    }

    // 处理本地文件上传
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
         qDebug() << "打开文件错了";
        emit error(tr("Cannot open file: %1").arg(filePath));
        delete file;
        return;
    }


    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 构建文件部分
    QHttpPart filePart;
    QMimeDatabase mimeDB;
    QMimeType mimeType = mimeDB.mimeTypeForFile(info);
     filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.name());
     filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
         QString("form-data; name=\"file\"; filename=\"%1\"")  // ✅ 修正字段名
             .arg(info.fileName()));

     // 修改字段名
     filePart.setBodyDevice(file);
     file->setParent(multipart);
     multipart->append(filePart);


     // 构建网络请求（添加必要请求头）
     QUrl requestUrl(m_uploadUrl);
     QNetworkRequest request(requestUrl);
     // 设置多部分表单的Content-Type
     // request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");//要注释掉  因为qt是自带的  不然的话就会报错
     request.setRawHeader("Accept", "application/json");
     // 设置客户端标识（版本号与功能模块匹配）
     request.setHeader(QNetworkRequest::UserAgentHeader, "VideoUploadClient/1.0");
     // 发送POST请求
     QNetworkReply* reply = manager->post(request, multipart);
     multipart->setParent(reply); // 绑定内存管理

    QTimer *timeoutTimer = new QTimer(reply);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(95000); // 30秒超时

    // 连接超时处理
     QObject::connect(timeoutTimer, &QTimer::timeout, [=]() {
         qDebug() << "【超时】请求超时，已取消上传：" << filePath;
         // 终止请求并释放资源
         if (reply->isRunning()) {
             reply->abort();
         }
         reply->deleteLater();
     });


     // 连接进度信号（保持原有）
        connect(reply, &QNetworkReply::uploadProgress, this, &FileUploader::progress);


     // 处理响应
     connect(reply, &QNetworkReply::finished, [=]() {
         timeoutTimer->stop();

         if (reply->error() == QNetworkReply::NoError) {
             QByteArray response = reply->readAll();
             Write *dataWriter = new Write(this);
             QString basePath = "qrc:/file_for_read";

             // 核心解析逻辑
             QJsonParseError parseError;
             QJsonDocument jsonDoc = QJsonDocument::fromJson(response, &parseError);

             if (parseError.error == QJsonParseError::NoError) {
                 // 格式化为带缩进的JSON字符串（与示例样式一致）
                 QString formattedJson = jsonDoc.toJson(QJsonDocument::Indented);
                 // 传递给写入器
                 dataWriter->processData(basePath + "/energy.txt", formattedJson);
             } else {
                 qDebug() << "JSON解析错误：" << parseError.errorString();
                 qDebug() << "原始数据：" << response;
             }

             reply->deleteLater();
         }

     });


    // 错误处理
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
           [=](QNetworkReply::NetworkError code) {
               qDebug() << "【网络错误】错误代码：" << code;
           });

}
