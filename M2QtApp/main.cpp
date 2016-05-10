#include "m2qtapplication.h"
#include "m2qthandler.h"

int main(int argc, char *argv[])
{
    QVariantMap params;

    QCoreApplication a(argc, argv);
    //a.init(params);

    M2QtHandler handler;
    handler.init(params);
    handler.recv();

    handler.cleanup();

    //return a.exec();
    return 0;
}
