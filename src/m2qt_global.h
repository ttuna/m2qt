#ifndef M2QT_GLOBAL_H
#define M2QT_GLOBAL_H

#include "m2qt.h"
#include <QByteArray>
#include <zmq.hpp>

namespace M2QT {

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
// join
// ----------------------------------------------------------------------------
static zmq::message_t toZmqMessage(const Response &in_msg)
{
    QByteArray uuid, id_and_len, data;
    std::tie(uuid, id_and_len, data) = in_msg;
    QByteArray rep_data = uuid + ' ' + id_and_len + ',' + ' ' + data;
    zmq::message_t msg(rep_data.data(), rep_data.size());
    return msg;
}

} // namespace

#endif // M2QT_GLOBAL_H
