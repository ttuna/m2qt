#ifndef WEBSOCKETHELPER_H
#define WEBSOCKETHELPER_H

#include <QObject>
#include <QString>
#include <QWebSocket>

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
#endif // WEBSOCKETHELPER_H
