#include <QCoreApplication>
#include <QDebug>

#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVariantMap params;
    // zmq pull socket address - messages from mongrel2 server ...
    params[QLatin1String("pull_addr")] = QByteArray("tcp://127.0.0.1:9999");
    // zmq pub socket address - messages to mongrel2 server ...
    params[QLatin1String("pub_addr")] = QByteArray("tcp://127.0.0.1:9998");
    // mongrel2 push socket id = server uuid in mongrel2 config ...
    params[QLatin1String("sender_id")] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");

    Controller controller;
    controller.init(params);

    QVariantMap handler_params;
    // debug output handler
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("debug_output");
    controller.createHandler(QLatin1String("debug_output"), handler_params);
    // websocket handshake handler
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("websocket_handshake");
    controller.createHandler(QLatin1String("websocket_handshake"), handler_params);
    // echo handler
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("echo");
    controller.createHandler(QLatin1String("echo"), handler_params);
    // pong handler
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("pong");
    controller.createHandler(QLatin1String("pong"), handler_params);

    controller.start();

    qDebug() << "\nstarting Main Event Loop ...\n";
    return a.exec();
}
