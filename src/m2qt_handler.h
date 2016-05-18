#ifndef M2QTHANDLER_H
#define M2QTHANDLER_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QSharedPointer>

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
    Q_PROPERTY(QByteArray msg_prefix READ msgPrefix WRITE setMsgPrefix NOTIFY signalMsgPrefixChanged)

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
    QString callback() const;
    QByteArray msgPrefix() const;

public slots:
    bool setCallback(HandlerCallback in_callback);
    void handleParserResults(const Request& in_req);

    // properties ...
    void setMsgPrefix(QByteArray msg_prefix);

private slots:
    void updateDefCallbacks(QString callback);

signals:
    void signalError(QString error) const; // TODO: use it ..
    void signalSendMsg(QByteArray msg) const;

    // properties ...
    void signalMsgPrefixChanged(QByteArray msg_prefix);

private:
    bool m_initialized = false;
    QString m_name;
    HandlerCallback m_callback;
    QByteArray m_msg_prefix;    // TODO: use it ...
};

} // namespace

#endif // M2QTHANDLER_H
