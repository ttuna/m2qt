#include "m2qthandler.h"
#include "m2qt.h"

#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
M2QtHandler::M2QtHandler(QObject *parent) : QObject(parent)
{

}

M2QtHandler::~M2QtHandler()
{
    cleanup();
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void M2QtHandler::cleanup()
{
    m_p_m2qt->cleanup();

    if (m_p_zmq_ctx)
    {
        delete m_p_zmq_ctx;
        m_p_zmq_ctx = nullptr;
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtHandler::init(const QVariantMap &in_params)
{
    qDebug() << "M2QtHandler::init";
    if (update(in_params) == false) return false;

    m_p_m2qt = &M2QtLoader::load();
    if (m_p_m2qt == nullptr) return false;

    m_p_zmq_ctx = new zmq::context_t;
    if (m_p_zmq_ctx == nullptr) return false;

    QVariantMap params;
    params["pull_addr"] = QByteArray("tcp://127.0.0.1:9999");
    params["pub_addr"] = QByteArray("tcp://127.0.0.1:9998");
    params["sender_id"] = QByteArray("f400bf85-4538-4f7a-8908-67e313d515c2");

    m_p_m2qt->init(m_p_zmq_ctx, params);

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtHandler::update(const QVariantMap& in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtHandler::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void M2QtHandler::recv()
{
    qDebug() << "M2QtHandler::recv";
    if (m_initialized == false) return;

    m_p_m2qt->receive();
}
