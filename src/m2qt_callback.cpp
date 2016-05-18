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
// Callbacks ...
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response DebugOutput(const Request &in_message)
{
    QByteArray uuid = std::get<REQ_UUID>(in_message);
    QByteArray id = std::get<REQ_ID>(in_message);
    QByteArray path = std::get<REQ_PATH>(in_message);
    QVector<NetString> net_strings = std::get<REQ_NETSTRINGS>(in_message);

    QByteArray debug_msg;
    QTextStream debug_stream(&debug_msg);
    debug_stream << "\n-------------------------" << endl;
    debug_stream << "DebugOutput:\n" << endl;
    debug_stream << "\tuuid:" << uuid << endl;
    debug_stream << "\tid:" << id << endl;
    debug_stream << "\tpath:" << path << endl;
    debug_stream << "\tnetstring entries:" << net_strings.count() << endl;

    debug_stream << "\n\tJson header:" << endl;
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    QJsonObject::const_iterator iter = jobj.constBegin();
    for ( ;iter!=jobj.constEnd(); ++iter )
    {
        debug_stream << "\t\t" << left << iter.key() << ":" << iter.value().toString() << endl;
    }

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
Response WebsocketHandshake(const Request &in_message)
{
    QByteArray uuid = std::get<REQ_UUID>(in_message);
    QByteArray id = std::get<REQ_ID>(in_message);
    QByteArray path = std::get<REQ_PATH>(in_message);
    QVector<NetString> net_strings = std::get<REQ_NETSTRINGS>(in_message);

    // get HEADER - first NetString must be the header obj ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("WebsocketHandshake - No header available!")); return Response(); }

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketHandshake - Couldn't find METHOD header!")); return Response(); }

    // must be "WEBSOCKET_HANDSHAKE" ...
    if (val.toString() != QLatin1String("WEBSOCKET_HANDSHAKE")) return Response();

    // get SEC-WEBSOCKET-KEY ...
    val = jobj.value("sec-websocket-key");
    if (val.isUndefined()) { emit helper->signalError(QStringLiteral("WebsocketHandshake - Couldn't find SEC-WEBSOCKET-KEY header!")); return Response(); }

    // TEST ...
    // TODO: put this into unit test ...
    //QByteArray test("dGhlIHNhbXBsZSBub25jZQ=="); // accpet == s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
    //QByteArray key = test + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    // calc SEC-WEBSOCKET-ACCEPT ...
    QByteArray key = val.toString().toLatin1() + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    QByteArray accept = QCryptographicHash::hash(key, QCryptographicHash::Sha1).toBase64();

    // build response body ...
    QByteArray response_data(QLatin1String("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: upgrade\r\nSec-WebSocket-Accept: ").data());
    response_data += accept + "\r\n\r\n";

    // build len(id):len ...
    QByteArray id_and_len = QByteArray::number(id.size()) + ':' + id;

    // build Response ...
    Response rep = std::make_tuple(uuid, id_and_len, response_data);
    return rep;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Response Echo(const Request &in_message)
{
    QByteArray uuid = std::get<REQ_UUID>(in_message);
    QByteArray id = std::get<REQ_ID>(in_message);
    QByteArray path = std::get<REQ_PATH>(in_message);
    QVector<NetString> net_strings = std::get<REQ_NETSTRINGS>(in_message);

    // get HEADER - first NetString must be the header obj ...
    QJsonObject jobj = M2QT::getJsonHeader(net_strings);
    if (jobj.isEmpty()) { emit helper->signalError(QStringLiteral("Echo - No header available!")); return Response(); }

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined()){ emit helper->signalError(QStringLiteral("Echo - Couldn't find METHOD header!")); return Response(); }

    // must be "WEBSOCKET" ...
    if (val.toString() != QLatin1String("WEBSOCKET")) return Response();

    QByteArray response_data;
    for (int i = 0; i<net_strings.size(); ++i)
    {
        response_data += std::get<NS_DATA>(net_strings[i]);
    }

    QByteArray header = M2QT::getWebSocketHeader(response_data.size(), 0x01, 0x00);

    response_data.prepend(header);

//    std::string test_header_1 = M2QT::websocket_header(65537, 0x01, 0x00);
//    qDebug() << "TEST1:" << QByteArray::fromStdString(test_header_1);

//    QByteArray test_data(65537, 'x');
//    QByteArray test_header_2 = M2QT::getWebSocketHeader(test_data.size(), 0x01, 0x0);
//    qDebug() << "TEST2:" << test_header_2;

    // build len(id):len ...
    QByteArray id_and_len = QByteArray::number(id.size()) + ':' + id;

    // build Response ...
    Response rep = std::make_tuple(uuid, id_and_len, response_data);
    return rep;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
#ifdef ENABLE_DEV_CALLBACKS
Response HeavyDuty(const Request &in_message)
{
    return Response();
}
#endif
} // namespace


// ----------------------------------------------------------------------------
//
// class DefaultCallbacks
//
// ----------------------------------------------------------------------------
QMap<QString, HandlerCallback> DefaultCallbackManager::m_callback_map =
{
    { "debug_output", &DebugOutput },
    { "websocket_handshake", &WebsocketHandshake },
    { "echo", &Echo },
#ifdef ENABLE_DEV_CALLBACKS
    { "heavy_duty", &HeavyDuty },
#endif
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
DefaultCallbackManager::DefaultCallbackManager(QObject *parent) : QObject(parent)
{
    m_p_static_cb_helper = helper;
    if (m_p_static_cb_helper != nullptr)
    {
        connect(m_p_static_cb_helper, &CallbackHelper::signalError, this, &DefaultCallbackManager::signalError);
        connect(m_p_static_cb_helper, &CallbackHelper::signalWarning, this, &DefaultCallbackManager::signalWarning);
        connect(m_p_static_cb_helper, &CallbackHelper::signalDebug, this, &DefaultCallbackManager::signalDebug);
        connect(m_p_static_cb_helper, &CallbackHelper::signalInfo, this, &DefaultCallbackManager::signalInfo);
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
HandlerCallback DefaultCallbackManager::getCallback(const QString &in_name)
{
    return m_callback_map.value(in_name, HandlerCallback());
}
