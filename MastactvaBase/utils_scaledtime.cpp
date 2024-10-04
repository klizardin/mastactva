#include "utils_scaledtime.h"
#include "utils.h"


QVariantList ScaledTime::getVectorValue() const
{
    QVariantList result;
    result.reserve(m_tScalesVector.size());
    for(const auto& val: m_tScalesVector)
    {
        result << QVariant::fromValue(val);
    }
    return result;
}

void ScaledTime::setVectorValue(const QVariantList& delaysVector)
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

int ScaledTime::getCycles() const
{
    return m_cycles;
}

void ScaledTime::setCycles(int cycles)
{
    m_cycles = cycles;
    m_cyclesCount = cycles;
    if(m_cycles > 0)
    {
        if(m_events)
        {
            m_events->onScaledTimeFirstCycle();
        }
    }
}

void ScaledTime::set(IScaledTimeEvents* events)
{
    m_events = events;
}

qreal ScaledTime::get(qreal dt_, qreal& tIntermediate_, bool pause_ /*= false*/)
{
    qreal result = get(dt_, m_currentTScalesVectorIndex, tIntermediate_, pause_);
    //qDebug() << result << m_currentTScalesVectorIndex << "\n";
    return result;
}

void ScaledTime::decCycles(int& delayIndex_)
{
    if(m_cycles > 0)
    {
        if(m_events)
        {
            m_events->onScaledTimeCycle(m_cyclesCount - m_cycles);
        }
        --m_cycles;
        delayIndex_ = 0;
    }
    else if(m_cycles < 0)
    {
        delayIndex_ = 0;
        m_events->onScaledTimeCycle(-1);
    }

    if(0 == m_cycles)
    {
        if(m_events)
        {
            m_events->onScaledTimeLastCycle();
            m_events->onScaledTimeFirstCycle();
        }
    }
}

qreal ScaledTime::get(qreal dt_, int& delayIndex_, qreal& tIntermediate_, bool pause_)
{
    if(pause_)
    {
        m_pauseTime += std::max(0.0, dt_ - m_lastTime);
        dt_ -= m_pauseTime;
    }
    else
    {
        m_lastTime = dt_;
    }
    delayIndex_ = std::max(0, delayIndex_);
    if(delayIndex_ + 2 >= (int)m_tScalesVector.size())
    {
        decCycles(delayIndex_);
    }
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
                    return std::clamp(dt_/currentDT*(endT-startT) + startT, startT, endT);
                }
                else
                {
                    return std::clamp(dt_/currentDT*(endT-startT) + startT, endT, startT);
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
    decCycles(delayIndex_);
    tIntermediate_ += dt_;
    return !m_tScalesVector.empty() ? m_tScalesVector.back() : 1.0;
}

