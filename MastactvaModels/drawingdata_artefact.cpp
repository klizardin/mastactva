#include "drawingdata_artefact.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefactarg.h"


DrawingDataArtefact::DrawingDataArtefact(ArtefactData &&data_)
    : ArtefactData(std::move(data_))
{
    drawingdata::utils::rebuild(m_artefactArgData, static_cast<DrawingDataArtefactArg *>(nullptr));
}

ArtefactData * DrawingDataArtefact::getBase()
{
    return this;
}
