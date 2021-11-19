/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

bool VariablesExtended::isUpdated(const QSet<QString> &vars_, IVariables *base_) const
{
    return m_extend ?
                m_extend->isUpdated(vars_, base_)
              : m_base ?
                    m_base->isUpdated(vars_, nullptr)
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

const QSet<QString> &IEffectCalculation::getRequiredVariables() const
{
    return m_vars;
}

void IEffectCalculation::setFilename(const QString &filename_)
{
    m_filename = filename_;
}

void IEffectCalculation::setRequiredVariables(const QStringList &vars_)
{
    for(const QString &v_ : vars_)
    {
        m_vars.insert(v_);
    }
}

QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_
        )
{
    QMatrix4x4 textureMatrix;
    const qreal imageRate = (qreal)std::max(1, imageSize_.width())
            / (qreal)std::max(1, imageSize_.height())
            ;
    const qreal rectRate = (qreal)std::max(1, rectSize_.width())
            / (qreal)std::max(1, rectSize_.height())
            ;
    if(rectRate >= imageRate)
    {
        textureMatrix.scale(rectRate/imageRate, 1.0);
        textureMatrix.translate(-(rectRate - imageRate)/rectRate*0.5, 0.0);
    }
    else
    {
        textureMatrix.scale(1.0, imageRate/rectRate);
        textureMatrix.translate(0.0, -(imageRate - rectRate)/imageRate*0.5);
    }
    return textureMatrix;
}

}
