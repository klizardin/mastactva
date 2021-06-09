#include "drawingdata_effectobjects.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_objectinfo.h"
#include "../MastactvaModels/drawingdata_objectartefact.h"


DrawingDataEffectObjects::DrawingDataEffectObjects(EffectObjectsData &&data_)
    : EffectObjectsData(std::move(data_))
{
    drawingdata::utils::rebuild(m_objectInfoData, static_cast<DrawingDataObjectInfo *>(nullptr));
    drawingdata::utils::rebuild(m_objectArtefactData, static_cast<DrawingDataObjectArtefact *>(nullptr));
}

EffectObjectsData * DrawingDataEffectObjects::getData()
{
    return this;
}

void DrawingDataEffectObjects::init(std::shared_ptr<drawingdata::IFileSource> filesources_)
{
    m_filesources = std::move(filesources_);
}

void DrawingDataEffectObjects::initialize(drawing_data::QuizImageObjects &data_) const
{
    Q_UNUSED(data_);
}
