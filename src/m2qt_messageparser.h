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

class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(QObject *parent = 0);

signals:
    void signalError(QString error_msg) const;
    void signalResult(Message msg) const;

public slots:
    Message parse(const QByteArray &in_data) const;
    QList<NetString> getNetStrings(const QByteArray &in_data) const;
};

} // namespace

#endif // M2QT_MESSAGEPARSER_H
