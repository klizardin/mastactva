#ifndef DRAWINGDATA_OBJECTARTEFACT_H
#define DRAWINGDATA_OBJECTARTEFACT_H


#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaModels/drawingdata_artefact.h"


class DrawingDataObjectArtefact :
        public ObjectArtefactData,
        public IDrawingDataArtefact
{
public:
    DrawingDataObjectArtefact() = default;
    DrawingDataObjectArtefact(ObjectArtefactData &&data_);

    int getStepIndex() const;

    bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addArguments(
            drawing_data::QuizImageObject &object_
            ) const override;
    void addTexture(
            drawing_data::QuizImageObject &object_
            ) const override;

    friend bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_);

private:
    bool forArtefacts(std::function<bool(const DrawingDataArtefact *)> func_) const;
};

bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_);

#endif // DRAWINGDATA_OBJECTARTEFACT_H
