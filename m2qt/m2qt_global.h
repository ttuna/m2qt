#ifndef M2QT_GLOBAL_H
#define M2QT_GLOBAL_H

#include "m2qt.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>
#include <QByteArray>
#include <QVariant>
#include <QVector>
#include <QDebug>
#include <zmq.hpp>

#include <string>
#include <sstream>

namespace M2QT {

// ----------------------------------------------------------------------------
// getWebSocketHeader - see https://tools.ietf.org/html/rfc6455 for more information ...
// ----------------------------------------------------------------------------
static QByteArray getWebSocketHeader(const quint64& in_data_size, const quint8 in_opcode, const quint8 in_rsv)
{
    QByteArray header;
    QDataStream out(&header, QIODevice::WriteOnly);

    // FIN (= 1), RSV1 - RSV3 (MUST be 0 unless negotiated otherwise), OPCODE (should be 0x1 for text msg) ...
    out << static_cast<quint8>(1<<7|in_opcode|in_rsv<<4);

    quint8 payload_length = 0;
    quint64 mask = 0;
    if (in_data_size <= 125)
    {
        //data size <= 125 -> payload_length = data size
        payload_length = in_data_size;
        mask = 0x0;
    }
    else if (in_data_size < 1<<16)
    {
        //125 < data size < 65536 -> payload_length = 126 (extended payload length = 2 bytes)
        payload_length = 126;
        mask = 0xFFFF;
    }
    else
    {
        //data size > 65536 -> payload_length = 127 (extended payload length = 8 bytes)
        payload_length = 127;
        mask = 0xFFFFFFFFFFFFFFFF;
    }

    // MASK (= 0, msb of payload_length), PAYLOAD LEN ...
    out << payload_length;

    // extended payload length ...
    switch(mask)
    {
        case 0x0:
        break;
        case 0xFFFF:
            out << static_cast<quint16>(in_data_size & mask);
        break;
        case 0xFFFFFFFFFFFFFFFF:
            out << static_cast<quint64>(in_data_size & mask);
        break;
    }

    return header;
}

// ----------------------------------------------------------------------------
// getHTTPHeader - see https://tools.ietf.org/html/rfc2616 for more information ...
// ----------------------------------------------------------------------------
static QByteArray getHTTPHeader(const QVector<NetString> &in_headers, const quint32 &in_status_code, const QByteArray &in_status)
{
    // TODO ...
    Q_UNUSED(in_headers)
    Q_UNUSED(in_status_code)
    Q_UNUSED(in_status)

    return QByteArray();
}

// ----------------------------------------------------------------------------
// getJsonData
// ----------------------------------------------------------------------------
static QJsonObject netstring2Json(const NetString &in_netstring, QByteArray& out_prefix = default_param)
{
    quint32 size = std::get<NS_SIZE>(in_netstring);
    if (size == 0) return QJsonObject();

    QByteArray raw_data = std::get<NS_DATA>(in_netstring);
    if (raw_data.isEmpty()) return QJsonObject();

    // Mongrel2 uses a special format for json data msg:
    // '@route { somejsondata } \0'
    //      route           = defined route in mongrel config
    //      somejsondata    = json data (e.g. "key1":"value1", "key2":"value2")
    QByteArray data;
    if (raw_data.startsWith('@'))
    {
        int b_open = raw_data.indexOf('{');
        int b_close = raw_data.lastIndexOf('}');
        if (b_open == -1 || b_close == -1 || b_open > b_close) return QJsonObject();
        data = raw_data.mid(b_open, b_close-b_open+1);
        out_prefix = raw_data.left(b_open);
    }
    else
    {
        // assume that NetString data is in valid json format ...
        data = raw_data;
    }

    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    if (jdoc.isEmpty()) return QJsonObject();

    QJsonObject jobj = jdoc.object();
    return jobj;
}

// ----------------------------------------------------------------------------
// getJsonHeader
// ----------------------------------------------------------------------------
static QJsonObject getJsonHeader(const QVector<NetString> &in_netstrings)
{
    if (in_netstrings.isEmpty()) return QJsonObject();

    // first NetString must be the header ...
    NetString header = in_netstrings[0];
    return netstring2Json(header);
}

// ----------------------------------------------------------------------------
// isReqEmpty
// ----------------------------------------------------------------------------
static bool isReqEmpty(const Request &in_msg)
{
    return (in_msg == Request());
}

// ----------------------------------------------------------------------------
// isRepEmpty
// ----------------------------------------------------------------------------
static bool isRepEmpty(const Response &in_msg)
{
    return (in_msg == Response());
}

// ----------------------------------------------------------------------------
// to<T>
// ----------------------------------------------------------------------------
template <typename T>
static T to(const Response &in_msg)
{
    NetString id;
    quint32 id_len;
    QByteArray uuid, data, id_data;
    std::tie(uuid, id, data) = in_msg;
    std::tie(id_len, id_data) = id;

    QVariant rep_data;
    QByteArray buffer = uuid + ' ' + QByteArray::number(id_len) + ':' + id_data + ',' + ' ' + data;
    if (rep_data.canConvert<T>() == true)
        return rep_data.value<T>();
    else
        return T();
}

// ----------------------------------------------------------------------------
// to<zmq::message_t>
// ----------------------------------------------------------------------------
template <>
zmq::message_t to<zmq::message_t>(const Response &in_msg)
{
    NetString id;
    quint32 id_len;
    QByteArray uuid, data, id_data;
    std::tie(uuid, id, data) = in_msg;
    std::tie(id_len, id_data) = id;

    QByteArray rep_data = uuid + ' ' + QByteArray::number(id_len) + ':' + id_data + ',' + ' ' + data;
    zmq::message_t msg(rep_data.data(), rep_data.size());
    return msg;
}

} // namespace

#endif // M2QT_GLOBAL_H
