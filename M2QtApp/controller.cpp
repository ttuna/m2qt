#include "controller.h"
#include "m2qt.h"

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

    m_p_m2qt = M2QtLoader::getM2Qt(in_params);
    if (m_p_m2qt == nullptr) return false;

    m_p_signal_agent = M2QtLoader::getSignalAgent();
    if (m_p_signal_agent == nullptr) return false;

    connect(m_p_signal_agent, &SignalAgent::signalError, this, &Controller::slotError);
    connect(m_p_signal_agent, &SignalAgent::signalWarning, this, &Controller::slotWarning);
    connect(m_p_signal_agent, &SignalAgent::signalDebug, this, &Controller::slotDebug);
    connect(m_p_signal_agent, &SignalAgent::signalInfo, this, &Controller::slotInfo);

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
    Q_UNUSED(in_params)
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
// start
// ----------------------------------------------------------------------------
void Controller::start()
{
    if (m_initialized == false) return;
    m_p_m2qt->start();
}

// ----------------------------------------------------------------------------
// stop
// ----------------------------------------------------------------------------
void Controller::stop()
{
    if (m_initialized == false) return;
    m_p_m2qt->stop();
}

// ----------------------------------------------------------------------------
// printMessage
// ----------------------------------------------------------------------------
void Controller::printMessage(const QString &in_msg)
{
    // TODO: add decent logger here ...
    QMessageLogger().debug(in_msg.toLatin1());
}

// ----------------------------------------------------------------------------
// Signal Agent slots ...
// ----------------------------------------------------------------------------
void Controller::slotError(QString in_msg)
{
    QMessageLogger().critical(in_msg.toLatin1());
}

void Controller::slotWarning(QString in_msg)
{
    QMessageLogger().warning(in_msg.toLatin1());
}

void Controller::slotDebug(QString in_msg)
{
    QMessageLogger().debug(in_msg.toLatin1());
}

void Controller::slotInfo(QString in_msg)
{
    QMessageLogger().info(in_msg.toLatin1());
}
