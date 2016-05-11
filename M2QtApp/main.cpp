﻿#include <QCoreApplication>
#include "controller.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList default_callbacks;
    default_callbacks << "debug_output" << "websocket_handshake";

    QVariantMap params;
    // zmq pull socket address - messages from mongrel2 server ...
    params["pull_addr"] = QByteArray("tcp://127.0.0.1:9999");
    // zmq pub socket address - messages to mongrel2 server ...
    params["pub_addr"] = QByteArray("tcp://127.0.0.1:9998");
    // mongrel2 push socket id = server uuid in mongrel2 config ...
    params["sender_id"] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");
    // default callbacks
    params["default_callbacks"] = default_callbacks;

    Controller controller;
    QString handler_name(QLatin1String("web_socket_handler"));
    controller.init(params);
    controller.createHandler(handler_name, params);
    controller.startHandler(handler_name);

    qDebug() << "\nstarting Main Event Loop ...\n";
    return a.exec();
}
