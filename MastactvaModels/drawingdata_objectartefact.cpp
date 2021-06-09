#include "drawingdata_objectartefact.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefact.h"


DrawingDataObjectArtefact::DrawingDataObjectArtefact(ObjectArtefactData &&data_)
    : ObjectArtefactData(std::move(data_))
{
    drawingdata::utils::rebuild(m_artefactData, static_cast<DrawingDataArtefact *>(nullptr));
}

ObjectArtefactData * DrawingDataObjectArtefact::getBase()
{
    return this;
}
