#ifndef FILEUPLOADER_AV_H
#define FILEUPLOADER_AV_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QFileDialog>
#include <QHttpMultiPart>  // 新增
#include <QHttpPart>       // 新增
#include <QJsonObject>     // 新增
#include <QJsonDocument>   // 新增
#include <QDebug>

class fileuploader_av : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    explicit fileuploader_av(QObject *parent = nullptr);

    Q_INVOKABLE void openFilePicker();
   Q_INVOKABLE void uploadFile(const QString &filePath);
   int timeout() const;
   void setTimeout(int newTimeout);

signals:
   void success(const QString &responseData);  // 成功时发射
   void failure(const QString &errorMessage);  // 失败时发射（包含超时、错误等
    void timeoutChanged();

private slots:
    void handleNetworkTimeout();
    void handleUploadFinished();
    void handleError(QNetworkReply::NetworkError code);


private:
      void cleanup();
      void startTimeout();
      void initRequest(const QString &filePath);

      QNetworkAccessManager manager;
      QNetworkReply *reply = nullptr;
      QTimer* timeoutTimer;
      int m_timeout = 300000; // 默认15秒超时
};

#endif // FILEUPLOADER_AV_H
