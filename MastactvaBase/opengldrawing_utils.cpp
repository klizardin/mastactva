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

#include <math.h>
#include <QFileInfo>


namespace opengl_drawing
{

const IVariables * IVariables::getRoot() const
{
    return this;
}

IVariables * IVariables::getRoot()
{
    return this;
}


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

const IVariables * VariablesExtended::getRoot() const
{
    return m_extend ? m_extend : m_base;
}

IVariables * VariablesExtended::getRoot()
{
    return m_extend ? m_extend : m_base;
}


const QString &IEffectCalculation::getFilename() const
{
    return m_filename;
}

bool IEffectCalculation::doExtend(const IEffectCalculation *calculation_) const
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

bool IEffectCalculation::doNeedUpdate() const
{
    return m_needUpdate;
}

void IEffectCalculation::setUpdated()
{
    m_needUpdate = false;
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
    const float iw = std::max(1.0f, (float)imageSize_.width());
    const float ih = std::max(1.0f, (float)imageSize_.height());
    const float imageRate = iw / ih;
    const float sw = std::max(1.0f, (float)rectSize_.width());
    const float sh = std::max(1.0f, (float)rectSize_.height());
    const float rectRate = sw / sh;

    //qDebug() << "iw" << iw << "ih" << ih << "sw" << sw << "sh" << sh << "imageRate" << imageRate << "rectRate" << rectRate;

    if(rectRate >= 1.0)
    {
        const float scaleX = 1.0f;
        const float scaleY = rectRate;
        textureMatrix.scale( scaleX, scaleY);
    }
    else
    {
        const float scaleX = 1.0f / rectRate;
        const float scaleY = 1.0f;
        textureMatrix.scale( scaleX, scaleY);
    }
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 1.0f;
    float y1 = 1.0f;

    if(rectRate >= imageRate)
    {
        const float shift = (1.0 - imageRate/rectRate) * 0.5f;
        //qDebug() << "shift" << shift;
        x0 += shift;
        x1 -= shift;
    }
    else
    {
        const float shift = (1.0 - rectRate/imageRate) * 0.5f;
        //qDebug() << "shift" << shift;
        y0 += shift;
        y1 -= shift;
    }
    const QVector<QVector2D> srcPts =
    {
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f}
    };
    const QVector<QVector2D> destPts =
    {
        {x0, y0},
        {x1, y1},
        {x0, y1},
        {x1, y0}
    };
    const QMatrix4x4 m1 = calculateTransfromMatrixBy4Points(destPts, srcPts);
    //qDebug() << "m1" << m1 << "destPts" << destPts;
    return m1 * textureMatrix;
}

QMatrix4x4 calculateTransfromMatrixBy4Points(
        const QVector<QVector2D> &srcPts_,
        const QVector<QVector2D> &destPts_
        )
{
    if(srcPts_.size() < 4 || destPts_.size() < 4)
    {
        return {};
    }
    if(fabs(srcPts_[0].y() - srcPts_[1].y()) < 1e-4
            || fabs(srcPts_[2].y() - srcPts_[3].y()) < 1e-4)
    {
        return {};  // reoder pts data, to make cross of lines (p0,p1) and (p2,p3)
    }
    const float sx1 = (destPts_[0].x() - destPts_[1].x())/(srcPts_[0].y() - srcPts_[1].y());
    const float sx2 = (destPts_[2].x() - destPts_[3].x())/(srcPts_[2].y() - srcPts_[3].y());
    const float sx3 = (srcPts_[0].x() - srcPts_[1].x())/(srcPts_[0].y() - srcPts_[1].y());
    const float sx4 = (srcPts_[2].x() - srcPts_[3].x())/(srcPts_[2].y() - srcPts_[3].y());
    const float sx = fabs(sx3 - sx4) < 1e-4 ? 1.0f : (sx1 - sx2)/(sx3 - sx4);
    const float kx = sx1 - sx3*sx;
    const float tx = destPts_[0].x() - srcPts_[0].x()*sx - srcPts_[0].y()*kx;

    if(fabs(srcPts_[0].x() - srcPts_[1].x()) < 1e-4
            || fabs(srcPts_[2].x() - srcPts_[3].x()) < 1e-4)
    {
        return {};  // reoder pts data, to make cross of lines (p0,p1) and (p2,p3)
    }
    const float sy1 = (destPts_[0].y() - destPts_[1].y())/(srcPts_[0].x() - srcPts_[1].x());
    const float sy2 = (destPts_[2].y() - destPts_[3].y())/(srcPts_[2].x() - srcPts_[3].x());
    const float sy3 = (srcPts_[0].y() - srcPts_[1].y())/(srcPts_[0].x() - srcPts_[1].x());
    const float sy4 = (srcPts_[2].y() - srcPts_[3].y())/(srcPts_[2].x() - srcPts_[3].x());
    const float sy = fabs(sy3 - sy4) < 1e-4 ? 1.0f : (sy1 - sy2)/(sy3 - sy4);
    const float ky = sy1 - sy3*sy;
    const float ty = destPts_[0].y() - srcPts_[0].x()*ky - srcPts_[0].y()*sy;

    return QMatrix4x4(sx,   kx,     0.0f,   tx,
                      ky,   sy,     0.0f,   ty,
                      0.0f, 0.0f,   1.0f,   0.0f,
                      0.0f, 0.0f,   0.0f,   1.0f);
}

}
