#include "controller.h"
#include <QDebug>

using namespace M2QT;

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
Controller::Controller(QObject *parent) : QObject(parent)
{

}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
bool Controller::init(const QVariantMap &in_params)
{
    if (update(in_params) == false) return false;

    m_p_m2qt = M2QtLoader::getM2Qt();
    if (m_p_m2qt == nullptr) return false;

    IM2QtHandler* web_socket_handler = m_p_m2qt->createM2QtHandler("web_socket_handler", in_params);
    if (web_socket_handler == nullptr) qDebug() << "handler == nullptr";

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
// cleanup
// ----------------------------------------------------------------------------
void Controller::cleanup()
{

}

// ----------------------------------------------------------------------------
// update
// ----------------------------------------------------------------------------
bool Controller::update(const QVariantMap &in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
// isValid
// ----------------------------------------------------------------------------
bool Controller::isValid() const
{
    return m_initialized;
}

// ----------------------------------------------------------------------------
// startHandler
// ----------------------------------------------------------------------------
void Controller::startHandler(const QString &in_name /*=QString()*/)
{
    if (m_initialized == false) return;
    m_p_m2qt->startHandler(in_name);
}
