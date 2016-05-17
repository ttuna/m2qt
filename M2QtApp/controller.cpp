#include "controller.h"
#include <QMessageLogger>
#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
// ctor / dtor ...
// ----------------------------------------------------------------------------
Controller::Controller(QObject *parent) : QObject(parent)
{

}

Controller::~Controller()
{
    cleanup();
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool Controller::init(const QVariantMap &in_params)
{
    if (update(in_params) == false) return false;

    m_p_m2qt = M2QtLoader::getM2Qt();
    if (m_p_m2qt == nullptr) return false;

    m_p_signal_agent = new M2QtSignalAgent();
    if (m_p_signal_agent == nullptr) return false;

    m_p_m2qt->setMsgPrefix("ws_msg");
    m_p_m2qt->setSignalAgent(m_p_signal_agent);

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void Controller::cleanup()
{
    if (m_p_signal_agent != nullptr)
    {
        m_p_m2qt->setSignalAgent(nullptr);
        delete m_p_signal_agent;
        m_p_signal_agent = nullptr;
    }
}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool Controller::update(const QVariantMap &in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool Controller::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// createHandler
// ----------------------------------------------------------------------------
bool Controller::createHandler(const QString &in_name, const QVariantMap &in_params)
{
    if (m_initialized == false) return false;
    if (in_name.isEmpty()) return false;

    bool created = m_p_m2qt->createHandler(in_name, in_params);

    if (created == false) qDebug() << "Controller::init handler creation failed ...";
    else m_handler_names.push_back(in_name);

    return created;
}

// ----------------------------------------------------------------------------
// startHandler
// ----------------------------------------------------------------------------
void Controller::startHandler(const QString &in_name /*=QString()*/)
{
    if (m_initialized == false) return;
    m_p_m2qt->startHandler(in_name);
}

// ----------------------------------------------------------------------------
// stopHandler
// ----------------------------------------------------------------------------
void Controller::stopHandler(const QString &in_name /*=QString()*/)
{
    if (m_initialized == false) return;
    m_p_m2qt->stopHandler(in_name);
}

// ----------------------------------------------------------------------------
// printMessage
// ----------------------------------------------------------------------------
void Controller::printMessage(const QString &in_msg)
{
    // TODO: add decent logger here ...
    QMessageLogger().debug(in_msg.toLatin1());
}
