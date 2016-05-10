#include "m2qtapplication.h"

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
M2QtApplication::M2QtApplication(int &argc, char **argv) : QCoreApplication(argc, argv)
{

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtApplication::init(const QVariantMap &in_params)
{
    if (update(in_params) == false) return false;

    m_initialized = true;
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtApplication::update(const QVariantMap& in_params)
{
    return true;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
bool M2QtApplication::isValid() const
{
    return m_initialized;
}
