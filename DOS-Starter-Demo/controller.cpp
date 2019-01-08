#include "controller.h"
#include "drivestarter.h"
#include "m2qt.h"

#include <QDir>
#include <QMessageLogger>
#include <QJsonObject>
#include <QDebug>

using namespace M2QT;

namespace {
// ----------------------------------------------------------------------------
// global DriveStarter ...
// ----------------------------------------------------------------------------
DriveStarter g_drive_starter;

// ----------------------------------------------------------------------------
// Drive start callback ...
// ----------------------------------------------------------------------------
Response callbackDriveStart(const Request& in_req)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_req;

    if (net_strings.size() < 2) return Response();

    // TODO: improve error handling :-)
    if (g_drive_starter.isValid() == false) return Response();
    if (g_drive_starter.state() != QProcess::NotRunning) return Response();

    // assume that payload is net_strings[1] ...
    QJsonObject json_data = M2QtHelper::netstring2Json(net_strings[1]);
    if (json_data.isEmpty()) return Response();
    qDebug() << "callbackDriveStart - json_data:" << json_data;

    // TODO ...
    QStringList args(json_data.find(QLatin1String("message")).value().toString());
    qDebug() << "callbackDriveStart - args:" << args;

    g_drive_starter.slotStart(args, true);

    // TODO ...
    Response resp;
    return resp;
}

// ----------------------------------------------------------------------------
// callbackFilter
// ----------------------------------------------------------------------------
bool callbackFilter(const Request& in_req)
{
    QByteArray uuid, id, path;
    QVector<NetString> net_strings;
    std::tie(uuid, id, path, net_strings) = in_req;

    if (net_strings.size() < 2) return false;

    QByteArray prefix;
    M2QtHelper::netstring2Json(net_strings[1], prefix);
    if (prefix.isEmpty()) return false;
    prefix = prefix.trimmed();
    qDebug() << "callbackFilter - prefix:" << prefix;

    if (prefix == QByteArray("@starter_msg")) return true;
    return false;
}

}   // namespace ...
// ----------------------------------------------------------------------------

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
    qDebug() << "\nController::init";
    if (update(in_params) == false) return false;

    // I.) load m2qt lib ...
    m_p_m2qt = M2QtHelper::getM2Qt(in_params);
    if (m_p_m2qt == nullptr) return false;

    // II.) create and connect signal agent for m2qt ...
    m_p_signal_agent = M2QtHelper::getSignalAgent();
    if (m_p_signal_agent == nullptr) return false;

    connect(m_p_signal_agent, &SignalAgent::signalError, this, &Controller::slotError);
    connect(m_p_signal_agent, &SignalAgent::signalWarning, this, &Controller::slotWarning);
    connect(m_p_signal_agent, &SignalAgent::signalDebug, this, &Controller::slotDebug);
    connect(m_p_signal_agent, &SignalAgent::signalInfo, this, &Controller::slotInfo);

    m_p_m2qt->setSignalAgent(m_p_signal_agent);

    // III.) init global DriveStarter for user callback ...
    if (g_drive_starter.init(in_params) == false) return false;

    // IV.) create default handler and user handler ...
    QVariantMap handler_params;
    // debug output handler (default callback)
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("debug_output");
    if (createHandler(QLatin1String("debug_output"), handler_params) == false) return false;
    // websocket handshake handler (default callback)
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("websocket_handshake");
    if (createHandler(QLatin1String("websocket_handshake"), handler_params) == false) return false;
    // websocket close handler (default callback)
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("websocket_close");
    if (createHandler(QLatin1String("websocket_close"), handler_params) == false) return false;
    // websocket pong handler (default callback)
    handler_params.clear();
    handler_params[QLatin1String("default_callback")] = QLatin1String("websocket_pong");
    if (createHandler(QLatin1String("websocket_pong"), handler_params) == false) return false;
    // starter handler (user callback)
    handler_params.clear();
    HandlerCallback starter = callbackDriveStart;
    FilterCallback filter = callbackFilter;
    handler_params[QLatin1String("user_callback")] = QVariant::fromValue(starter);
    handler_params[QLatin1String("user_filter")] = QVariant::fromValue(filter);
    if (createHandler(QLatin1String("callbackDriveStart"), handler_params) == false) return false;

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
    //if (m_initialized == false) return false;
    if (in_name.isEmpty()) return false;

    bool created = m_p_m2qt->createHandler(in_name, in_params);

    if (created == false) qDebug() << "\nController::createHandler - handler creation failed ...";
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
