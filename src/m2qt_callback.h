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
// class DefaultCallbacks
//
// ----------------------------------------------------------------------------
class DefaultCallbacks final : public QObject
{
    Q_OBJECT
public:
    explicit DefaultCallbacks(QObject *parent = 0);
    ~DefaultCallbacks() = default;
    DefaultCallbacks(const DefaultCallbacks& other) = delete;
    DefaultCallbacks& operator= (const DefaultCallbacks& other) = delete;

    static QVector<HandlerCallback> getCallbacks(const QStringList &in_names);

signals:
    void signalError(QString error) const;

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
