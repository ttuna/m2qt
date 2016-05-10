#include "m2qt_handler.h"
#include "m2qt_serverconnection.h"
#include "m2qt_messageparser.h"
#include "m2qt.h"

#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Handler::Handler()
{

}

Handler::~Handler()
{
    cleanup();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void Handler::start()
{
    if (init(m_p_zmq_ctx, m_params) == false) return;

    emit signalStarted();
    m_p_server_con->receive(); // loop ...
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void Handler::stop()
{
    emit signalStopped();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void Handler::registerCallback(const QString &in_name, IM2QtHandlerCallback in_callback)
{

}



// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool Handler::init(zmq::context_t *in_ctx, const QVariantMap &in_params)
{
    qDebug() << "Handler::init";
    if (in_ctx == nullptr) return false;
    if (in_params.isEmpty()) return false;
    if (update(in_params) == false) return false;

    QScopedPointer<ServerConnection> tmp_server_con(new ServerConnection);
    if (tmp_server_con.isNull()) return false;
    if (tmp_server_con->init(in_ctx, in_params) == false) return false;
    QScopedPointer<MessageParser> tmp_msg_parser(new MessageParser);
    if (tmp_msg_parser.isNull()) return false;

    m_p_server_con = QSharedPointer<ServerConnection>(tmp_server_con.take());
    m_p_parser = QSharedPointer<MessageParser>(tmp_msg_parser.take());

    connect(m_p_server_con.data(), &ServerConnection::signalNewMessage, m_p_parser.data(), &MessageParser::parse);
    connect(m_p_parser.data(), &MessageParser::signalResult, this, &Handler::handleParserResults);

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool Handler::update(const QVariantMap& in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void Handler::cleanup()
{

    if (m_p_server_con.isNull() == false)
    {
        m_p_server_con.clear();
    }

    if (m_p_parser.isNull() == false)
    {
        m_p_parser.clear();
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool Handler::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// handleResult
// ----------------------------------------------------------------------------
void Handler::handleParserResults(const Message &msg)
{
    if (m_initialized == false) return;

    QString uuid = std::get<0>(msg);
    QString id = std::get<1>(msg);
    QString path = std::get<2>(msg);
    QJsonObject header = std::get<3>(msg);
    QList<NetString> body = std::get<4>(msg);

    qDebug() << "\nHandler::handleParserMessage:";
    qDebug() << "\tuuid:" << uuid;
    qDebug() << "\tid:" << id;
    qDebug() << "\tpath:" << path;
    qDebug() << "\theader:" << header;
}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
zmq::context_t *Handler::zmqCtx() const
{
    return m_p_zmq_ctx;
}

QVariantMap Handler::params() const
{
    return m_params;
}

void Handler::setZmqCtx(zmq::context_t *p_zmq_ctx)
{
    if (m_p_zmq_ctx == p_zmq_ctx)
        return;

    m_p_zmq_ctx = p_zmq_ctx;
    emit signalZmqCtxChanged(p_zmq_ctx);
}

void Handler::setParams(QVariantMap params)
{
    if (m_params == params)
        return;

    m_params = params;
    emit signalParamsChanged(params);
}
