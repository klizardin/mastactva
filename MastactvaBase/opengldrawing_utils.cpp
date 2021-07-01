#include "opengldrawing_utils.h"


#include <QFileInfo>


namespace opengl_drawing
{

VariablesExtended::VariablesExtended(IVariables *base_, IVariables *extend_)
    : m_base(base_),
      m_extend(extend_)
{
}

bool VariablesExtended::get(const QString &name_, QVector<double> &data_) const
{
    return m_base ?
                m_base->get(name_, data_)
              : m_extend ?
                    m_extend->get(name_, data_)
                  : false
                    ;
}

void VariablesExtended::set(const QString &name_, const QVector<double> &data_)
{
    m_base ?
        m_base->set(name_, data_)
        : m_extend ?
            m_extend->set(name_, data_)
          : static_cast<void>(nullptr)
            ;
}

void VariablesExtended::set(const QString &name_, QVector<double> &&data_)
{
    m_base ?
        m_base->set(name_, std::move(data_))
        : m_extend ?
            m_extend->set(name_, std::move(data_))
          : static_cast<void>(nullptr)
            ;
}

bool VariablesExtended::isUpdated(const QStringList &vars_) const
{
    return m_extend ?
                m_extend->isUpdated(vars_)
              : m_base ?
                    m_base->isUpdated(vars_)
                  : false
                    ;
}


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

const QStringList &IEffectCalculation::getRequiredVariables() const
{
    return m_vars;
}

void IEffectCalculation::setFilename(const QString &filename_)
{
    m_filename = filename_;
}

void IEffectCalculation::setRequiredVariables(const QStringList &vars_)
{
    m_vars = vars_;
}

}
