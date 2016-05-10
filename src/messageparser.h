#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include <QVector>
#include <QByteArray>

class MessageParser : public QObject
{
    Q_OBJECT
public:
    explicit MessageParser(QObject *parent = 0);

signals:

public slots:
    QVector<QString> parse(const QByteArray &in_data);
};

#endif // MESSAGEPARSER_H
