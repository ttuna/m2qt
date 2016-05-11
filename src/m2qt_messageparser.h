#ifndef M2QT_MESSAGEPARSER_H
#define M2QT_MESSAGEPARSER_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <tuple>

namespace M2QT {

// ----------------------------------------------------------------------------
//
// class MessageParser
//
// ----------------------------------------------------------------------------
class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(QObject *parent = 0);

    static QJsonObject getJson(const NetString &in_netstring);

signals:
    void signalError(QString error_msg) const;
    void signalResult(Request msg) const;

public slots:
    Request parse(const QByteArray &in_data) const;

private:
    static QList<NetString> getNetStrings(const QByteArray &in_data);

};

} // namespace

#endif // M2QT_MESSAGEPARSER_H
