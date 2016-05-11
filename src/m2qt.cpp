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

public slots:
    bool createHandler(const QString& in_name, const QVariantMap &in_params) override;
    void startHandler(const QString &in_name = QString()) const override;
    void stopHandler(const QString &in_name = QString()) const override;
    void addHandlerCallback(const QString& in_name, HandlerCallback in_callback) const override;

private:
    bool m_initialized = false;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    QMap<QString, Handler*> m_handler_map;
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
    if (in_ctx == nullptr) return false;

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
// createM2QtHandler
// ----------------------------------------------------------------------------
bool M2Qt::createHandler(const QString& in_name, const QVariantMap &in_params)
{
    if (m_initialized == false) return false;
    if (in_name.isEmpty()) return false;
    if (in_params.isEmpty()) return false;
    if (m_handler_map.contains(in_name)) return (m_handler_map.value(in_name, nullptr) != nullptr);

    QScopedPointer<Handler> p_handler(new Handler());
    if (p_handler.isNull()) return false;
    if (p_handler->init(m_p_zmq_ctx, in_params) == false) return false;
    m_handler_map[in_name] = p_handler.take();

    return true;
}

// ----------------------------------------------------------------------------
// startHandler
// ----------------------------------------------------------------------------
void M2Qt::startHandler(const QString &in_name /*=QString()*/) const
{
    if (m_initialized == false) return;

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
    if (m_initialized == false) return;

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
void M2Qt::addHandlerCallback(const QString& in_name, HandlerCallback in_callback) const
{

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
