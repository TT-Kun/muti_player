#include "fileuploader_av.h"
#include <QFile>

fileuploader_av::fileuploader_av(QObject *parent) : QObject(parent) , timeoutTimer(new QTimer(this))
{
    timeoutTimer->setSingleShot(true);// 设置定时器为单次触发模式
    connect(timeoutTimer, &QTimer::timeout, this, &fileuploader_av::handleNetworkTimeout);
}


void fileuploader_av::openFilePicker()
{
    QString path = QFileDialog::getOpenFileName(
        nullptr,
        "选择上传文件",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "All Files (*.*)"
    );

    if (!path.isEmpty()) {
        uploadFile(path);
    }else{
         emit failure("没有打开文件");
         return ;
    }
}




void fileuploader_av::uploadFile(const QString &filePath)
{
    cleanup();
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
         emit failure("无法打开文件");
        return;
    }
    initRequest(filePath);
    startTimeout();//开始及时
}

void fileuploader_av::initRequest(const QString &filePath)
{

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QFile *file = new QFile(filePath);
    file->open(QIODevice::ReadOnly);


    QHttpPart filePart;

    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
           QString("form-data; name=\"video\"; filename=\"%1\"")  // ✅ 核心修改点
               .arg(QFileInfo(filePath).fileName()));  // 必须严格对应图片中的参数名"video"

    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

      QString url ;
       QFile file1(":/file_for_read/http_av.txt");
       if (file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
           QTextStream in(&file1);
           QString ip = in.readLine().trimmed();
           url = QString("http://%1/analyze").arg(ip);
            qDebug() << "av频配置文件"<<url;
       }else{
           qDebug() << "无法读取av配置文件";
       }

    QNetworkRequest request(url);//上传地址
    qDebug() << "上传地址;losdijflkasds频av配置文件"<<url;

    reply = manager.post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, &fileuploader_av::handleUploadFinished);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                this, &fileuploader_av::handleError);

    startTimeout();
}

void fileuploader_av::handleUploadFinished()
{
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {

        emit success(QString::fromUtf8(reply->readAll()));

    } else {
        emit failure(reply->errorString());
    }
    cleanup();
}


void fileuploader_av::handleNetworkTimeout()
{
    if (reply && reply->isRunning()) {
        reply->abort();
        emit failure("请求超时");
        cleanup();
    }
}

void fileuploader_av::handleError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)
    if (reply) emit failure(reply->errorString());
}


void fileuploader_av::cleanup()
{
    timeoutTimer->stop();
    if (reply) {
        reply->deleteLater();
        reply = nullptr;
    }
}


void fileuploader_av::startTimeout()
{
    timeoutTimer->start(m_timeout);
}


int fileuploader_av::timeout() const { return m_timeout; }

void fileuploader_av::setTimeout(int newTimeout)
{
    if (m_timeout != newTimeout) {
        m_timeout = newTimeout;
        emit timeoutChanged();
    }
}

