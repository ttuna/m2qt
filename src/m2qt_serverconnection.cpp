#include "m2qt_serverconnection.h"
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
    int time = 0;
    if (m_p_pull_sock)
    {
        m_p_pull_sock->setsockopt(ZMQ_LINGER, &time, sizeof(time));
        delete m_p_pull_sock;
    }
    if (m_p_pub_sock)
    {
        m_p_pub_sock->setsockopt(ZMQ_LINGER, &time, sizeof(time));
        delete m_p_pub_sock;
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
    if (m_pull_addr.isEmpty()) return false;
    if (m_pub_addr.isEmpty()) return false;
    if (m_sender_id.isEmpty()) return false;

    QScopedPointer<zmq::socket_t> tmp_pull_sock(new zmq::socket_t(*in_zmq_ctx, ZMQ_PULL));
    if (tmp_pull_sock.isNull()) return false;
    QScopedPointer<zmq::socket_t> tmp_pub_sock(new zmq::socket_t(*in_zmq_ctx, ZMQ_PUB));
    if (tmp_pub_sock.isNull()) return false;

    m_p_pull_sock = tmp_pull_sock.take();
    m_p_pub_sock = tmp_pub_sock.take();
    m_p_pub_sock->setsockopt(ZMQ_IDENTITY, m_sender_id.data(), m_sender_id.length());

    m_p_pull_sock->connect(m_pull_addr.data());
    m_p_pub_sock->connect(m_pub_addr.data());

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
void ServerConnection::receive()
{
    qDebug() << "ServerConnection::receive";

    zmq::pollitem_t items [] = {
        { *m_p_pull_sock, 0, ZMQ_POLLIN, 0 }
    };

    zmq::message_t msg;
    while (1) {
        zmq::poll(&items[0], 1, -1);

        if (items[0].revents & ZMQ_POLLIN)
        {
            m_p_pull_sock->recv(&msg);

            QByteArray in_msg(msg.data<char>(), msg.size());
            if (in_msg.isEmpty() == false)
                emit signalNewMessage(in_msg);
            else
                emit signalError(QLatin1String("ServerConnection::receive - Empty message! Skip ..."));
        }
    }
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
