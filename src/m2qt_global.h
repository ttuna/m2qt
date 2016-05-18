#ifndef M2QT_GLOBAL_H
#define M2QT_GLOBAL_H

#include "m2qt.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariant>
#include <QVector>
#include <zmq.hpp>

namespace M2QT {

// ----------------------------------------------------------------------------
// getHeader
// ----------------------------------------------------------------------------
static QJsonObject getHeader(const QVector<NetString> &in_netstrings)
{
    if (in_netstrings.isEmpty()) return QJsonObject();
    NetString header = in_netstrings[0];

    quint32 size = std::get<NS_SIZE>(header);
    if (size == 0) return QJsonObject();

    QByteArray data = std::get<NS_DATA>(header);
    if (data.isEmpty()) return QJsonObject();

    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    if (jdoc.isEmpty()) return QJsonObject();

    QJsonObject jobj = jdoc.object();
    return jobj;
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
    QByteArray uuid, id_and_len, data;
    std::tie(uuid, id_and_len, data) = in_msg;
    QVariant rep_data = uuid + ' ' + id_and_len + ',' + ' ' + data;
    if (rep_data.canConvert<T>() == true)
        return rep_data.value<T>();
    else
        return T();
}

// ----------------------------------------------------------------------------
// to<zmq::message_t>
// ----------------------------------------------------------------------------
template <>
static zmq::message_t to<zmq::message_t>(const Response &in_msg)
{
    QByteArray uuid, id_and_len, data;
    std::tie(uuid, id_and_len, data) = in_msg;
    QByteArray rep_data = uuid + ' ' + id_and_len + ',' + ' ' + data;
    zmq::message_t msg(rep_data.data(), rep_data.size());
    return msg;
}

} // namespace

#endif // M2QT_GLOBAL_H
