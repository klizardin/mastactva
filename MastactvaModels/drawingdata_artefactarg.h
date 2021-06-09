#ifndef DRAWINGDATA_ARTEFACTARG_H
#define DRAWINGDATA_ARTEFACTARG_H


#include "../MastactvaModels/artefactarg_data.h"


class DrawingDataArtefactArg : private ArtefactArgData
{
public:
    DrawingDataArtefactArg() = default;
    DrawingDataArtefactArg(ArtefactArgData &&data_);

    ArtefactArgData * getBase();
};


#endif // DRAWINGDATA_ARTEFACTARG_H
