﻿#include "controller.h"
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
// createHandler
// ----------------------------------------------------------------------------
bool Controller::createHandler(const QString &in_name, const QVariantMap &in_params)
{
    if (m_initialized == false) return false;
    if (in_name.isEmpty()) return false;

    bool created = m_p_m2qt->createHandler(in_name, in_params);

    if (created == false) qDebug() << "Controller::init handler creation failed ...";
    else m_handler_names.push_back(in_name);

    return created;
}

// ----------------------------------------------------------------------------
// startHandler
// ----------------------------------------------------------------------------
void Controller::startHandler(const QString &in_name /*=QString()*/)
{
    if (m_initialized == false) return;
    m_p_m2qt->startHandler(in_name);
}

// ----------------------------------------------------------------------------
// stopHandler
// ----------------------------------------------------------------------------
void Controller::stopHandler(const QString &in_name /*=QString()*/)
{
    if (m_initialized == false) return;
    m_p_m2qt->stopHandler(in_name);
}