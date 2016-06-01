#ifndef DRIVESTARTER_H
#define DRIVESTARTER_H

#include <QObject>
#include <QProcess>
#include <QDir>
#include <QVariant>

#include "global.h"

class DriveStarter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDir drive_path READ drivePath)
    Q_PROPERTY(QString drive_exe READ driveExe)
    Q_PROPERTY(QProcess::ProcessState state READ state)
public:
    explicit DriveStarter(QObject *parent = 0);

    bool init(const QVariantMap& in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid();

    QDir drivePath() const;
    QString driveExe() const;
    QProcess::ProcessState state() const;

signals:
    void signalProcessStarted();
    void signalProcessStateChanged(QProcess::ProcessState new_state);
    void signalProcessFinished(int exit_code, QProcess::ExitStatus exit_status);
    void signalProcessError(QProcess::ProcessError error);

public slots:
    void slotStart(const QStringList in_arguments, const bool in_native_args = false);
    void slotTerminate();
    void slotKill();

private:
    Q_DISABLE_COPY(DriveStarter)

    bool m_initialized = false;
    QDir m_drive_path = QDir("");
    QString m_drive_exe = QLatin1String("AVLDrive.exe");
    QProcess m_drive_process;
};

#endif // DRIVESTARTER_H
