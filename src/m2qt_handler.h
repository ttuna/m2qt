#ifndef M2QTHANDLER_H
#define M2QTHANDLER_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QThread>
#include <QVariant>
#include <QSharedPointer>

#include <zmq.hpp>

namespace M2QT {

class IM2Qt;
class ServerConnection;
class MessageParser;

class Handler final : public IM2QtHandler
{
    Q_OBJECT
    Q_PROPERTY(zmq::context_t* p_zmq_ctx READ zmqCtx WRITE setZmqCtx NOTIFY signalZmqCtxChanged)
    Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY signalParamsChanged)
public:
    explicit Handler();
    ~Handler();

    bool init(zmq::context_t* in_ctx, const QVariantMap& in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid() const;

    // properties ...
    zmq::context_t* zmqCtx() const;
    QVariantMap params() const;

public slots:
    void start();
    void stop();
    void registerCallback(const QString &in_name, IM2QtHandlerCallback in_callback);

    // properties ...
    void setZmqCtx(zmq::context_t* p_zmq_ctx);
    void setParams(QVariantMap params);

private slots:
    void handleParserResults(const Message& in_data);

signals:
    void signalStarted();
    void signalStopped();

    // properties ...
    void signalZmqCtxChanged(zmq::context_t* p_zmq_ctx);
    void signalParamsChanged(QVariantMap params);

private:
    bool m_initialized = false;
    QSharedPointer<ServerConnection> m_p_server_con;
    QSharedPointer<MessageParser> m_p_parser;
    IM2QtHandlerCallback m_callback;
    zmq::context_t* m_p_zmq_ctx;
    QVariantMap m_params;
};

} // namespace

#endif // M2QTHANDLER_H
