#ifndef M2QT_H
#define M2QT_H

#include <QObject>
#include <QVariant>
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
enum NetStringIdx {NS_SIZE=0, NS_DATA};

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
// class SignalAgent
//
// ----------------------------------------------------------------------------
class M2QTSHARED_EXPORT SignalAgent final : public QObject
{
    Q_OBJECT
public:
    explicit SignalAgent(QObject* in_parent = nullptr);
    ~SignalAgent() = default;
    SignalAgent(const SignalAgent& other) = delete;
    SignalAgent& operator= (const SignalAgent& other) = delete;

signals:
    void signalError(QString error) const;
    void signalWarning(QString warning) const;
    void signalDebug(QString debug) const;
    void signalInfo(QString info) const;

private:
    bool m_dummy = false;
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
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setHandlerCallback(const QString &in_handler_name, const QString &in_callback_name, HandlerCallback in_callback) const = 0;
    virtual void setSignalAgent(SignalAgent* in_signal_agent) = 0;

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

    M2QTSHARED_EXPORT static M2QT::IM2Qt *getM2Qt(const QVariantMap &in_params = QVariantMap());
    M2QTSHARED_EXPORT static M2QT::SignalAgent *getSignalAgent();
};

} // namespace M2QT

#endif // M2QT_H
