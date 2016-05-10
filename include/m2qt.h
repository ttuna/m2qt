#ifndef M2QT_H
#define M2QT_H

#include <QObject>
#include <QMap>
#include <QtCore/qglobal.h>

#if defined(Q_OS_WIN)
#  if !defined(M2QTSHARED_EXPORT) && !defined(M2QTSHARED_IMPORT)
#    define M2QTSHARED_EXPORT
#  elif defined(M2QTSHARED_IMPORT)
#    if defined(M2QTSHARED_EXPORT)
#      undef M2QTSHARED_EXPORT
#    endif
#    define M2QTSHARED_EXPORT Q_DECL_IMPORT
#  elif defined(M2QTSHARED_EXPORT)
#    undef M2QTSHARED_EXPORT
#    define M2QTSHARED_EXPORT Q_DECL_EXPORT
#  endif
#else
#  define M2QTSHARED_EXPORT
#endif

namespace M2QT {

using NetString = std::tuple<quint32, QByteArray>;
using Message = std::tuple<QString, QString, QString, QJsonObject, QList<NetString>>;
typedef void (*IM2QtHandlerCallback)(const Message&);

// ----------------------------------------------------------------------------
//
// class IM2QtHandler
//
// ----------------------------------------------------------------------------
class M2QTSHARED_EXPORT IM2QtHandler : public QObject
{
public:
    virtual ~IM2QtHandler() = default;

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void registerCallback(const QString &in_name, IM2QtHandlerCallback in_callback) = 0;

signals:
};

// ----------------------------------------------------------------------------
//
// class IM2Qt
//
// ----------------------------------------------------------------------------
class M2QTSHARED_EXPORT IM2Qt : public QObject
{
public:
    virtual ~IM2Qt() = default;
    virtual bool isValid() const = 0;
    virtual IM2QtHandler* createM2QtHandler(const QString& in_name, const QVariantMap &in_params) = 0;
    virtual IM2QtHandler* getM2QtHandler(const QString& in_name) const = 0;
    virtual void startHandler(const QString& in_name = QString()) const = 0;
};

// ----------------------------------------------------------------------------
//
// class M2QtLoader
//
// ----------------------------------------------------------------------------
class M2QtLoader final
{
public:
    virtual ~M2QtLoader() = default;
    explicit M2QtLoader() = delete;
    M2QtLoader(const M2QtLoader& other) = delete;
    M2QtLoader& operator= (const M2QtLoader& other) = delete;

    M2QTSHARED_EXPORT static M2QT::IM2Qt *getM2Qt();
};

} // namespace M2QT

#endif // M2QT_H
