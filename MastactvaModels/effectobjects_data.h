#ifndef EFFECTOBJECTS_DATA_H
#define EFFECTOBJECTS_DATA_H


struct EffectObjectsData
{
    virtual ~EffectObjectsData() = default;

    int m_id = -1;
    int m_effectId = -1;
    int m_objectInfoId = -1;
    int m_stepIndex = -1;
};


#endif // EFFECTOBJECTS_DATA_H
