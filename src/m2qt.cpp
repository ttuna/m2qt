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
    explicit M2Qt();

public slots:
    void cleanup();
    bool init(zmq::context_t *in_ctx);
    bool isValid() const;
    IM2QtHandler* createM2QtHandler(const QString& in_name, const QVariantMap &in_params);
    IM2QtHandler* getM2QtHandler(const QString& in_name) const;
    void startHandler(const QString &in_name = QString()) const;

private:
    bool m_initialized = false;
    zmq::context_t* m_p_zmq_ctx = nullptr;

    QMap<QString, Handler*> m_handler_map;
};
#include "m2qt.moc"

} // namespace

using namespace M2QT;

// ----------------------------------------------------------------------------
// ctor / dtor / ...
// ----------------------------------------------------------------------------
M2Qt::M2Qt()
{

}

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
IM2QtHandler *M2Qt::createM2QtHandler(const QString& in_name, const QVariantMap &in_params)
{
    if (m_initialized == false) return nullptr;
    if (in_name.isEmpty()) return nullptr;
    if (in_params.isEmpty()) return nullptr;
    if (m_handler_map.contains(in_name)) return m_handler_map.value(in_name, nullptr);

    QScopedPointer<QThread> p_thread(new QThread());
    if (p_thread.isNull()) return nullptr;
    QScopedPointer<Handler> p_handler(new Handler());
    if (p_handler.isNull()) return nullptr;

    p_handler->setZmqCtx(m_p_zmq_ctx);
    p_handler->setParams(in_params);

    QThread* t = p_thread.take();
    Handler* h = p_handler.take();
    m_handler_map[in_name] = h;

    // start handler when thread has started ...
    QObject::connect(t, &QThread::started, h, &Handler::start);
    // delete handler when thread has finished ...
    QObject::connect(t, &QThread::finished, h, &QObject::deleteLater);

    h->moveToThread(t);
    t->start();

    return h;
}

// ----------------------------------------------------------------------------
// getM2QtHandler
// ----------------------------------------------------------------------------
IM2QtHandler *M2Qt::getM2QtHandler(const QString &in_name) const
{
    if (m_initialized == false) return nullptr;
    if (in_name.isEmpty()) return nullptr;
    if (m_handler_map.contains(in_name)) return m_handler_map.value(in_name, nullptr);
}

// ----------------------------------------------------------------------------
// getM2QtHandler
// ----------------------------------------------------------------------------
void M2Qt::startHandler(const QString &in_name /*=QString()*/) const
{
    if (m_initialized == false) return;

    QStringList handler_names = (in_name.isEmpty()) ? m_handler_map.keys() : QStringList(in_name);
    foreach(QString name, handler_names)
    {
        IM2QtHandler* handler = m_handler_map.value(name, nullptr);
        if (handler == nullptr) continue;

        handler->start();
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
