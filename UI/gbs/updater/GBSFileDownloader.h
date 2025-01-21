#pragma once

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QUrl>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr)
        : QObject(parent),
          isInMemory(false)  // 默认不保存到内存
    {
        manager = new QNetworkAccessManager(this);
    }

    void downloadFile(const QUrl &url, const QString &saveFilePath, bool saveToMemory = false)
    {
        // 创建请求
        QNetworkRequest request(url);

        // 发送 GET 请求
        reply = manager->get(request);

        // 设置是否保存到内存
        this->isInMemory = saveToMemory;


        // 连接信号与槽
        connect(reply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished);
        connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::onReadyRead);

        // 设置保存路径
        this->saveFilePath = saveFilePath;
    }

    bool downloadFileSync(const QUrl &url, const QString &saveFilePath, bool saveToMemory = false)
    {
	    // 创建请求
	    QNetworkRequest request(url);

	    // 发送 GET 请求
	    reply = manager->get(request);

	    // 设置是否保存到内存
	    this->isInMemory = saveToMemory;

	    // 设置保存路径
	    this->saveFilePath = saveFilePath;

	    //// 阻塞直到下载完成
	    //if (!reply->waitForFinished()) {
		   // qDebug() << "Error occurred while downloading:" << reply->errorString();
		   // return false;
	    //}

	    // 处理下载后的内容
	    if (reply->error() == QNetworkReply::NoError) {
		    qDebug() << "File downloaded successfully!";
		    if (isInMemory) {
			    // 如果保存到内存，将内容存入 QByteArray
			    downloadedData = reply->readAll();
			    qDebug() << "Downloaded data in memory:" << downloadedData;
		    } else {
			    // 否则保存到文件
			    QFile file(saveFilePath);
			    if (file.open(QIODevice::WriteOnly)) {
				    file.write(reply->readAll());
				    file.close();
			    } else {
				    qDebug() << "Failed to open file for writing.";
				    return false;
			    }
		    }
		    return true;
	    } else {
		    qDebug() << "Error occurred while downloading:" << reply->errorString();
		    return false;
	    }
    }
    QByteArray getDownloadedData() const
    {
        return downloadedData;
    }

private slots:
    void onReadyRead()
    {
        // 当数据准备好时，将数据写入文件或内存
        QByteArray data = reply->readAll();

        if (isInMemory) {
            // 如果要保存到内存，将数据追加到 QByteArray 中
            downloadedData.append(data);
        } else {
            // 否则将数据写入文件
            if (!file.isOpen()) {
                file.setFileName(saveFilePath);
                file.open(QIODevice::WriteOnly);
            }
            file.write(data);
        }
    }

    void onDownloadFinished()
    {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "File downloaded successfully!";
            if (isInMemory) {
                // 如果下载成功且保存到内存，则输出内容（如果需要）
                qDebug() << "Downloaded data in memory:" << downloadedData;
            }
        } else {
            qDebug() << "Error occurred while downloading:" << reply->errorString();
        }

        // 关闭文件和回复
        if (!isInMemory) {
            file.close();
        }
        reply->deleteLater();
	emit sigDownloadFinished();
    }

signals:
    void sigDownloadFinished();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile file;
    QString saveFilePath;

    QByteArray downloadedData;  // 用于保存到内存的数据
    bool isInMemory;  // 标志位，决定是否保存到内存
};

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);

//     // 远程文件 URL 和保存的本地文件路径
//     QUrl url("https://renew.guobo.shop/exe/gbpc/appcast.xml");
//     QString saveFilePath = "appcast.xml";

//     FileDownloader downloader;

//     // 设置保存到内存，保存到内存后可以访问数据
//     downloader.downloadFile(url, saveFilePath, true);

//     return a.exec();
// }

