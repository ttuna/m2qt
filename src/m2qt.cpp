#include "m2qt.h"
#include "serverconnection.h"
#include "zmq.hpp"
#include <QVariant>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QDebug>

using namespace M2QT;

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
    bool init(zmq::context_t *in_ctx, const QVariantMap &in_params);
    bool isValid() const;
    void receive();

private:
    bool m_initialized = false;
    QSharedPointer<ServerConnection> m_p_server_con;
};
#include "m2qt.moc"

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
    qDebug() << "M2Qt::cleanup()";

    if (m_p_server_con.isNull() == false)
    {
        m_p_server_con.clear();
    }
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool M2Qt::init(zmq::context_t* in_ctx, const QVariantMap &in_params)
{
    qDebug() << "M2Qt::init()";

    if (in_ctx == nullptr) return false;
    if (in_params.isEmpty()) return false;

    QScopedPointer<ServerConnection> tmp_server_con(new ServerConnection);
    if (tmp_server_con.isNull()) return false;
    if (tmp_server_con->init(in_ctx, in_params) == false) return false;

    m_p_server_con = QSharedPointer<ServerConnection>(tmp_server_con.take());

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
// receive
// ----------------------------------------------------------------------------
void M2Qt::receive()
{
    qDebug() << "M2Qt::receive()";
    if (m_initialized == false) return;

    m_p_server_con->receive();
}


// ----------------------------------------------------------------------------
//
// class M2QtLoader
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// load (static)
// ----------------------------------------------------------------------------
IM2Qt &M2QtLoader::load()
{
    qDebug() << "M2QtLoader::load()";
    static M2Qt* p_lib = nullptr;

    if (p_lib == nullptr) p_lib = new M2Qt();
    return *p_lib;
}
