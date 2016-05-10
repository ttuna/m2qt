#ifndef M2QTAPPLICATION_H
#define M2QTAPPLICATION_H

#include <QCoreApplication>
#include <QVariantMap>

class M2QtApplication : public QCoreApplication
{
    Q_OBJECT
public:
    explicit M2QtApplication(int &argc, char **argv);

    bool init(const QVariantMap& in_params);
    bool update(const QVariantMap& in_params);
    bool isValid() const;

private:
    bool m_initialized = false;
};

#endif // M2QTAPPLICATION_H
