#include "m2qt_messageparser.h"
#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
MessageParser::MessageParser(QObject *parent) : QObject(parent)
{
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Request MessageParser::parse(const QByteArray &in_data) const   // TODO: make this static ???
{
    if (in_data.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Empty message!"));
        return Request();
    }

    const QList<QByteArray> token_list = in_data.split(' ');
    if (token_list.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Token list is empty!"));
        return Request();
    }
    else if (token_list.count() != 4)
    {
        emit signalError(QLatin1String("MessageParser::parse - Token list mismatch!"));
        return Request();
    }

    QByteArray uuid(token_list[0]);
    QByteArray id(token_list[1]);
    QByteArray path(token_list[2]);

    QList<NetString> net_strings = getNetStrings(token_list[3]);
    if (net_strings.size() < 1)
    {
        emit signalError(QLatin1String("MessageParser::parse - No message data available!"));
        return Request();
    }

    Request result = std::make_tuple(uuid, id, path, net_strings);
    emit signalResult(result);
    return result;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QList<NetString> MessageParser::getNetStrings(const QByteArray &in_data)
{
    if (in_data.isEmpty()) return QList<NetString>();

    // NetString: <length>:<data> ...
    QList<NetString> net_strings;
    net_strings.reserve(10); // meaningful? and if so what's a good number ... ?!?

    int pos = 0;
    int sep = 0;
    int len = 0;
    bool ok = false;
    QByteArray left, right;
    while (true)
    {
        sep = in_data.indexOf(':', pos);
        if (sep < 0) return QList<NetString>();

        left = in_data.mid(pos, sep-pos);
        len = left.toInt(&ok);
        if (ok == false) return QList<NetString>();

        right = in_data.mid(sep+1, len);
        if (right.isEmpty()) return QList<NetString>();

        net_strings.append(std::make_tuple(len, right));
        pos += (sep+1 + right.length() + 1); // +1 to step over the ',' seperator ...

        if (pos >= in_data.length()) break;
    }

    return net_strings;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QJsonObject MessageParser::getJson(const NetString &in_netstring)
{
    quint32 size = std::get<NetStringIdx::SIZE>(in_netstring);
    if (size == 0) return QJsonObject();
    QByteArray data = std::get<NetStringIdx::DATA>(in_netstring);
    if (data.isEmpty()) return QJsonObject();

    QJsonDocument jdoc = QJsonDocument::fromJson(data);
    if (jdoc.isEmpty()) return QJsonObject();

    QJsonObject jobj = jdoc.object();
    return jobj;
}
