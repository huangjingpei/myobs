#pragma once


#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QUrl>
#include <QDebug>

class WebSocketClient : public QObject {

public:
	WebSocketClient(const QString &url, QObject *parent = nullptr)
		: QObject(parent),
		  m_url(url)
	{
		connect(&m_webSocket, &QWebSocket::connected, this,
			&WebSocketClient::onConnected);
		connect(&m_webSocket, &QWebSocket::disconnected, this,
			&WebSocketClient::onDisconnected);
		connect(&m_webSocket, &QWebSocket::textMessageReceived, this,
			&WebSocketClient::onTextMessageReceived);

		// 初始化心跳定时器
		m_heartbeatTimer.setInterval(30000); // 每30秒发送一次心跳
		connect(&m_heartbeatTimer, &QTimer::timeout, this,
			&WebSocketClient::sendHeartbeat);
	}

	void connectToServer() { m_webSocket.open(QUrl(m_url)); }

	void sendMessage(const QString &message)
	{
		m_webSocket.sendTextMessage(message);
	}

private slots:
	void onConnected()
	{
		qDebug() << "WebSocket connected";
		m_heartbeatTimer.start(); // 启动心跳定时器
	}

	void onDisconnected()
	{
		qDebug() << "WebSocket disconnected";
		m_heartbeatTimer.stop(); // 停止心跳定时器
	}

	void onTextMessageReceived(const QString &message)
	{
		qDebug() << "Message received:" << message;
	}

	void sendHeartbeat()
	{
		// 发送心跳消息
		qDebug() << "Sending heartbeat";
		// m_webSocket.sendTextMessage("heartbeat"); // 你可以根据需要自定义心跳消息
	}

private:
	QWebSocket m_webSocket;
	QString m_url;
	QTimer m_heartbeatTimer; // 心跳定时器
};
