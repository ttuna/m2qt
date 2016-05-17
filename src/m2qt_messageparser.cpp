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
    if (in_data.isEmpty()) { emit signalError(QLatin1String("MessageParser::parse - Empty message!")); return Request(); }

    char separator = ' ';
    QByteArrayList token_list = in_data.split(separator);
    if (token_list.isEmpty()) { emit signalError(QLatin1String("MessageParser::parse - Token list is empty!")); return Request(); }
    else if (token_list.count() < 4) { emit signalError(QLatin1String("MessageParser::parse - Token list mismatch!")); return Request(); }

    QByteArray uuid(token_list.takeFirst());
    QByteArray id(token_list.takeFirst());
    QByteArray path(token_list.takeFirst());

    QVector<NetString> net_strings = getNetStrings(token_list.join(separator));
    if (net_strings.size() < 1) { emit signalError(QLatin1String("MessageParser::parse - No message data available!")); return Request(); }

    Request result = std::make_tuple(uuid, id, path, net_strings);
    emit signalResult(result);
    return result;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QVector<NetString> MessageParser::getNetStrings(const QByteArray &in_data)
{
    if (in_data.isEmpty()) return QVector<NetString>();

    // TODO: find some reasonable checks to verify that in_data is valid (e.g. contains some NetStrings) ...

    // NetString: <length>:<data> ...
    QVector<NetString> net_strings;
    net_strings.reserve(10); // meaningful? and if so what's a good number ... ?!?

    int pos = 0, sep = 0, len = 0;
    bool ok = false;
    QByteArray left, right;
    while (true)
    {
        sep = in_data.indexOf(':', pos);
        if (sep < 0) return QVector<NetString>();

        left = in_data.mid(pos, sep-pos);
        len = left.toInt(&ok);
        if (ok == false) return QVector<NetString>();

        right = in_data.mid(sep+1, len);
        if (right.isEmpty()) return QVector<NetString>();

        net_strings.push_back(std::make_tuple(len, right));
        pos += (sep+1 + right.length() + 1); // +1 to step over the ',' seperator ...

        if (pos >= in_data.length()) break;
    }

    return net_strings;
}
