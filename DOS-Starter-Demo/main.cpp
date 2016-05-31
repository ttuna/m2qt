#include <QCoreApplication>
#include <QDebug>

#include "controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVariantMap params;
    // zmq pull socket address - messages from mongrel2 server ...
    params[QLatin1String("pull_addr")] = QByteArray("tcp://127.0.0.1:9997");
    // zmq pub socket address - messages to mongrel2 server ...
    params[QLatin1String("pub_addr")] = QByteArray("tcp://127.0.0.1:9996");
    // mongrel2 push socket id = server uuid in mongrel2 config ...
    params[QLatin1String("sender_id")] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");
    // starter app dir ...
    params[QLatin1String("app_dir")] = QVariant::fromValue(QDir(QLatin1String("E:/Program Files (x86)/AVL/AVLDRIVE_V4.0.0/Program")));
    // starter app file name ...
    params[QLatin1String("app_file")] = QLatin1String("AVLDrive.exe");

    Controller controller;
    controller.init(params);
    controller.start();

    qDebug() << "\nstarting Main Event Loop ...\n";

    return a.exec();
}
