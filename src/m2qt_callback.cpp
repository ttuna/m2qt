#include "m2qt_callback.h"
#include "m2qt_messageparser.h"

#include <QCryptographicHash>
#include <QJsonObject>
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

    qDebug() << "\n-------------------------";
    qDebug() << "DebugOutput:\n";
    qDebug() << "\tuuid:" << uuid;
    qDebug() << "\tid:" << id;
    qDebug() << "\tpath:" << path;
    qDebug() << "\tnetstring entries:" << net_strings.count();

//    foreach(NetString ns, net_strings)
//        qDebug() << "\t\t" << std::get<NetStringIdx::SIZE>(ns) << std::get<NetStringIdx::DATA>(ns);

    qDebug() << "\n\tjson header:";
    QJsonObject jobj = M2QT::getJson(net_strings[0]);
    QJsonObject::const_iterator iter = jobj.constBegin();
    for ( ;iter!=jobj.constEnd(); ++iter )
    {
        qDebug() << "\t\t" << left << iter.key() << ":" << iter.value();
    }
    qDebug() << "-------------------------\n";

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
    QJsonObject jobj = M2QT::getJson(net_strings[0]);
    if (jobj.isEmpty()) return Response();

    // get METHOD ...
    QJsonValue val = jobj.value(QLatin1String("METHOD"));
    if (val.isUndefined())
    {
        emit helper->signalError("Couldn't find METHOD header ...");
        return Response();
    }
    // must be "WEBSOCKET_HANDSHAKE" ...
    if (val.toString() != QLatin1String("WEBSOCKET_HANDSHAKE"))
    {
        emit helper->signalError("METHOD != WEBSOCKET_HANDSHAKE ...");
        return Response();
    }

    // get SEC-WEBSOCKET-KEY ...
    val = jobj.value("sec-websocket-key");
    if (val.isUndefined())
    {
        emit helper->signalError("Couldn't find SEC-WEBSOCKET-KEY header ...");
        return Response();
    }

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
    return Response();
}

} // namespace


// ----------------------------------------------------------------------------
//
// class DefaultCallbacks
//
// ----------------------------------------------------------------------------
QMap<QString, HandlerCallback> DefaultCallbacks::m_callback_map =
{
    { "debug_output", &DebugOutput },
    { "websocket_handshake", &WebsocketHandshake },
    { "echo", &Echo },
};

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
DefaultCallbacks::DefaultCallbacks(QObject *parent) : QObject(parent)
{
    m_p_static_cb_helper = helper;
    if (m_p_static_cb_helper != nullptr)
    {
        connect(m_p_static_cb_helper, &CallbackHelper::signalError, this, &DefaultCallbacks::signalError);
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QVector<HandlerCallback> DefaultCallbacks::getCallbacks(const QStringList &in_names)
{
    QVector<HandlerCallback> ret;

    foreach (QString name, in_names)
    {
        if (m_callback_map.contains(name) == false) continue;
        ret.push_back(m_callback_map.value(name));
    }

    return ret;
}
