#include "websockethelper.h"

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnConnected()
{
    qDebug() << "WebSocketHelper::slotOnConnected";
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnDisconnected()
{
    qDebug() << "WebSocketHelper::slotOnDisconnected";
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "WebSocketHelper::slotOnStateChanged:" << state;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocketHelper::slotOnError:" << error;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnTextMessage(const QString &message)
{
    qDebug() << "WebSocketHelper::slotOnTextMessage:" << message;
}
