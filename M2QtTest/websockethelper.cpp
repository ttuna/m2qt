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

    m_ping_timer.setTimerType(Qt::VeryCoarseTimer);
    m_ping_timer.setInterval(10000);

    connect(m_p_web_sock, &QWebSocket::connected, this, &WebSocketHelper::slotOnConnected);
    connect(m_p_web_sock, &QWebSocket::disconnected, this, &WebSocketHelper::slotOnDisconnected);
    connect(m_p_web_sock, &QWebSocket::stateChanged, this, &WebSocketHelper::slotOnStateChanged);
    connect(m_p_web_sock, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &WebSocketHelper::slotOnError);
    connect(m_p_web_sock, &QWebSocket::textMessageReceived, this, &WebSocketHelper::slotOnTextMessage);
    connect(m_p_web_sock, &QWebSocket::pong, this, &WebSocketHelper::slotOnPong);
    connect(&m_ping_timer, &QTimer::timeout, this, &WebSocketHelper::slotPing);

    m_ping_timer.start();

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
        // TODO: parse template and create a json-key string list ...
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
void WebSocketHelper::socketConnect(const QUrl &in_url)
{
    if (m_initialized == false) return;
    if (in_url.isEmpty()) return;

    QNetworkRequest ws_req(in_url);
    qDebug() << "WebSocketHelper::socketConnect - NetworkRequest Url:" << ws_req.url();

    //ws_req.setRawHeader(QByteArray("Origin"), QByteArray("http://").append(in_url.host().toLatin1()));
    //qDebug() << "WebSocketHelper::socketConnect - NetworkRequest Origin:" << ws_req.rawHeader(QByteArray("Origin")) << endl;

    m_p_web_sock->open(ws_req);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::send(const QByteArray &in_type, const QByteArray &in_data)
{
    // TODO: pass a QStringList/QMap to send() and fill template with separated tokens ...
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

    // send greetings after websocket is connected ...
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
    qDebug() << "WebSocketHelper::slotOnStateChanged:\n\t" << state;
    if (state != QAbstractSocket::ConnectedState) m_ws_connected = false;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocketHelper::slotOnError:\n\t" << error;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnTextMessage(const QString &message)
{
    qDebug() << "WebSocketHelper::slotOnTextMessage:\n\t" << message;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotPing()
{
    if (m_initialized == false) return;
    if (m_p_web_sock->state() != QAbstractSocket::ConnectedState) return;

    qDebug() << "WebSocketHelper::slotPing - send ping to server";
    m_p_web_sock->ping(QByteArray("-Heartbeat-"));
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void WebSocketHelper::slotOnPong(quint64 elapsedTime, const QByteArray &payload)
{
    qDebug() << "WebSocketHelper::slotOnPong - time:" << elapsedTime << " data:" << payload;
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

