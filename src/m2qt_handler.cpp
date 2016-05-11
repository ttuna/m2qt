#include "m2qt_handler.h"
#include "m2qt_serverconnection.h"
#include "m2qt_messageparser.h"
#include "m2qt_defaultcallbacks.h"

#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
Handler::Handler(QObject *parent) : QObject(parent)
{

}

Handler::~Handler()
{
    cleanup();
}

// ----------------------------------------------------------------------------
// init
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

    if (m_default_callbacks.isEmpty() == false)
        m_def_callbacks = DefaultCallbacks::getCallbacks(m_default_callbacks);

    connect(this, &Handler::signalDefaultCallbacksChanged, this, &Handler::updateDefCallbacks);

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool Handler::update(const QVariantMap& in_params)
{
    if (in_params.contains("default_callbacks"))
    {
        m_default_callbacks = in_params["default_callbacks"].toStringList();
        qDebug() << "Handler::update - default_callbacks:" << m_default_callbacks;
    }

    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void Handler::cleanup()
{

}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool Handler::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// handleParserResults
// ----------------------------------------------------------------------------
void Handler::handleParserResults(const Request &msg)
{
    if (m_initialized == false) return;
    if (isReqEmpty(msg) == true) return;

    Response rep;

    // I. default callbacks ...
    foreach (HandlerCallback callback, m_def_callbacks)
    {
        rep = callback(msg);
        if (isRepEmpty(rep)) continue;

        m_p_server_con->send(rep);
    }

    // II. user callbacks ...
    foreach (UserCallback callback, m_user_callbacks)
    {
        rep = (std::get<1>(callback)(msg));
        if (isRepEmpty(rep)) continue;

        m_p_server_con->send(rep);
    }
}

// ----------------------------------------------------------------------------
// start
// ----------------------------------------------------------------------------
void Handler::start()
{
    if (m_initialized == false) return;
    QFuture<void> poll_future = QtConcurrent::run(m_p_server_con.data(), &ServerConnection::poll);
    m_poll_watcher.setFuture(poll_future);
    emit signalStarted();
}

// ----------------------------------------------------------------------------
// stop
// ----------------------------------------------------------------------------
void Handler::stop()
{
    if (m_initialized == false) return;
    m_p_server_con->stop();
    emit signalStopped();
}

// ----------------------------------------------------------------------------
// registerCallback
// ----------------------------------------------------------------------------
void Handler::registerCallback(const QString &in_name, HandlerCallback in_callback)
{
    if (m_initialized == false) return;
    if (in_name.isEmpty()) return;

    m_user_callbacks.push_back(std::make_tuple(in_name, in_callback));
}

// ----------------------------------------------------------------------------
// updateDefCallbacks
// ----------------------------------------------------------------------------
void Handler::updateDefCallbacks(QStringList default_callbacks)
{
    m_def_callbacks = DefaultCallbacks::getCallbacks(default_callbacks);
}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
QStringList Handler::defaultCallbacks() const
{
    return m_default_callbacks;
}

void Handler::setDefaultCallbacks(QStringList default_callbacks)
{
    if (m_default_callbacks == default_callbacks)
        return;

    m_default_callbacks = default_callbacks;
    emit signalDefaultCallbacksChanged(default_callbacks);
}
