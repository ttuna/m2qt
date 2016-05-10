#include <QString>
#include <QWebSocket>
#include <QtTest>
#include <QtConcurrent>
#include <QThread>
#include <QCoreApplication>

#include <zmq.hpp>
#include "m2qt.h"

// ----------------------------------------------------------------------------
//
// class M2QtTest
//
// ----------------------------------------------------------------------------
class M2QtTest : public QObject
{
    Q_OBJECT

public:
    M2QtTest();

private Q_SLOTS:
    void testCase1();
};
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
M2QtTest::M2QtTest()
{
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void M2QtTest::testCase1()
{
    using namespace M2QT;
}

QTEST_APPLESS_MAIN(M2QtTest)

#include "tst_m2qt.moc"
