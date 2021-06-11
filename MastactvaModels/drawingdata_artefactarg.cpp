#include "drawingdata_artefactarg.h"


DrawingDataArtefactArg::DrawingDataArtefactArg(ArtefactArgData &&data_)
    : ArtefactArgData(std::move(data_))
{
}
