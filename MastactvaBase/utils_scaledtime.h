#ifndef UTILS_SCALEDTIME_H
#define UTILS_SCALEDTIME_H


#include <QVariant>
#include <qvector.h>


struct IScaledTimeEvents
{
    virtual void onScaledTimeCycle(int cycle) const = 0;
    virtual void onScaledTimeLastCycle() const = 0;
};


class ScaledTime
{
public:
    ScaledTime() = default;

    QVariantList getVectorValue() const;
    void setVectorValue(const QVariantList& delaysVector);
    int getCycles() const;
    void setCycles(int cycles);
    qreal get(qreal dt_, qreal& tIntermediate_);
    void set(IScaledTimeEvents* events);

private:
    qreal get(qreal dt_, int& delayIndex_, qreal& tIntermediate_);
    void decCycles(int& delayIndex_);

private:
    QVector<qreal> m_tScalesVector;
    int m_currentTScalesVectorIndex = 0;
    qreal m_lastTime = 0.0;
    int m_cycles = 0;
    int m_cyclesCount = 0;
    IScaledTimeEvents* m_events = nullptr;
};



#endif // UTILS_SCALEDTIME_H
