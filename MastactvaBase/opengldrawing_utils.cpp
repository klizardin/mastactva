#include "opengldrawing_utils.h"


namespace opengl_drawing
{

const QString &IEffectCalculation::getFilename() const
{
    return m_filename;
}

const QStringList &IEffectCalculation::getVariables() const
{
    return m_vars;
}

void IEffectCalculation::setFilename(const QString &filename_)
{
    m_filename = filename_;
}

void IEffectCalculation::setVariables(const QStringList &vars_)
{
    m_vars = vars_;
}

}
