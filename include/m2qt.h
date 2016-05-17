#ifndef M2QT_H
#define M2QT_H

#include <QObject>
#include <QMap>
#include <QVector>
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

// Mongrel2 NetString   // size | data
using NetString = std::tuple<quint32, QByteArray>;
enum NetStringIdx {SIZE=0, DATA};

// Mongrel2 Request     // uuid | id | path | netstrings
using Request = std::tuple<QByteArray, QByteArray, QByteArray, QVector<NetString>>;
enum RequestIdx {REQ_UUID=0, REQ_ID, REQ_PATH, REQ_NETSTRINGS};

// TODO: change second item to NetString ...
// Mongrel2 Response    // uuid | (size(id):id) | body
using Response = std::tuple<QByteArray, QByteArray, QByteArray>;
enum ResponseIdx {REP_UUID=0, REP_ID, REP_BODY};

// Prototye of handler callback ...
typedef Response (*HandlerCallback)(const Request&);

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
//
// class M2QtSignalAgent
//
// ----------------------------------------------------------------------------
class M2QTSHARED_EXPORT M2QtSignalAgent final : public QObject
{
    Q_OBJECT
public:
    M2QtSignalAgent() = default;
    ~M2QtSignalAgent() = default;
    M2QtSignalAgent(const M2QtSignalAgent& other) = delete;
    M2QtSignalAgent& operator= (const M2QtSignalAgent& other) = delete;

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;
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
    virtual bool createHandler(const QString& in_name, const QVariantMap &in_params) = 0;
    virtual void startHandler(const QString& in_name = QString()) const = 0;
    virtual void stopHandler(const QString& in_name = QString()) const = 0;
    virtual void addHandlerCallback(const QString& in_callback_name, HandlerCallback in_callback) const = 0;
    virtual void setMsgPrefix(QByteArray msg_prefix) = 0;
    virtual void setSignalAgent(M2QtSignalAgent* in_signal_agent) = 0;

//signals:
    virtual void signalError(QString error) const = 0;
    virtual void signalWarning(QString warning) const = 0;
    virtual void signalDebug(QString debug) const = 0;
    virtual void signalInfo(QString info) const = 0;
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
