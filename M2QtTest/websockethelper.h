#ifndef WEBSOCKETHELPER_H
#define WEBSOCKETHELPER_H

#include <QObject>
#include <QVariant>
#include <QWebSocket>

// ----------------------------------------------------------------------------
//
// class WebSocketHelper
//
// ----------------------------------------------------------------------------
class WebSocketHelper final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString msg_template READ msgTemplate WRITE setMsgTemplate NOTIFY signalMsgTemplateChanged)
public:
    explicit WebSocketHelper(QObject* in_parent = nullptr);

    // the fabulous four ... :-)
    bool init(const QVariantMap &in_params);
    void cleanup();
    bool update(const QVariantMap &in_params);
    bool isValid() const;

    void connect(const QUrl &in_url);
    void send(const QByteArray &in_type, const QByteArray &in_data);

    // properties ...
    QString msgTemplate() const;

signals:
    void signalConnected();
    void signalError(const QString &error);

    // properties ...
    void signalMsgTemplateChanged(QString msg_template);

public slots:
    void slotOnConnected();
    void slotOnDisconnected();
    void slotOnStateChanged(QAbstractSocket::SocketState state);
    void slotOnError(QAbstractSocket::SocketError error);
    void slotOnTextMessage(const QString &message);

    // properties ...
    void setMsgTemplate(QString msg_template);

private:
    bool m_initialized = false;
    bool m_ws_connected = false;
    QWebSocket* m_p_web_sock = nullptr;
    QString m_msg_template;
};
#endif // WEBSOCKETHELPER_H
