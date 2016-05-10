
#define QT_NO_SSL

#include <QCoreApplication>
#include <QObject>
#include <QtWebSockets/QWebSocket>

#include <windows.h>

#include "websockethelper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QWebSocket test_sock;
    WebSocketHelper helper;
    QObject::connect(&test_sock, &QWebSocket::connected, &helper, &WebSocketHelper::slotOnConnected);
    QObject::connect(&test_sock, &QWebSocket::disconnected, &helper, &WebSocketHelper::slotOnDisconnected);
    QObject::connect(&test_sock, &QWebSocket::stateChanged, &helper, &WebSocketHelper::slotOnStateChanged);
    QObject::connect(&test_sock, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), &helper, &WebSocketHelper::slotOnError);
    QObject::connect(&test_sock, &QWebSocket::textMessageReceived, &helper, &WebSocketHelper::slotOnTextMessage);

    //QUrl url("ws://157.247.245.36:6767/websocket/");
    QUrl url("ws://192.168.0.13:6767/websocket/");
    QNetworkRequest ws_req(url);
    //ws_req.setRawHeader(QByteArray("Origin"), QByteArray("http://157.247.245.36"));
    ws_req.setRawHeader(QByteArray("Origin"), QByteArray("http://192.168.0.13"));

    qDebug() << "ws_req url:" << ws_req.url();
    qDebug() << "ws_req header:";
    qDebug() << "\tConnection:" << ws_req.rawHeader(QByteArray("Connection"));
    qDebug() << "\tUpgrade:" << ws_req.rawHeader(QByteArray("Upgrade"));
    qDebug() << "\tHost:" << ws_req.rawHeader(QByteArray("Host"));
    qDebug() << "\tOrigin:" << ws_req.rawHeader(QByteArray("Origin"));
    qDebug() << "---";

    test_sock.open(ws_req);

    return a.exec();
}
