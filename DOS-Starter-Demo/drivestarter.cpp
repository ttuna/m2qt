#include "drivestarter.h"
#include <QDebug>

DriveStarter::DriveStarter(QObject *parent) : QObject(parent)
{

}
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
bool DriveStarter::init(const QDir &in_drive_path, const QString &in_drive_exe)
{
    qDebug() << "DriveStarter::init" << in_drive_path << in_drive_exe;
    if (in_drive_path.exists() == false) return false;
    if (in_drive_exe.isEmpty()) return false;
    if (QFile::exists(in_drive_path.absoluteFilePath(in_drive_exe)) == false) return false;

    m_drive_path = in_drive_path;
    m_drive_exe = in_drive_exe;

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
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
bool DriveStarter::isValid()
{
    return m_initialized;
}
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
void DriveStarter::slotStart(const QStringList in_arguments)
{
    if (m_initialized == false) return;
    m_drive_process.start(m_drive_path.absoluteFilePath(m_drive_exe), in_arguments);
}
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
void DriveStarter::slotTerminate()
{
    if (m_initialized == false) return;
    m_drive_process.terminate();
}
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
void DriveStarter::slotKill()
{
    if (m_initialized == false) return;
    m_drive_process.kill();
}
//////////////////////////////////////////////////////////////////////////
///
///
//////////////////////////////////////////////////////////////////////////
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
