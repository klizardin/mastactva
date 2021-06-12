#ifndef DRAWINGDATA_ARTEFACT_H
#define DRAWINGDATA_ARTEFACT_H


#include "../MastactvaModels/artefact_data.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"


class IDrawingDataArtefact
{
public:
    virtual ~IDrawingDataArtefact() = default;
    virtual bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addArguments(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addTexture(
            drawing_data::QuizImageObject &object_
            ) const = 0;
};


class DrawingDataArtefact :
        public ArtefactData,
        public IDrawingDataArtefact
{
public:
    DrawingDataArtefact() = default;
    DrawingDataArtefact(ArtefactData &&data_);

    bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addArguments(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addTexture(
            drawing_data::QuizImageObject &object_
            ) const override;
};


#endif // DRAWINGDATA_ARTEFACT_H
