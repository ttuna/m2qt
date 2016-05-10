#ifndef M2QT_H
#define M2QT_H

#include <QObject>

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

namespace zmq {
class context_t;
}

namespace M2QT {

// ----------------------------------------------------------------------------
//
// class IM2Qt
//
// ----------------------------------------------------------------------------
class M2QTSHARED_EXPORT IM2Qt : public QObject
{
public:
    virtual ~IM2Qt() = default;

public slots:
    virtual void cleanup() = 0;
    virtual bool init(zmq::context_t* in_ctx, const QVariantMap &in_params) = 0;
    virtual bool isValid() const = 0;
    virtual void receive() = 0;
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

    M2QTSHARED_EXPORT static IM2Qt& load();
};

} // namespace M2QT

#endif // M2QT_H
