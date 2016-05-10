#include "m2qt_messageparser.h"
#include <QDebug>

using namespace M2QT;

MessageParser::MessageParser(QObject *parent) : QObject(parent)
{

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Message MessageParser::parse(const QByteArray &in_data) const   // TODO: make this static ???
{
    if (in_data.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Empty message!"));
        return Message();
    }

    const QList<QByteArray> token_list = in_data.split(' ');
    if (token_list.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Token list is empty!"));
        return Message();
    }
    else if (token_list.count() != 4)
    {
        emit signalError(QLatin1String("MessageParser::parse - Token list mismatch!"));
        return Message();
    }

    QString uuid(QLatin1String(token_list[0].data()));
    QString id(QLatin1String(token_list[1].data()));
    QString path(QLatin1String(token_list[2].data()));

    QList<NetString> net_strings = getNetStrings(token_list[3]);
    if (net_strings.size() < 1)
    {
        emit signalError(QLatin1String("MessageParser::parse - No header data available!"));
        return Message();
    }

    QJsonDocument jdoc = QJsonDocument::fromJson(std::get<1>(net_strings.takeFirst()));
    if (jdoc.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Could not parse header data!"));
        return Message();
    }

    QJsonObject jobj = jdoc.object();
    if (jobj.isEmpty())
    {
        emit signalError(QLatin1String("MessageParser::parse - Could not parse header data!"));
        return Message();
    }

    Message result = std::make_tuple(uuid, id, path, jobj, net_strings);
    emit signalResult(result);
    return result;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QList<NetString> MessageParser::getNetStrings(const QByteArray &in_data) const
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

