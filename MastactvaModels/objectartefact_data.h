#ifndef OBJECTARTEFACT_DATA_H
#define OBJECTARTEFACT_DATA_H


#include <memory>
#include <QVector>
#include "../MastactvaModels/artefact_data.h"


struct ObjectArtefactData
{
    ObjectArtefactData();
    ObjectArtefactData(ObjectArtefactData &&data_);
    virtual ~ObjectArtefactData() = default;

    ObjectArtefactData &operator = (ObjectArtefactData &&data_);

    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    int m_stepIndex = -1;

    std::shared_ptr<QVector<ArtefactData *>> m_artefactData;

    ObjectArtefactData(const ObjectArtefactData &) = delete;
    ObjectArtefactData & operator = (const ObjectArtefactData &) = delete;
};


#endif // OBJECTARTEFACT_DATA_H
