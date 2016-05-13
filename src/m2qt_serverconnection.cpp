﻿#include "m2qt_serverconnection.h"
#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
// ctor / dtor / ...
// ----------------------------------------------------------------------------
ServerConnection::ServerConnection(QObject *in_parent) : QObject(in_parent)
{
}

ServerConnection::~ServerConnection()
{
    if (m_p_pub_sock != nullptr)
    {
        int time = 0;
        m_p_pub_sock->setsockopt(ZMQ_LINGER, &time, sizeof(time));
        delete m_p_pub_sock;
        m_p_pub_sock = nullptr;
    }
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool ServerConnection::init(zmq::context_t* in_zmq_ctx, const QVariantMap& in_params)
{
    qDebug() << "ServerConnection::init";
    if (in_zmq_ctx == nullptr) return false;

    if (update(in_params) == false) return false;
    m_p_zmq_ctx = in_zmq_ctx;

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool ServerConnection::update(const QVariantMap &in_params)
{
    if (in_params.contains("pull_addr"))
    {
        m_pull_addr = in_params["pull_addr"].toByteArray();
        qDebug() << "ServerConnection::update - pull_addr:" << m_pull_addr;
    }
    if (in_params.contains(("pub_addr")))
    {
        m_pub_addr = in_params["pub_addr"].toByteArray();
        qDebug() << "ServerConnection::update - pub_addr:" << m_pub_addr;
    }
    if (in_params.contains(("sender_id")))
    {
        m_sender_id = in_params["sender_id"].toByteArray();
        qDebug() << "ServerConnection::update - sender_id:" << m_sender_id;
    }

    if (m_pull_addr.isEmpty()) return false;
    if (m_pub_addr.isEmpty()) return false;
    if (m_sender_id.isEmpty()) return false;

    return true;
}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool ServerConnection::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// receive - loop ...
// ----------------------------------------------------------------------------
void ServerConnection::poll()
{
    qDebug() << "ServerConnection::poll";
    if (m_initialized == false)
    {
        emit signalError(QLatin1String("ServerConnection::poll - ServerConnection not initialized!"));
        return;
    }

    // init connections ...
    qDebug() << "ServerConnection::poll - pull addr:" << m_pull_addr.data();
    zmq::socket_t* pull_sock = new zmq::socket_t(*m_p_zmq_ctx, ZMQ_PULL);
    pull_sock->connect(m_pull_addr.data());

    // init poller ...
    zmq::message_t msg;
    zmq::pollitem_t items [] = {
        { *pull_sock, 0, ZMQ_POLLIN, 0 }
    };

    // set running-flag and ... BANZAIIII!!!
    m_running.testAndSetRelaxed(0, 1);
    qDebug() << "ServerConnection::poll -" << ((m_running.load() == 1) ? "starting ..." : "error ...");

    while (m_running.load()) {
        zmq::poll(&items[0], 1, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            pull_sock->recv(&msg);

            QByteArray in_msg(msg.data<char>(), msg.size());
            if (in_msg.isEmpty() == false)
            {
                qDebug() << "ServerConnection::poll - Received:" << in_msg << endl;
                emit signalNewMessage(in_msg);
            }
            else
            {
                emit signalError(QLatin1String("ServerConnection::poll - Empty message! Skip ..."));
            }
        }
    }

    // cleanup ...
    int time = 0;
    if (pull_sock)
    {
        pull_sock->setsockopt(ZMQ_LINGER, &time, sizeof(time));
        delete pull_sock;
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void ServerConnection::stop()
{
    qDebug() << "ServerConnection::stop";
    if (m_initialized == false)
    {
        emit signalError(QLatin1String("ServerConnection::stop - ServerConnection not initialized!"));
        return;
    }

    // reset running-flag ...
    m_running.testAndSetRelaxed(1, 0);
    qDebug() << "ServerConnection::stop() -:" << ((m_running.load() == 0) ? "stopping ..." : "error ...");
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void ServerConnection::send(const Response &in_rep)
{
    qDebug() << "ServerConnection::send";
    if (m_initialized == false)
    {
        emit signalError(QLatin1String("ServerConnection::send - ServerConnection not initialized!"));
        return;
    }

    if (m_p_pub_sock == nullptr)
    {
        m_p_pub_sock = new zmq::socket_t(*m_p_zmq_ctx, ZMQ_PUB);
        if (m_p_pub_sock == nullptr) return;

        m_p_pub_sock->connect(m_pub_addr.data());
    }

    QString msg_str = to<QString>(in_rep);
    qDebug() << "ServerConnection::send - Sending:\n" << msg_str;
    zmq::message_t msg = to<zmq::message_t>(in_rep);
    bool ret = m_p_pub_sock->send(msg);
    qDebug() << "ServerConnection::send - zmq ret:" << ret << endl << endl;
}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
QByteArray ServerConnection::pubAddr() const
{
    return m_pub_addr;
}

QByteArray ServerConnection::pullAddr() const
{
    return m_pull_addr;
}

QByteArray ServerConnection::senderId() const
{
    return m_sender_id;
}

void ServerConnection::setPubAddr(QByteArray pub_addr)
{
    if (m_pub_addr == pub_addr)
        return;

    m_pub_addr = pub_addr;
    emit signalPubAddrChanged(pub_addr);
}

void ServerConnection::setPullAddr(QByteArray pull_addr)
{
    if (m_pull_addr == pull_addr)
        return;

    m_pull_addr = pull_addr;
    emit signalPullAddrChanged(pull_addr);
}

void ServerConnection::setSenderId(QByteArray sender_id)
{
    if (m_sender_id == sender_id)
        return;

    m_sender_id = sender_id;
    emit signalSenderIdChanged(sender_id);
}
