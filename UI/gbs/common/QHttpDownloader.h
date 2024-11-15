#pragma once


#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QObject>
#include "QBizLogger.h"


class [[deprecated]]  QHttpDownloader : public QObject {
	Q_OBJECT
public:
	// 获取单例实例的方法
	static QHttpDownloader *getInstance()
	{
		static QHttpDownloader instance;
		return &instance;
	}

	// 启动下载任务
	void downloadFile(const QUrl url, const QString savePath)
	{

		// 创建一个独立的下载任务对象
		QNetworkRequest request(url);
		QNetworkReply *reply = m_networkManager->get(request);

		QFile *file = new QFile(savePath);
		if (!file->open(QIODevice::WriteOnly)) {
			QLogE("Failed to open file for writing:", savePath.toStdString().c_str());
			reply->abort();
			reply->deleteLater();
			delete file;
			return;
		}
		


		connect(reply, &QNetworkReply::downloadProgress, this, [](qint64 bytesReceived, qint64 bytesTotal) {
			qDebug() << "Download progress:" << bytesReceived << "of" << bytesTotal;
		});

		connect(m_networkManager, &QNetworkAccessManager::finished, this,
			[this, reply, file, savePath](QNetworkReply *reply) {
			qDebug() << "Download finished signal emitted";

			file->close();
			file->deleteLater();
			reply->deleteLater();
			emit downloadFinished(savePath);
		});

		connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError code) {
			qDebug() << "Download error:" << code;
			QLogE("Download error:", code);
			reply->deleteLater();
		});
	}

signals:
	void downloadFinished(QString);

private:
	// 私有构造函数，保证单例模式
	explicit QHttpDownloader(QObject *parent = nullptr)
		: QObject(parent),
		  m_networkManager(new QNetworkAccessManager(this))
	{
	}

	// 禁止复制和赋值
	QHttpDownloader(const QHttpDownloader &) = delete;
	QHttpDownloader &operator=(const QHttpDownloader &) = delete;

	QNetworkAccessManager *m_networkManager;
};
