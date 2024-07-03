#ifndef UTILS_SCALEDTIME_H
#define UTILS_SCALEDTIME_H


#include <QVariant>
#include <qvector.h>


class ScaledTime
{
public:
    ScaledTime() = default;

    QVariantList getVectorValue() const;
    void setVectorValue(const QVariantList& delaysVector);
    int getCycles() const;
    void setCycles(int cycles);
    int getCycle() const;
    qreal get(qreal dt_, qreal& tIntermediate_);

private:
    qreal get(qreal dt_, int& delayIndex_, qreal& tIntermediate_);

private:
    QVector<qreal> m_tScalesVector;
    int m_currentTScalesVectorIndex = 0;
    qreal m_lastTime = 0.0;
    int m_cycle = 0;
    int m_cycles = -1;
};



#endif // UTILS_SCALEDTIME_H
