#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include "m2qt_global.h"
#include "zmq.hpp"
#include <QObject>
#include <QVariant>

namespace M2QT {

// ----------------------------------------------------------------------------
//
// class ServerConnection
//
// ----------------------------------------------------------------------------
class ServerConnection final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray pub_addr READ pubAddr WRITE setPubAddr NOTIFY pubAddrChanged)
    Q_PROPERTY(QByteArray pull_addr READ pullAddr WRITE setPullAddr NOTIFY pullAddrChanged)
    Q_PROPERTY(QByteArray sender_id READ senderId WRITE setSenderId NOTIFY senderIdChanged)
public:
    explicit ServerConnection(QObject* in_parent = nullptr);
    ~ServerConnection();

    bool init(zmq::context_t* in_zmq_ctx, const QVariantMap &in_params);
    bool update(const QVariantMap &in_params);
    bool isValid() const;

    QByteArray pubAddr() const;
    QByteArray pullAddr() const;
    QByteArray senderId() const;

public slots:
    void receive();
    void setPubAddr(QByteArray pub_addr);
    void setPullAddr(QByteArray pull_addr);
    void setSenderId(QByteArray sender_id);

signals:
    void newMessage(QByteArray data);
    void pubAddrChanged(QByteArray pub_addr);
    void pullAddrChanged(QByteArray pull_addr);
    void senderIdChanged(QByteArray sender_id);

private:
    bool m_initialized = false;
    QByteArray m_pub_addr;
    QByteArray m_pull_addr;
    QByteArray m_sender_id;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    zmq::socket_t* m_p_pull_sock = nullptr;
    zmq::socket_t* m_p_pub_sock = nullptr;
};

} // namespace M2QT

#endif // SERVERCONNECTION_H
