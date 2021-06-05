#ifndef OBJECTARTEFACT_DATA_H
#define OBJECTARTEFACT_DATA_H


struct ObjectArtefactData
{
    virtual ~ObjectArtefactData() = default;

    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    int m_stepIndex = -1;
};


#endif // OBJECTARTEFACT_DATA_H
