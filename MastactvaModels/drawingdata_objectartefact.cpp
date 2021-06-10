#include "drawingdata_objectartefact.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefact.h"


DrawingDataObjectArtefact::DrawingDataObjectArtefact(ObjectArtefactData &&data_)
    : ObjectArtefactData(std::move(data_))
{
    drawingdata::utils::rebuild(m_artefactData, static_cast<DrawingDataArtefact *>(nullptr));
}

int DrawingDataObjectArtefact::getStepIndex() const
{
    return m_stepIndex;
}

bool DrawingDataObjectArtefact::forArtefacts(
        std::function<bool(const DrawingDataArtefact *)> func_
        ) const
{
    if(!m_artefactData.operator bool())
    {
        return false;
    }
    for(const ArtefactData * artefact_: *m_artefactData)
    {
        auto artefact = dynamic_cast<const DrawingDataArtefact *>(artefact_);
        if(!artefact)
        {
            continue;
        }
        if(func_(artefact))
        {
            return true;
        }
    }
    return false;
}

bool DrawingDataObjectArtefact::setVertexShader(
        drawing_data::QuizImageObject &object_,
        drawingdata::IFileSource *filesource_
        ) const
{
    return forArtefacts([&object_, &filesource_](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->setVertexShader(object_, filesource_);
    });
}

bool DrawingDataObjectArtefact::setFragmentShader(
        drawing_data::QuizImageObject &object_,
        drawingdata::IFileSource *filesource_
        ) const
{
    return forArtefacts([&object_, &filesource_](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->setFragmentShader(object_, filesource_);
    });
}

void DrawingDataObjectArtefact::addArguments(
        drawing_data::QuizImageObject &object_
        ) const
{
    (void)forArtefacts([&object_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addArguments(object_);
        return false;
    });
}

bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_)
{
    return left_.m_stepIndex < right_.m_stepIndex;
}
