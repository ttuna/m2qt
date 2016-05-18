#include "m2qt_handler.h"
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

    m_name = in_name;
    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void Handler::cleanup()
{
    m_name = QString();
    m_callback = HandlerCallback();
    m_initialized = false;
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool Handler::update(const QVariantMap& in_params)
{
    if (in_params.contains("default_callback"))
    {
        QString def_callback = in_params["default_callback"].toString();
        m_callback = DefaultCallbackManager::getCallback(def_callback);
        qDebug() << "Handler::update - default_callback:" << def_callback;
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

    rep = m_callback(in_req);
    if (M2QT::isRepEmpty(rep)) return;

    msg = to<QByteArray>(rep);
    emit signalSendMsg(msg);
}

// ----------------------------------------------------------------------------
// registerCallback
// ----------------------------------------------------------------------------
bool Handler::setCallback(HandlerCallback in_callback)
{
    if (m_initialized == false) { emit signalError("Handler::registerCallback - Handler not initialized!"); return false; }

    m_callback = in_callback;
    return true;
}

// ----------------------------------------------------------------------------
// updateDefCallbacks
// ----------------------------------------------------------------------------
void Handler::updateDefCallbacks(QString default_callbacks)
{

}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
QString Handler::name() const
{
    return m_name;
}

QByteArray Handler::msgPrefix() const
{
    return m_msg_prefix;
}

void Handler::setMsgPrefix(QByteArray msg_prefix)
{
    if (m_msg_prefix == msg_prefix)
        return;

    m_msg_prefix = msg_prefix;
    emit signalMsgPrefixChanged(msg_prefix);
}
