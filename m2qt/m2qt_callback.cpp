#include "m2qt_callback.h"
#include "m2qt_messageparser.h"

#include <QCryptographicHash>
#include <QJsonObject>
#include <QTextStream>
#include <QDebug>

using namespace M2QT;

namespace {

static CallbackHelper* helper = new M2QT::CallbackHelper();

// ----------------------------------------------------------------------------
//
// Handler callbacks ...
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response DebugOutputHandler(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    QByteArray debug_msg;
    QTextStream debug_stream(&debug_msg);

    // mongrel2 message ...
    debug_stream << "\n-------------------------" << endl;
    debug_stream << "DebugOutputHandler:\n" << endl;
    debug_stream << "\tuuid:" << uuid << endl;
    debug_stream << "\tid:" << id << endl;
    debug_stream << "\tpath:" << path << endl;
    debug_stream << "\tnetstring entries:" << net_strings.count() << endl;

    // json header ...
    debug_stream << "\n\tJson header:" << endl;
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    QJsonObject::const_iterator iter = jobj.constBegin();
    for ( ;iter!=jobj.constEnd(); ++iter )
    {
        debug_stream << "\t\t" << left << iter.key() << ":" << iter.value().toString() << endl;
    }

    // netstring payload ...
    debug_stream << "\n\tPayload:" << endl;
    for(int i = 1; i<net_strings.size(); ++i)
    {
        debug_stream << "\t\t"
                     << std::get<NS_SIZE>(net_strings[i])
                     << ":"
                     << std::get<NS_DATA>(net_strings[i])
                     << endl;
    }
    debug_stream << "-------------------------\n" << endl;

    // remove \0 chars from debug_msg ...
    debug_msg.replace('\0', "");
    helper->signalDebug(QLatin1String(debug_msg));
    return Response();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response WebsocketHandshakeHandler(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    // get HEADER - first NetString must be the header ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("WebsocketHandshakeHandler - No header available!")); return Response(); }

    // get SEC-WEBSOCKET-KEY ...
    QJsonValue val = jobj.value("sec-websocket-key");
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketHandshakeHandler - Couldn't find SEC-WEBSOCKET-KEY header!")); return Response(); }

    // calc SEC-WEBSOCKET-ACCEPT ...
    QByteArray key = val.toString().toLatin1() + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    QByteArray accept = QCryptographicHash::hash(key, QCryptographicHash::Sha1).toBase64();

    // build response body ...
    QByteArray response_data(QLatin1String("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: upgrade\r\nSec-WebSocket-Accept: ").data());
    response_data += accept + "\r\n\r\n";

    NetString rep_id = std::make_tuple(static_cast<quint32>(id.size()), id);

    // build Response ...
    Response rep = std::make_tuple(uuid, rep_id, response_data);
    return rep;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response WebsocketEchoHandler(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    QByteArray response_data; // = "HTTP/1.1 202 Accepted\r\n\r\n";
    for (int i = 1; i<net_strings.size(); ++i)
    {
        response_data += std::get<NS_DATA>(net_strings[i]);
    }

    QByteArray ws_header = M2QT::getWebSocketHeader(response_data.size(), 0x01, 0x00);
    response_data.prepend(ws_header);

    NetString rep_id = std::make_tuple(static_cast<quint32>(id.size()), id);

    // build Response ...
    Response rep = std::make_tuple(uuid, rep_id, response_data);
    return rep;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response WebsocketPongHandler(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    QByteArray response_data;
    for (int i = 1; i<net_strings.size(); ++i)
    {
        response_data += std::get<NS_DATA>(net_strings[i]);
    }

    QByteArray ws_header = M2QT::getWebSocketHeader(response_data.size(), 0x0A, 0x00);
    response_data.prepend(ws_header);

    NetString rep_id = std::make_tuple(static_cast<quint32>(id.size()), id);

    // build Response ...
    Response rep = std::make_tuple(uuid, rep_id, response_data);
    return rep;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
#ifdef ENABLE_DEV_CALLBACKS
Response HeavyDutyHandler(const Request &in_message)
{
    Q_UNUSED(in_message)
    return Response();
}
#endif

// ----------------------------------------------------------------------------
//
// Filter callbacks ...
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebsocketHandshakeFilter(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    // get HEADER - first NetString must be the header obj ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("WebsocketHandshakeFilter - No header available!")); return false; }

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketHandshakeFilter - Couldn't find METHOD header!")); return false; }
    // must be "WEBSOCKET_HANDSHAKE" ...
    if (val.toString() != QLatin1String("WEBSOCKET_HANDSHAKE")) return false;

    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebsocketEchoFilter(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    // get HEADER - first NetString must be the header obj ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("WebsocketEchoFilter - No header available!")); return false; }

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketEchoFilter - Couldn't find METHOD header!")); return false; }
    // must be "WEBSOCKET" ...
    if (val.toString() != QLatin1String("WEBSOCKET")) return false;

    // get FLAGS ...
    val = jobj.value(QLatin1String("FLAGS"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketEchoFilter - Couldn't find FLAGS header!")); return false; }
    // req opcode must be 0x01 = text frame ...
    bool ok = false;
    quint8 value = val.toString().toInt(&ok, 16);
    if (ok == false || value == 0x0 || (value^(1<<7)) != 0x01) return false;

    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool WebsocketPongFilter(const Request &in_message)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_message;

    // get HEADER - first NetString must be the header obj ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("WebsocketPongFilter - No header available!")); return false; }

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketPongFilter - Couldn't find METHOD header!")); return false; }
    // must be "WEBSOCKET" ...
    if (val.toString() != QLatin1String("WEBSOCKET")) return false;

    // get FLAGS ...
    val = jobj.value(QLatin1String("FLAGS"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketPongFilter - Couldn't find FLAGS header!")); return false; }
    // req opcode must be 0x09 = ping frame ...
    bool ok = false;
    quint8 value = val.toString().toInt(&ok, 16);
    if (ok == false || value == 0x0 || (value^(1<<7)) != 0x09) return false;

    return true;
}

} // namespace


namespace M2Qt {
    const QString DEBUG_OUTPUT_NAME = "debug_output";
    const QString WS_HANDSHAKE_NAME = "websocket_handshake";
    const QString WS_ECHO_NAME      = "websocket_echo";
    const QString WS_PONG_NAME      = "websocket_pong";
    const QString HEAVY_DUTY_NAME   = "heavy_duty";
}

// ----------------------------------------------------------------------------
//
// class CallbackManager
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// handler callback names and filter callback names must correspond if they have to work together ...
// ----------------------------------------------------------------------------
QMap<QString, HandlerCallback> CallbackManager::m_handler_callback_map =
{
    { M2Qt::DEBUG_OUTPUT_NAME,  &DebugOutputHandler },
    { M2Qt::WS_HANDSHAKE_NAME,  &WebsocketHandshakeHandler },
    { M2Qt::WS_ECHO_NAME,       &WebsocketEchoHandler },
    { M2Qt::WS_PONG_NAME,       &WebsocketPongHandler },
#ifdef ENABLE_DEV_CALLBACKS
    { M2Qt::HEAVY_DUTY_NAME,    &HeavyDutyHandler },
#endif
};

QMap<QString, FilterCallback> CallbackManager::m_filter_callback_map =
{
    { M2Qt::WS_HANDSHAKE_NAME,  &WebsocketHandshakeFilter },
    { M2Qt::WS_ECHO_NAME,       &WebsocketEchoFilter },
    { M2Qt::WS_PONG_NAME,       &WebsocketPongFilter },
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
CallbackManager::CallbackManager(QObject *parent) : QObject(parent)
{
    m_p_static_cb_helper = helper;
    if (m_p_static_cb_helper != nullptr)
    {
        connect(m_p_static_cb_helper, &CallbackHelper::signalError, this, &CallbackManager::signalError);
        connect(m_p_static_cb_helper, &CallbackHelper::signalWarning, this, &CallbackManager::signalWarning);
        connect(m_p_static_cb_helper, &CallbackHelper::signalDebug, this, &CallbackManager::signalDebug);
        connect(m_p_static_cb_helper, &CallbackHelper::signalInfo, this, &CallbackManager::signalInfo);
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
HandlerCallback CallbackManager::getHandlerCallback(const QString &in_name)
{
    return m_handler_callback_map.value(in_name, nullptr);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
FilterCallback CallbackManager::getFilterCallback(const QString &in_name)
{
    return m_filter_callback_map.value(in_name, nullptr);
}
