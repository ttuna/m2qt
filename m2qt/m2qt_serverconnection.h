﻿#ifndef M2QT_SERVERCONNECTION_H
#define M2QT_SERVERCONNECTION_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QVariant>

#include <zmq.hpp>

namespace M2QT {

// ----------------------------------------------------------------------------
//
// class ServerConnection
//
// ----------------------------------------------------------------------------
class ServerConnection final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray pub_addr READ pubAddr WRITE setPubAddr NOTIFY signalPubAddrChanged)
    Q_PROPERTY(QByteArray pull_addr READ pullAddr WRITE setPullAddr NOTIFY signalPullAddrChanged)
    Q_PROPERTY(QByteArray sender_id READ senderId WRITE setSenderId NOTIFY signalSenderIdChanged)
public:
    explicit ServerConnection(QObject* in_parent = nullptr);
    ~ServerConnection();

    bool init(zmq::context_t* in_zmq_ctx, const QVariantMap &in_params);
    void cleanup();
    bool update(const QVariantMap &in_params);
    bool isValid() const;

    // properties ...
    QByteArray pubAddr() const;
    QByteArray pullAddr() const;
    QByteArray senderId() const;

public slots:
    void poll();
    void stop();
    void send(const QByteArray &in_data);

    // properties ...
    void setPubAddr(QByteArray pub_addr);
    void setPullAddr(QByteArray pull_addr);
    void setSenderId(QByteArray sender_id);

signals:
    void signalError(QString error);    // TODO: use it ...
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

    void signalStartPolling();
    void signalStopPolling();
    void signalNewMessage(QByteArray data);

    // properties ...
    void signalPubAddrChanged(QByteArray pub_addr);
    void signalPullAddrChanged(QByteArray pull_addr);
    void signalSenderIdChanged(QByteArray sender_id);

private:
    bool m_initialized = false;
    QByteArray m_pub_addr;
    QByteArray m_pull_addr;
    QByteArray m_sender_id;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    zmq::socket_t* m_p_pub_sock = nullptr;
//    zmq::socket_t* m_p_pull_sock = nullptr;
    QAtomicInt m_running = 0;
};

} // namespace M2QT

#endif // M2QT_SERVERCONNECTION_H
