#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QMap>
#include <QSharedPointer>

#include <m2qt.h>

class Controller final : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    ~Controller();

    bool init(const QVariantMap& in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid() const;

signals:

public slots:
    bool createHandler(const QString &in_name, const QVariantMap &in_params);
    void startHandler(const QString &in_name = QString());
    void stopHandler(const QString &in_name = QString());

private slots:
    void printMessage(const QString &in_msg);

private:
    bool m_initialized = false;
    M2QT::IM2Qt* m_p_m2qt;
    M2QT::M2QtSignalAgent* m_p_signal_agent = nullptr;
    QVector<QString> m_handler_names;
};

#endif // CONTROLLER_H
