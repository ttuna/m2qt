#include "m2qt_handler.h"
#include "m2qt_serverconnection.h"
#include "m2qt_messageparser.h"
#include "m2qt_callback.h"

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
bool Handler::init(const QString &in_name, zmq::context_t *in_zmq_ctx, const QVariantMap &in_params)
{
    qDebug() << "Handler::init";
    if (in_zmq_ctx == nullptr) { emit signalError("Handler::init - zmq_context == nullptr!"); return false; }
    if (in_params.isEmpty()) { emit signalError("Handler::init - Parameter map is empty"); return false; }
    if (update(in_params) == false) { emit signalError("Handler::init - update() failed!"); return false; }

    QScopedPointer<ServerConnection> tmp_server_con(new ServerConnection);
    if (tmp_server_con.isNull()) { emit signalError("Handler::init - Couldn't allocate ServerConnection!"); return false; }
    if (tmp_server_con->init(in_zmq_ctx, in_params) == false) { emit signalError("Handler::init - ServerConnection initialisation failed!"); return false; }

    QScopedPointer<MessageParser> tmp_msg_parser(new MessageParser);
    if (tmp_msg_parser.isNull()) { emit signalError("Handler::init - Couldn't allocate MessageParser!"); return false; }

    m_p_server_con = QSharedPointer<ServerConnection>(tmp_server_con.take());
    m_p_parser = QSharedPointer<MessageParser>(tmp_msg_parser.take());

    connect(m_p_server_con.data(), &ServerConnection::signalNewMessage, m_p_parser.data(), &MessageParser::parse);
    connect(m_p_parser.data(), &MessageParser::signalResult, this, &Handler::handleParserResults);

    if (m_default_callbacks.isEmpty() == false)
        m_def_callbacks = DefaultCallbacks::getCallbacks(m_default_callbacks);

    connect(this, &Handler::signalDefaultCallbacksChanged, this, &Handler::updateDefCallbacks);

    m_name = in_name;
    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void Handler::cleanup()
{
    m_default_callbacks = QStringList();
    m_name = QString();
    m_initialized = false;
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
// isValid
// ----------------------------------------------------------------------------
bool Handler::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// handleParserResults
// ----------------------------------------------------------------------------
void Handler::handleParserResults(const Request &in_req)
{
    if (m_initialized == false) { emit signalError("Handler::handleParserResults - Handler not initialized!"); return; }
    if (M2QT::isReqEmpty(in_req) == true) { emit signalError("Handler::handleParserResults - Request is empty!"); return; }

    Response rep;
    QByteArray msg;

    // I. default callbacks ...
    foreach (HandlerCallback callback, m_def_callbacks)
    {
        rep = callback(in_req);
        if (M2QT::isRepEmpty(rep)) continue;
        msg = to<QByteArray>(rep);
        m_p_server_con->send(msg);
    }

    // II. user callbacks ...
    foreach (UserCallback callback, m_user_callbacks)
    {
        rep = (std::get<1>(callback)(in_req));
        if (M2QT::isRepEmpty(rep)) continue;
        msg = to<QByteArray>(rep);
        m_p_server_con->send(msg);
    }
}

// ----------------------------------------------------------------------------
// start
// ----------------------------------------------------------------------------
void Handler::start()
{
    if (m_initialized == false) { emit signalError("Handler::start - Handler not initialized!"); return; }

    // send first INIT msg ...
    qDebug() << "Handler::start - send init msg" << m_p_server_con->senderId();
    m_p_server_con->send(m_p_server_con->senderId());

    QFuture<void> poll_future = QtConcurrent::run(m_p_server_con.data(), &ServerConnection::poll);
    m_poll_watcher.setFuture(poll_future);  // TODO: ...

    emit signalStarted();
}

// ----------------------------------------------------------------------------
// stop
// ----------------------------------------------------------------------------
void Handler::stop()
{
    if (m_initialized == false) { emit signalError("Handler::stop - Handler not initialized!"); return; }
    m_p_server_con->stop();
    emit signalStopped();
}

// ----------------------------------------------------------------------------
// registerCallback
// ----------------------------------------------------------------------------
bool Handler::registerCallback(const QString &in_name, HandlerCallback in_callback)
{
    if (m_initialized == false) { emit signalError("Handler::registerCallback - Handler not initialized!"); return false; }
    if (in_name.isEmpty()) { emit signalError("Handler::registerCallback - Callback name is empty!"); return false; }

    m_user_callbacks.push_back(std::make_tuple(in_name, in_callback));
    return true;
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
QString Handler::name() const
{
    return m_name;
}

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
