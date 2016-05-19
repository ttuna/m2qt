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
// class CallbackManager
//
// ----------------------------------------------------------------------------
class CallbackManager final : public QObject
{
    Q_OBJECT
public:
    explicit CallbackManager(QObject *parent = 0);
    ~CallbackManager() = default;
    CallbackManager(const CallbackManager& other) = delete;
    CallbackManager& operator= (const CallbackManager& other) = delete;

    static HandlerCallback getHandlerCallback(const QString &in_name);
    static FilterCallback getFilterCallback(const QString &in_name);

signals:
    void signalError(QString error) const;  // TODO: use it ..
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

public slots:

private:
    CallbackHelper* m_p_static_cb_helper = nullptr;
    static QMap<QString, HandlerCallback> m_handler_callback_map;
    static QMap<QString, FilterCallback> m_filter_callback_map;
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
