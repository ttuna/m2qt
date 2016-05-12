#ifndef M2QT_DEFAULTCALLBACKS_H
#define M2QT_DEFAULTCALLBACKS_H

#include "m2qt.h"
#include "m2qt_global.h"

#include <QObject>
#include <QString>
#include <QMap>

namespace M2QT {

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

public slots:

private:
    static QMap<QString, HandlerCallback> m_callback_map;
};

} // namespace

#endif // M2QT_DEFAULTCALLBACKS_H
