#include "drivestarter.h"
#include <QDebug>

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------
DriveStarter::DriveStarter(QObject *parent) : QObject(parent)
{

}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool DriveStarter::init(const QVariantMap &in_params)
{
    qDebug() << "\nDriveStarter::init";
    if (update(in_params) == false) return false;

    connect(&m_drive_process, &QProcess::started, this, &DriveStarter::signalProcessStarted);
    connect(&m_drive_process, &QProcess::stateChanged, this, &DriveStarter::signalProcessStateChanged);

    // TODO: problem with obsolete signals and overloaded functions ...
    //connect(&m_drive_process, &QProcess::finished, this, &DriveStarter::signalProcessFinished);
    //connect(&m_drive_process, &QProcess::error, this, &DriveStarter::signalProcessError);

    // FIX: use SIGNAL and SLOT macros ...
    connect(&m_drive_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(signalProcessFinished(int, QProcess::ExitStatus)));
    connect(&m_drive_process, SIGNAL(error(QProcess::ProcessError)), this, SIGNAL(signalProcessError(QProcess::ProcessError)));

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void DriveStarter::cleanup()
{

}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool DriveStarter::update(const QVariantMap &in_params)
{
    if (in_params.contains("app_dir"))
    {
        QVariant var_app_dir = in_params["app_dir"];
        if (var_app_dir.isValid() == false) return false;
        m_drive_path = var_app_dir.value<QDir>();
        qDebug() << "DriveStarter::update - app_dir:" << m_drive_path.absolutePath();
    }

    if (in_params.contains("app_file"))
    {
        QVariant var_app_file = in_params["app_file"];
        if (var_app_file.isValid() == false) return false;
        m_drive_exe = var_app_file.value<QString>();
        qDebug() << "DriveStarter::update - app_file:" << m_drive_exe;
    }

    if (QFileInfo(m_drive_path, m_drive_exe).isExecutable() == false) return false;

    return true;
}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool DriveStarter::isValid()
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// slotStart
// ----------------------------------------------------------------------------
void DriveStarter::slotStart(const QStringList in_arguments, const bool in_native_args)
{
    if (m_initialized == false) return;

    if (in_native_args == false)
        m_drive_process.start(m_drive_path.absoluteFilePath(m_drive_exe), in_arguments);
    else
    {
        m_drive_process.setNativeArguments(in_arguments.join(' '));
        m_drive_process.setProgram(m_drive_path.absoluteFilePath(m_drive_exe));
        m_drive_process.start();
    }
}

// ----------------------------------------------------------------------------
// slotTerminate
// ----------------------------------------------------------------------------
void DriveStarter::slotTerminate()
{
    if (m_initialized == false) return;
    m_drive_process.terminate();
}

// ----------------------------------------------------------------------------
// slotKill
// ----------------------------------------------------------------------------
void DriveStarter::slotKill()
{
    if (m_initialized == false) return;
    m_drive_process.kill();
}

// ----------------------------------------------------------------------------
// Properties ...
// ----------------------------------------------------------------------------
QDir DriveStarter::drivePath() const
{
    return m_drive_path;
}

QString DriveStarter::driveExe() const
{
    return m_drive_exe;
}

QProcess::ProcessState DriveStarter::state() const
{
    return m_drive_process.state();
}
