#ifndef DRAWINGDATA_OBJECTARTEFACT_H
#define DRAWINGDATA_OBJECTARTEFACT_H


#include "../MastactvaModels/objectartefact_data.h"


class DrawingDataObjectArtefact : private ObjectArtefactData
{
public:
    DrawingDataObjectArtefact() = default;
    DrawingDataObjectArtefact(ObjectArtefactData &&data_);

    ObjectArtefactData * getData();
    int getStepIndex() const;

    friend bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_);
};

inline
bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_)
{
    return left_.m_stepIndex < right_.m_stepIndex;
}

#endif // DRAWINGDATA_OBJECTARTEFACT_H
