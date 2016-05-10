#include <QString>
#include <QtWebSockets/QWebSocket>
#include <QtTest>
#include <QtConcurrent>
#include <QThread>
#include <QCoreApplication>

#include <zmq.hpp>
#include "m2qt.h"

// ----------------------------------------------------------------------------
//
// class WebSocketHelper
//
// ----------------------------------------------------------------------------
class WebSocketHelper final : public QObject
{
    Q_OBJECT
public:
    WebSocketHelper() = default;

public slots:
    void slotOnConnected();
    void slotOnDisconnected();
    void slotOnStateChanged(QAbstractSocket::SocketState state);
    void slotOnError(QAbstractSocket::SocketError error);
    void slotOnTextMessage(const QString &message);

};
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnConnected()
{
    qDebug() << "slotOnConnected";
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnDisconnected()
{
    qDebug() << "slotOnDisconnected";
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "slotOnStateChanged:" << state;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnError(QAbstractSocket::SocketError error)
{
    qDebug() << "slotOnError:" << error;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnTextMessage(const QString &message)
{
    qDebug() << "slotOnTextMessage:" << message;
}


// ----------------------------------------------------------------------------
//
// class M2QtTest
//
// ----------------------------------------------------------------------------
class M2QtTest : public QObject
{
    Q_OBJECT

public:
    M2QtTest();

private Q_SLOTS:
    void testCase1();
};
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
M2QtTest::M2QtTest()
{
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void M2QtTest::testCase1()
{
    using namespace M2QT;

#define QT_NO_SSL
    QWebSocket test_sock;
    WebSocketHelper helper;
    connect(&test_sock, &QWebSocket::connected, &helper, &WebSocketHelper::slotOnConnected);
    connect(&test_sock, &QWebSocket::disconnected, &helper, &WebSocketHelper::slotOnDisconnected);
    connect(&test_sock, &QWebSocket::stateChanged, &helper, &WebSocketHelper::slotOnStateChanged);
    connect(&test_sock, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), &helper, &WebSocketHelper::slotOnError);
    connect(&test_sock, &QWebSocket::textMessageReceived, &helper, &WebSocketHelper::slotOnTextMessage);

//    QSslConfiguration ssl_config = QSslConfiguration::defaultConfiguration();
//    test_sock.setSslConfiguration(ssl_config);

//    QUrl url("ws://192.168.0.13:6767/websocket");
    QUrl url("ws://157.247.245.36:6767/websocket/");
    QNetworkRequest ws_req(url);
    ws_req.setRawHeader(QByteArray("Connection"), QByteArray("Upgrade"));
    ws_req.setRawHeader(QByteArray("Upgrade"), QByteArray("websocket"));
    ws_req.setRawHeader(QByteArray("Host"), QByteArray("157.247.245.36"));
    ws_req.setRawHeader(QByteArray("Origin"), QByteArray("http://157.247.245.36"));

    qDebug() << "ws_req Connection header:" << ws_req.rawHeader(QByteArray("Connection"));
    qDebug() << "ws_req Upgrade header:" << ws_req.rawHeader(QByteArray("Upgrade"));
    qDebug() << "ws_req Host header:" << ws_req.rawHeader(QByteArray("Host"));
    qDebug() << "ws_req Origin header:" << ws_req.rawHeader(QByteArray("Origin"));

    test_sock.open(ws_req);

    if (test_sock.error() != QAbstractSocket::UnknownSocketError)
        qDebug() << "error:" << test_sock.errorString();
    else
    {
        qDebug() << "test_sock origin:" << test_sock.origin();
    }

    Sleep(120000);
    qDebug() << test_sock.state();
}

QTEST_APPLESS_MAIN(M2QtTest)

#include "tst_m2qt.moc"
