#ifndef EFFECTOBJECTS_DATA_H
#define EFFECTOBJECTS_DATA_H


#include <memory>
#include <QVector>
#include "../MastactvaModels/objectinfo_data.h"
#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"


struct EffectObjectsData
{
public:
    EffectObjectsData();
    EffectObjectsData(
            int id_,
            int effectId_,
            int objectInfoId_,
            int stepIndex_
            );
    EffectObjectsData(EffectObjectsData &&data_);
    virtual ~EffectObjectsData() = default;

    EffectObjectsData & operator = (EffectObjectsData &&data_);

    // to extract data from model object
    std::unique_ptr<EffectObjectsData> copy() const;

    void addObjects(drawing_data::QuizImageObjects &data_, drawingdata::IFileSource *filesource_) const;

public:
    int m_id = -1;
    int m_effectId = -1;
    int m_objectInfoId = -1;
    int m_stepIndex = -1;

    std::shared_ptr<QVector<ObjectInfoData *>> m_objectInfoData;
    std::shared_ptr<QVector<ObjectArtefactData *>> m_objectArtefactData;

    EffectObjectsData(const EffectObjectsData &data_) = delete;
    EffectObjectsData & operator = (const EffectObjectsData &data_) = delete;
};


#endif // EFFECTOBJECTS_DATA_H
