#ifndef DRAWINGDATA_ARTEFACTARG_H
#define DRAWINGDATA_ARTEFACTARG_H


#include "../MastactvaModels/artefactarg_data.h"
#include "../MastactvaBase/quizimagedrawingdata.h"


class IDrawingDataArtefactArg
{
public:
    virtual ~IDrawingDataArtefactArg() = default;
    virtual void addArgument(drawing_data::QuizImageObject &object_) const = 0;
};


class DrawingDataArtefactArg : private ArtefactArgData
{
public:
    DrawingDataArtefactArg() = default;
    DrawingDataArtefactArg(ArtefactArgData &&data_);

    ArtefactArgData * getData();
};


#endif // DRAWINGDATA_ARTEFACTARG_H
