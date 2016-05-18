#ifndef M2QT_DEFAULTCALLBACKS_H
#define M2QT_DEFAULTCALLBACKS_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QString>
#include <QMap>

namespace M2QT {

class CallbackHelper;

// ----------------------------------------------------------------------------
//
// class DefaultCallbackManager
//
// ----------------------------------------------------------------------------
class DefaultCallbackManager final : public QObject
{
    Q_OBJECT
public:
    explicit DefaultCallbackManager(QObject *parent = 0);
    ~DefaultCallbackManager() = default;
    DefaultCallbackManager(const DefaultCallbackManager& other) = delete;
    DefaultCallbackManager& operator= (const DefaultCallbackManager& other) = delete;

    static HandlerCallback getCallback(const QString &in_name);

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

public slots:

private:
    CallbackHelper* m_p_static_cb_helper = nullptr;
    static QMap<QString, HandlerCallback> m_callback_map;
};

// ----------------------------------------------------------------------------
//
// class CallbackHelper
//
// ----------------------------------------------------------------------------
class CallbackHelper final : public QObject
{
    Q_OBJECT
public:
    CallbackHelper() = default;
    ~CallbackHelper() = default;
    CallbackHelper(const CallbackHelper& other) = delete;
    CallbackHelper& operator= (const CallbackHelper& other) = delete;

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

public slots:

private:

};

} // namespace

#endif // M2QT_DEFAULTCALLBACKS_H
