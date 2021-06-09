#ifndef DRAWINGDATA_OBJECTARTEFACT_H
#define DRAWINGDATA_OBJECTARTEFACT_H


#include "../MastactvaModels/objectartefact_data.h"


class DrawingDataObjectArtefact : private ObjectArtefactData
{
public:
    DrawingDataObjectArtefact() = default;
    DrawingDataObjectArtefact(ObjectArtefactData &&data_);

    ObjectArtefactData * getBase();
};


#endif // DRAWINGDATA_OBJECTARTEFACT_H
