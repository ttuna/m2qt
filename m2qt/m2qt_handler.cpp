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
    qDebug() << "\nHandler::init";
    if (in_zmq_ctx == nullptr) { emit signalError(QStringLiteral("Handler::init - zmq_context == nullptr!")); return false; }
    if (in_params.isEmpty()) { emit signalError(QStringLiteral("Handler::init - Parameter map is empty")); return false; }
    if (update(in_params) == false) { emit signalError(QStringLiteral("Handler::init - update() failed!")); return false; }

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
    m_handler_callback = HandlerCallback();
    m_initialized = false;
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool Handler::update(const QVariantMap& in_params)
{
    QString key(QLatin1String("default_callback"));
    if (in_params.contains(key))
    {
        QString def_callback = in_params[key].toString();
        // get handler callback ...
        m_handler_callback = CallbackManager::getHandlerCallback(def_callback);
        // get corresponding filter callback ...
        m_filter_callback = CallbackManager::getFilterCallback(def_callback);
        qDebug() << "Handler::update - default_callback / filter_callback:" << def_callback;
    }

    key = QLatin1String("user_callback");
    if (in_params.contains(key))
    {
        QVariant var_handler = in_params.value(key, QVariant());
        if (var_handler.isValid() == false) return false;
        qDebug() << "Handler::update - user_callback:" << var_handler;
        m_handler_callback = var_handler.value<HandlerCallback>();
    }

    key = QLatin1String("user_filter");
    if (in_params.contains(key))
    {
        QVariant var_filter = in_params.value(key, QVariant());
        if (var_filter.isValid() == false) return false;
        qDebug() << "Handler::update - user_filter:" << var_filter;
        m_filter_callback = var_filter.value<FilterCallback>();
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
    if (m_initialized == false) { emit signalError(QStringLiteral("Handler::handleParserResults - Handler not initialized!")); return; }
    if (M2QT::isReqEmpty(in_req) == true) { emit signalError(QStringLiteral("Handler::handleParserResults - Request is empty!")); return; }
    if (m_handler_callback == nullptr) return;

    // apply filter callback if available ...
    if (m_filter_callback != nullptr && m_filter_callback(in_req) == false) return;

    Response rep;
    QByteArray msg;

    // call handler callback ...
    rep = m_handler_callback(in_req);
    if (M2QT::isRepEmpty(rep)) return;

    // send response ...
    msg = to<QByteArray>(rep);
    emit signalSendMsg(msg);
}

// ----------------------------------------------------------------------------
// setHandlerCallback
// ----------------------------------------------------------------------------
bool Handler::setHandlerCallback(HandlerCallback in_callback)
{
    if (m_initialized == false) { emit signalError(QStringLiteral("Handler::setHandlerCallback - Handler not initialized!")); return false; }

    m_handler_callback = in_callback;
    emit signalDebug(QStringLiteral("Handler::setHandlerCallback - set handler callback"));
    return true;
}

// ----------------------------------------------------------------------------
// setFilterCallback
// ----------------------------------------------------------------------------
bool Handler::setFilterCallback(FilterCallback in_callback)
{
    if (m_initialized == false) { emit signalError(QStringLiteral("Handler::setFilterCallback - Handler not initialized!")); return false; }

    m_filter_callback = in_callback;
    return true;
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
