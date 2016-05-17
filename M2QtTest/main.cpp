
#define QT_NO_SSL

#include <QCoreApplication>

#include "websockethelper.h"

const QString msg_prefix = "@ws_msg";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifndef homeoffice
    QUrl url("ws://157.247.245.36:6767/websocket/");
#else
    QUrl url("ws://192.168.0.13:6767/websocket/");
#endif

    QVariantMap params;
    // template for json messages
    params["msg_template"] = QString("@ws_msg {\"type\":\"%1\", \"data\":\"%2\"} \0");

    WebSocketHelper web_socket_helper;
    if (web_socket_helper.init(params) == false) return -1;
    web_socket_helper.connect(url);

    return a.exec();
}
