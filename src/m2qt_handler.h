#ifndef M2QTHANDLER_H
#define M2QTHANDLER_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QSharedPointer>
#include <QtConcurrent>

#include <zmq.hpp>

namespace M2QT {

class ServerConnection;
class MessageParser;

// ----------------------------------------------------------------------------
//
// class Handler
//
// ----------------------------------------------------------------------------
class Handler final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QStringList default_callbacks READ defaultCallbacks WRITE setDefaultCallbacks NOTIFY signalDefaultCallbacksChanged)
public:
    explicit Handler(QObject* parent = nullptr);
    ~Handler();
    Handler(const Handler& other) = delete;
    Handler& operator= (const Handler& other) = delete;

    bool init(const QString &in_name, zmq::context_t* in_zmq_ctx, const QVariantMap& in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid() const;

    // properties ...
    QString name() const;
    QStringList defaultCallbacks() const;

public slots:
    void start();
    void stop();
    bool registerCallback(const QString &in_name, HandlerCallback in_callback);

    // properties ...
    void setDefaultCallbacks(QStringList default_callbacks);

private slots:
    void handleParserResults(const Request& in_req);
    void updateDefCallbacks(QStringList default_callbacks);

signals:
    void signalError(QString error) const; // TODO: use it ..
    void signalStarted() const;
    void signalStopped() const;

    // properties ...
    void signalDefaultCallbacksChanged(QStringList default_callbacks);

private:
    using UserCallback = std::tuple<QString, HandlerCallback>;

    bool m_initialized = false;
    QString m_name;
    QSharedPointer<ServerConnection> m_p_server_con;
    QSharedPointer<MessageParser> m_p_parser;
    QStringList m_default_callbacks;
    QVector<HandlerCallback> m_def_callbacks;
    QVector<UserCallback> m_user_callbacks;
    QFutureWatcher<void> m_poll_watcher;
};

} // namespace

#endif // M2QTHANDLER_H
