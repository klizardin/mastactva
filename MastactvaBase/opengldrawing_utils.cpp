#include "opengldrawing_utils.h"


#include <QFileInfo>


namespace opengl_drawing
{

const QString &IEffectCalculation::getFilename() const
{
    return m_filename;
}

bool IEffectCalculation::doExtend(IEffectCalculation *calculation_) const
{
    if(!calculation_)
    {
        return false;
    }
    QFileInfo fiCurrent(getFilename());
    QFileInfo fiNew(calculation_->getFilename());
    return fiCurrent.baseName().startsWith(fiNew.baseName());
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
