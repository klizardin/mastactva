#ifndef DRAWINGDATA_ARTEFACT_H
#define DRAWINGDATA_ARTEFACT_H


#include "../MastactvaModels/artefact_data.h"


class DrawingDataArtefact : private ArtefactData
{
public:
    DrawingDataArtefact() = default;
    DrawingDataArtefact(ArtefactData &&data_);

    ArtefactData * getBase();
};


#endif // DRAWINGDATA_ARTEFACT_H
