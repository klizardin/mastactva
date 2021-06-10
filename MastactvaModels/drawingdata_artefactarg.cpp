#include "drawingdata_artefactarg.h"


DrawingDataArtefactArg::DrawingDataArtefactArg(ArtefactArgData &&data_)
    : ArtefactArgData(std::move(data_))
{
}

ArtefactArgData * DrawingDataArtefactArg::getData()
{
    return this;
}

void DrawingDataArtefactArg::addArgument(drawing_data::QuizImageObject &object_) const
{
    Q_UNUSED(object_);
}
