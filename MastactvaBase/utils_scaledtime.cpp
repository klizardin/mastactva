#include "utils_scaledtime.h"
#include "utils.h"


QVariantList ScalledTime::getVectorValue() const
{
    QVariantList result;
    result.reserve(m_tScalesVector.size());
    for(const auto& val: m_tScalesVector)
    {
        result << QVariant::fromValue(val);
    }
    return result;
}

void ScalledTime::setVectorValue(const QVariantList& delaysVector)
{
    m_tScalesVector.resize(delaysVector.length());
    int i = 0;
    for(const QVariant& val: delaysVector)
    {
        m_tScalesVector[i] = val.toReal();
        ++i;
    }
    m_currentTScalesVectorIndex = 0;
}

int ScalledTime::getCycles() const
{
    return m_cycles;
}

void ScalledTime::setCycles(int cycles)
{
    m_cycles = cycles;
    m_cycle = 0;
}

qreal ScalledTime::get(qreal dt_, qreal& tIntermediate_)
{
    return get(dt_, m_currentTScalesVectorIndex, tIntermediate_);
}

qreal ScalledTime::get(qreal dt_, int& delayIndex_, qreal& tIntermediate_)
{
    delayIndex_ = std::max(0, delayIndex_);
    while(dt_ >= 0.0 && delayIndex_ + 2 < (int)m_tScalesVector.size())
    {
        const int oldDelayIndex = delayIndex_;
        const qreal currentDT = fabs(m_tScalesVector[oldDelayIndex + 1]);
        if(dt_ < currentDT)
        {
            const qreal startT = m_tScalesVector[oldDelayIndex];
            const qreal endT = m_tScalesVector[oldDelayIndex + 2];
            if(currentDT > 0)
            {
                if(startT <= endT)
                {
                    return std::clamp(dt_/currentDT*sign(endT-startT) + startT, startT, endT);
                }
                else
                {
                    return std::clamp(dt_/currentDT*sign(endT-startT) + startT, endT, startT);
                }
            }
            else
            {
                return endT;
            }
        }
        else
        {
            dt_ -= currentDT;
            tIntermediate_ += currentDT;
            delayIndex_ += 2;
        }
    }
    tIntermediate_ += dt_;
    return !m_tScalesVector.empty() ? m_tScalesVector.back() : 1.0;
}

