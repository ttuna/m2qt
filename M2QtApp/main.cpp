#include <QCoreApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVariantMap params;
    params["pull_addr"] = QByteArray("tcp://127.0.0.1:9999");
    params["pub_addr"] = QByteArray("tcp://127.0.0.1:9998");
    params["sender_id"] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");

    Controller controller;
    controller.init(params);

    return a.exec();
}
