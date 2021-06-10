#include "drawingdata_artefact.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefactarg.h"


DrawingDataArtefact::DrawingDataArtefact(ArtefactData &&data_)
    : ArtefactData(std::move(data_))
{
    drawingdata::utils::rebuild(m_artefactArgData, static_cast<DrawingDataArtefactArg *>(nullptr));
}

ArtefactData * DrawingDataArtefact::getData()
{
    return this;
}

bool DrawingDataArtefact::setVertexShader(
        drawing_data::QuizImageObject &object_,
        drawingdata::IFileSource *filesource_
        ) const
{
    Q_UNUSED(object_);
    Q_UNUSED(filesource_);
    return false;
}

bool DrawingDataArtefact::setFragmentShader(
        drawing_data::QuizImageObject &object_,
        drawingdata::IFileSource *filesource_
        ) const
{
    Q_UNUSED(object_);
    Q_UNUSED(filesource_);
    return false;
}

void DrawingDataArtefact::addArguments(
        drawing_data::QuizImageObject &object_
        ) const
{
    Q_UNUSED(object_);
}
