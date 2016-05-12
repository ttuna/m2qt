#include "websockethelper.h"
#include <windows.h>
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
WebSocketHelper::WebSocketHelper(QObject *in_parent) : QObject(in_parent)
{

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebSocketHelper::init(const QVariantMap &in_params)
{
    m_p_web_sock = new QWebSocket();
    if (m_p_web_sock == nullptr) return false;

    if (update(in_params) == false) return false;

    QObject::connect(m_p_web_sock, &QWebSocket::connected, this, &WebSocketHelper::slotOnConnected);
    QObject::connect(m_p_web_sock, &QWebSocket::disconnected, this, &WebSocketHelper::slotOnDisconnected);
    QObject::connect(m_p_web_sock, &QWebSocket::stateChanged, this, &WebSocketHelper::slotOnStateChanged);
    QObject::connect(m_p_web_sock, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &WebSocketHelper::slotOnError);
    QObject::connect(m_p_web_sock, &QWebSocket::textMessageReceived, this, &WebSocketHelper::slotOnTextMessage);

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::cleanup()
{

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebSocketHelper::update(const QVariantMap &in_params)
{
    if (in_params.contains("msg_template"))
    {
        m_msg_template = in_params["msg_template"].toString();
        qDebug() << "WebSocketHelper::update - msg_template:" << m_msg_template;
    }

    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebSocketHelper::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::connect(const QUrl &in_url)
{
    if (m_initialized == false) return;
    if (in_url.isEmpty()) return;

    QNetworkRequest ws_req(in_url);
    ws_req.setRawHeader(QByteArray("Origin"), QByteArray("http://").append(in_url.host().toLatin1()));

    qDebug() << "WebSocketHelper::connect - NetworkRequest url:" << ws_req.url();
    qDebug() << "WebSocketHelper::connect \tOrigin:" << ws_req.rawHeader(QByteArray("Origin"));

    m_p_web_sock->open(ws_req);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::send(const QByteArray &in_type, const QByteArray &in_data)
{
    if (m_initialized == false) return;
    if (m_p_web_sock->isValid() == false) return;
    if (m_ws_connected == false) return;

    QString msg_str = m_msg_template.arg(QLatin1String(in_type)).arg(QLatin1String(in_data));
    m_p_web_sock->sendTextMessage(msg_str);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnConnected()
{
    qDebug() << "WebSocketHelper::slotOnConnected";
    m_ws_connected = true;
    Sleep(10);
    send("message", "Hello Mongrel!");
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnDisconnected()
{
    qDebug() << "WebSocketHelper::slotOnDisconnected";
    m_ws_connected = true;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "WebSocketHelper::slotOnStateChanged:" << state;
    if (state != QAbstractSocket::ConnectedState) m_ws_connected = false;
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

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
QString WebSocketHelper::msgTemplate() const
{
    return m_msg_template;
}

void WebSocketHelper::setMsgTemplate(QString msg_template)
{
    if (m_msg_template == msg_template)
        return;

    m_msg_template = msg_template;
    emit signalMsgTemplateChanged(msg_template);
}

