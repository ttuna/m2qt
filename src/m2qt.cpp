#include "m2qt.h"
#include "m2qt_handler.h"

#include <zmq.hpp>

#include <QVariant>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QDebug>


namespace M2QT {

// ----------------------------------------------------------------------------
//
// class M2Qt
//
// ----------------------------------------------------------------------------
class M2Qt final : public IM2Qt
{
    Q_OBJECT
public:
    explicit M2Qt() = default;
    ~M2Qt() = default;
    M2Qt(const M2Qt& other) = delete;
    M2Qt& operator= (const M2Qt& other) = delete;

    bool init(zmq::context_t *in_ctx);
    void cleanup();
    bool isValid() const override;

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

public slots:
    void setSignalAgent(const M2QtSignalAgent *in_signal_agent);
    bool createHandler(const QString& in_name, const QVariantMap &in_params) override;
    void startHandler(const QString &in_name = QString()) const override;
    void stopHandler(const QString &in_name = QString()) const override;
    bool addHandlerCallback(const QString& in_handler_name, const QString &in_callback_name, HandlerCallback in_callback) const override;

private:
    bool m_initialized = false;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    QMap<QString, Handler*> m_handler_map;
    const M2QtSignalAgent* m_p_signal_agent = nullptr;
};
#include "m2qt.moc"

} // namespace

using namespace M2QT;

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void M2Qt::cleanup()
{
    qDebug() << "M2Qt::cleanup";

    if (m_handler_map.isEmpty() == false)
    {
        qDeleteAll(m_handler_map);
        m_handler_map.clear();
    }
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool M2Qt::init(zmq::context_t* in_ctx)
{
    qDebug() << "M2Qt::init";
    if (in_ctx == nullptr)
    {
        emit signalError("M2Qt::init - zmq_context == null!");
        return false;
    }

    m_p_zmq_ctx = in_ctx;

    m_initialized = true;
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
void M2Qt::setSignalAgent(const M2QtSignalAgent* in_signal_agent)
{
    disconnect(this, &M2Qt::signalError, 0, 0);
    disconnect(this, &M2Qt::signalWarning, 0, 0);
    disconnect(this, &M2Qt::signalDebug, 0, 0);
    disconnect(this, &M2Qt::signalInfo, 0, 0);

    if (in_signal_agent != nullptr)
    {
        m_p_signal_agent = in_signal_agent;
        connect(this, &M2Qt::signalError, in_signal_agent, &M2QtSignalAgent::signalError);
        connect(this, &M2Qt::signalWarning, in_signal_agent, &M2QtSignalAgent::signalWarning);
        connect(this, &M2Qt::signalDebug, in_signal_agent, &M2QtSignalAgent::signalDebug);
        connect(this, &M2Qt::signalInfo, in_signal_agent, &M2QtSignalAgent::signalInfo);
    }
    else
    {
        m_p_signal_agent = nullptr;
    }
}

// ----------------------------------------------------------------------------
// createM2QtHandler
// ----------------------------------------------------------------------------
bool M2Qt::createHandler(const QString& in_name, const QVariantMap &in_params)
{
    if (m_initialized == false)
    {
        emit signalError("M2Qt::createHandler - M2Qt not initialized!");
        return false;
    }
    if (in_name.isEmpty())
    {
        emit signalError("M2Qt::createHandler - Handler name is empty!");
        return false;
    }
    if (in_params.isEmpty())
    {
        emit signalError("M2Qt::createHandler - Handler params are empty!");
        return false;
    }
    if (m_handler_map.contains(in_name)) return (m_handler_map.value(in_name, nullptr) != nullptr);

    QScopedPointer<Handler> p_handler(new Handler());
    if (p_handler.isNull())
    {
        emit signalError("M2Qt::createHandler - Couldn't create handler!");
        return false;
    }
    if (p_handler->init(in_name, m_p_zmq_ctx, in_params) == false)
    {
        emit signalError("M2Qt::createHandler - Handler initialisation failed!");
        return false;
    }

    // forward handler signals ...
    connect(p_handler.data(), &Handler::signalError, this, &M2Qt::signalError);

    // store into map ...   // TODO: implement removeHandler();
    m_handler_map[in_name] = p_handler.take();

    return true;
}

// ----------------------------------------------------------------------------
// startHandler
// ----------------------------------------------------------------------------
void M2Qt::startHandler(const QString &in_name /*=QString()*/) const
{
    if (m_initialized == false)
    {
        emit signalError("M2Qt::startHandler - M2Qt not initialized!");
        return;
    }

    // if handler name is empty start all handler ...
    QStringList handler_names = (in_name.isEmpty()) ? m_handler_map.keys() : QStringList(in_name);
    foreach(QString name, handler_names)
    {
        Handler* handler = m_handler_map.value(name, nullptr);
        if (handler == nullptr) continue;

        handler->start();
    }
}

// ----------------------------------------------------------------------------
// stopHandler
// ----------------------------------------------------------------------------
void M2Qt::stopHandler(const QString &in_name /*=QString()*/) const
{
    if (m_initialized == false)
    {
        emit signalError("M2Qt::stopHandler - M2Qt not initialized!");
        return;
    }

    // if handler name is empty stop all handler ...
    QStringList handler_names = (in_name.isEmpty()) ? m_handler_map.keys() : QStringList(in_name);
    foreach(QString name, handler_names)
    {
        Handler* handler = m_handler_map.value(name, nullptr);
        if (handler == nullptr) continue;

        handler->stop();
    }
}

// ----------------------------------------------------------------------------
// addHandlerCallback
// ----------------------------------------------------------------------------
bool M2Qt::addHandlerCallback(const QString& in_handler_name, const QString& in_callback_name, HandlerCallback in_callback) const
{
    if (m_initialized == false)
    {
        emit signalError("M2Qt::addHandlerCallback - M2Qt not initialized!");
        return false;
    }
    if (m_handler_map.contains(in_handler_name) == false)
    {
        emit signalError("M2Qt::addHandlerCallback - Handler not found!");
        return false;
    }
}


// ----------------------------------------------------------------------------
//
// class M2QtCreator
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// createM2Qt (static)
// ----------------------------------------------------------------------------
IM2Qt *M2QtLoader::getM2Qt()
{
    static M2Qt *p_lib;

    if (p_lib == nullptr)
    {
        QScopedPointer<M2Qt> tmp_p_lib(new M2Qt());
        if (tmp_p_lib.isNull()) return nullptr;

        zmq::context_t* p_zmq_ctx = new zmq::context_t;
        if (p_zmq_ctx == nullptr) return nullptr;

        if (tmp_p_lib->init(p_zmq_ctx) == false) return nullptr;
        p_lib = tmp_p_lib.take();
    }

    return p_lib;
}
