#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QVariant>
#include <QMap>

#include <m2qt.h>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);

    bool init(const QVariantMap& in_params);
    void cleanup();
    bool update(const QVariantMap& in_params);
    bool isValid() const;

signals:

public slots:
    void startHandler(const QString &in_name = QString());

private:
    bool m_initialized = false;
    M2QT::IM2Qt* m_p_m2qt;
};

#endif // CONTROLLER_H
