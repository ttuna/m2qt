#include "m2qt.h"
#include "m2qt_handler.h"
#include "m2qt_serverconnection.h"
#include "m2qt_messageparser.h"
#include "m2qt_callback.h"

#include <zmq.hpp>

#include <QVariant>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QDebug>


namespace M2QT {

// ----------------------------------------------------------------------------
//
// class M2QtSignalAgent
//
// ----------------------------------------------------------------------------
SignalAgent::SignalAgent(QObject *in_parent) : QObject(in_parent)
{
}

// ----------------------------------------------------------------------------
//
// class M2Qt
//
// ----------------------------------------------------------------------------
class M2Qt final : public IM2Qt
{
    Q_OBJECT
    Q_PROPERTY(SignalAgent* p_signal_agent READ signalAgent WRITE setSignalAgent NOTIFY signalSignalAgentChanged)
public:
    explicit M2Qt() = default;
    ~M2Qt();
    M2Qt(const M2Qt& other) = delete;
    M2Qt& operator= (const M2Qt& other) = delete;

    bool init(zmq::context_t *in_ctx, const QVariantMap &in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid() const override;

    //properties ...
    SignalAgent* signalAgent() const;

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;
    void signalStarted() const;
    void signalStopped() const;

    //properties ...
    void signalSignalAgentChanged(SignalAgent* p_signal_agent);

public slots:
    bool createHandler(const QString& in_name, const QVariantMap &in_params) override;
    void start() override;
    void stop() override;
    void setHandlerCallback(const QString &in_handler_name, const QString &in_callback_name, HandlerCallback in_callback) const override;

    //properties ...
    void setSignalAgent(SignalAgent *in_signal_agent);

private:
    bool m_initialized = false;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    QMap<QString, Handler*> m_handler_map;
    SignalAgent* m_p_signal_agent = nullptr;
    QSharedPointer<ServerConnection> m_p_server_con;
    QSharedPointer<MessageParser> m_p_parser;
    QSharedPointer<DefaultCallbackManager> m_p_def_cb_manager;
    QFutureWatcher<void> m_poll_watcher;
};

#include "m2qt.moc"

} // namespace

using namespace M2QT;

// ----------------------------------------------------------------------------
// dtor ...
// ----------------------------------------------------------------------------
M2Qt::~M2Qt()
{
    cleanup();
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool M2Qt::init(zmq::context_t* in_ctx, const QVariantMap& in_params)
{
    qDebug() << "M2Qt::init";
    if (in_ctx == nullptr) { emit signalError("M2Qt::init - zmq_context == null!"); return false; }
    if (update(in_params) == false) { emit signalError("M2Qt::init - update() failed!"); return false; }

    QScopedPointer<ServerConnection> tmp_server_con(new ServerConnection);
    if (tmp_server_con.isNull()) { emit signalError("M2Qt::init - Couldn't allocate ServerConnection!"); return false; }
    if (tmp_server_con->init(in_ctx, in_params) == false) { emit signalError("M2Qt::init - ServerConnection initialisation failed!"); return false; }

    QScopedPointer<MessageParser> tmp_msg_parser(new MessageParser);
    if (tmp_msg_parser.isNull()) { emit signalError("M2Qt::init - Couldn't allocate MessageParser!"); return false; }

    QScopedPointer<DefaultCallbackManager> tmp_def_cb_manager(new DefaultCallbackManager);
    if (tmp_def_cb_manager.isNull()) { emit signalError("M2Qt::init - Couldn't allocate DefaultCallbackManager!"); return false; }

    m_p_server_con = QSharedPointer<ServerConnection>(tmp_server_con.take());
    m_p_parser = QSharedPointer<MessageParser>(tmp_msg_parser.take());
    m_p_def_cb_manager = QSharedPointer<DefaultCallbackManager>(tmp_def_cb_manager.take());

    connect(m_p_server_con.data(), &ServerConnection::signalNewMessage, m_p_parser.data(), &MessageParser::parse);
    connect(m_p_def_cb_manager.data(), &DefaultCallbackManager::signalError, this, &M2Qt::signalError);
    connect(m_p_def_cb_manager.data(), &DefaultCallbackManager::signalWarning, this, &M2Qt::signalWarning);
    connect(m_p_def_cb_manager.data(), &DefaultCallbackManager::signalDebug, this, &M2Qt::signalDebug);
    connect(m_p_def_cb_manager.data(), &DefaultCallbackManager::signalInfo, this, &M2Qt::signalInfo);

    m_p_zmq_ctx = in_ctx;
    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void M2Qt::cleanup()
{
    qDebug() << "M2Qt::cleanup";
    stop();
    if (m_handler_map.isEmpty() == false)
    {
        qDeleteAll(m_handler_map);
        m_handler_map.clear();
    }

    setSignalAgent(nullptr);
    m_p_zmq_ctx = nullptr;
    m_initialized = false;
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool M2Qt::update(const QVariantMap& in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool M2Qt::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// setSignalAgent
// ----------------------------------------------------------------------------
void M2Qt::setSignalAgent(SignalAgent* in_signal_agent)
{
    disconnect(this, &M2Qt::signalError, 0, 0);
    disconnect(this, &M2Qt::signalWarning, 0, 0);
    disconnect(this, &M2Qt::signalDebug, 0, 0);
    disconnect(this, &M2Qt::signalInfo, 0, 0);

    m_p_signal_agent = in_signal_agent;

    if (in_signal_agent != nullptr)
    {
        connect(this, &M2Qt::signalError, in_signal_agent, &SignalAgent::signalError);
        connect(this, &M2Qt::signalWarning, in_signal_agent, &SignalAgent::signalWarning);
        connect(this, &M2Qt::signalDebug, in_signal_agent, &SignalAgent::signalDebug);
        connect(this, &M2Qt::signalInfo, in_signal_agent, &SignalAgent::signalInfo);
    }
}

// ----------------------------------------------------------------------------
// createM2QtHandler
// ----------------------------------------------------------------------------
bool M2Qt::createHandler(const QString& in_name, const QVariantMap &in_params)
{
    if (m_initialized == false) { emit signalError("M2Qt::createHandler - M2Qt not initialized!"); return false; }
    if (in_name.isEmpty()) { emit signalError("M2Qt::createHandler - Handler name is empty!"); return false; }
    if (in_params.isEmpty()) { emit signalError("M2Qt::createHandler - Handler params are empty!"); return false; }
    if (m_handler_map.contains(in_name)) return (m_handler_map.value(in_name, nullptr) != nullptr);

    QScopedPointer<Handler> p_handler(new Handler());
    if (p_handler.isNull()) { emit signalError("M2Qt::createHandler - Couldn't allocate handler!"); return false; }
    if (p_handler->init(in_name, m_p_zmq_ctx, in_params) == false) { emit signalError("M2Qt::createHandler - Handler initialisation failed!"); return false; }

    // handler signals ...
    connect(m_p_parser.data(), &MessageParser::signalResult, p_handler.data(), &Handler::handleParserResults);
    connect(p_handler.data(), &Handler::signalSendMsg, m_p_server_con.data(), &ServerConnection::send);
    connect(p_handler.data(), &Handler::signalError, this, &M2Qt::signalError);

    // store into map ...   // TODO: implement removeHandler();
    m_handler_map[in_name] = p_handler.take();

    return true;
}

// ----------------------------------------------------------------------------
// start
// ----------------------------------------------------------------------------
void M2Qt::start()
{
    if (m_initialized == false) { emit signalError("M2Qt::start - M2Qt not initialized!"); return; }

    // send first INIT msg ...
    qDebug() << "M2Qt::start - send init msg" << m_p_server_con->senderId();
    m_p_server_con->send(m_p_server_con->senderId());

    const QFuture<void> poll_future = QtConcurrent::run(m_p_server_con.data(), &ServerConnection::poll);
    m_poll_watcher.setFuture(poll_future);  // TODO: ...

    emit signalStarted();
}

// ----------------------------------------------------------------------------
// stop
// ----------------------------------------------------------------------------
void M2Qt::stop()
{
    if (m_initialized == false) { emit signalError("M2Qt::stopHandler - M2Qt not initialized!"); return; }
    m_p_server_con->stop();
    emit signalStopped();
}

// ----------------------------------------------------------------------------
// setHandlerCallback
// ----------------------------------------------------------------------------
void M2Qt::setHandlerCallback(const QString& in_handler_name, const QString& in_callback_name, HandlerCallback in_callback) const
{
    if (m_initialized == false) { emit signalError("M2Qt::setHandlerCallback - M2Qt not initialized!"); return; }
    if (in_handler_name.isEmpty()) { emit signalError("M2Qt::setHandlerCallback - in_handler_name is empty!"); return; }
    if (in_callback_name.isEmpty()) { emit signalError("M2Qt::setHandlerCallback - in_callback_name is empty!"); return; }
    if (m_handler_map.contains(in_handler_name) == false) { emit signalError("M2Qt::setHandlerCallback - in_handler_name not found!"); return; }

    Handler* handler = m_handler_map.value(in_handler_name, nullptr);
    if (handler == nullptr) return;

    handler->setCallback(in_callback);
}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
SignalAgent *M2Qt::signalAgent() const
{
    return m_p_signal_agent;
}


// ----------------------------------------------------------------------------
//
// class M2QtCreator
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// getM2Qt (static)
// ----------------------------------------------------------------------------
IM2Qt *M2QtLoader::getM2Qt(const QVariantMap &in_params)
{
    static M2Qt *p_lib;

    if (p_lib == nullptr)
    {
        QScopedPointer<M2Qt> tmp_p_lib(new M2Qt());
        if (tmp_p_lib.isNull()) return nullptr;

        zmq::context_t* p_zmq_ctx = new zmq::context_t;
        if (p_zmq_ctx == nullptr) return nullptr;

        if (tmp_p_lib->init(p_zmq_ctx, in_params) == false) return nullptr;
        p_lib = tmp_p_lib.take();
    }

    return p_lib;
}

// ----------------------------------------------------------------------------
// getM2Qt (static)
// ----------------------------------------------------------------------------
SignalAgent *M2QtLoader::getSignalAgent()
{
    return new SignalAgent();
}
