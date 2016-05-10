#ifndef M2QTHANDLER_H
#define M2QTHANDLER_H

#include <QObject>
#include <QThread>
#include <QVariantMap>

#include <zmq.hpp>

namespace M2QT {
class IM2Qt;
}

class M2QtHandler final : public QObject
{
    Q_OBJECT
public:
    explicit M2QtHandler(QObject *parent = 0);
    ~M2QtHandler();

    void cleanup();
    bool init(const QVariantMap& in_params);
    bool update(const QVariantMap& in_params);
    bool isValid() const;

public slots:
    void recv();

private:
    bool m_initialized = false;
    zmq::context_t* m_p_zmq_ctx = nullptr;
    M2QT::IM2Qt* m_p_m2qt = nullptr;
};

#endif // M2QTHANDLER_H
