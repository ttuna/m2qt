#include "controller.h"
#include <m2qt.h>
#include <QDebug>

using namespace M2QT;

Controller::Controller(QObject *parent) : QObject(parent)
{
    QVariantMap params;
    params["pull_addr"] = QByteArray("tcp://127.0.0.1:9999");
    params["pub_addr"] = QByteArray("tcp://127.0.0.1:9998");
    params["sender_id"] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");

    IM2Qt& m2qt = M2QtCreator::getM2Qt(params);
    //if (m2qt.isValid() == false) return false;

    IM2QtHandler* web_socket_handler = m2qt.createM2QtHandler("web_socket_handler", params);
    if (web_socket_handler == nullptr) qDebug() << "handler == nullptr";
}
