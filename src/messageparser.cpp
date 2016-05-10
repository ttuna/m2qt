#include "messageparser.h"

MessageParser::MessageParser(QObject *parent) : QObject(parent)
{

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
QVector<QString> MessageParser::parse(const QByteArray &in_data)
{
    if (in_data.isEmpty()) return QVector<QString>();
}

