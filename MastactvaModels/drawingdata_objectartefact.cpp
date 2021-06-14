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
        const drawingdata::Details &details_
        ) const
{
    return forArtefacts([&object_, &details_](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->setVertexShader(object_, details_);
    });
}

bool DrawingDataObjectArtefact::setFragmentShader(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    return forArtefacts([&object_, &details_](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->setFragmentShader(object_, details_);
    });
}

void DrawingDataObjectArtefact::addArguments(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    (void)forArtefacts([&object_, &details_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addArguments(object_, details_);
        return false;
    });
}

void DrawingDataObjectArtefact::addTexture(
        drawing_data::QuizImageObject &object_
        ) const
{
    (void)forArtefacts([&object_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addTexture(object_);
        return false;
    });
}

void DrawingDataObjectArtefact::addData(
        const drawingdata::Details &details_
        ) const
{
    (void)forArtefacts([&details_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addData(details_);
        return false;
    });
}

void DrawingDataObjectArtefact::updateStepIndex(drawingdata::IPosition *position_, bool first_) const
{
    if(!position_)
    {
        return;
    }

    if(first_)
    {
        position_->resetArtefactStepIndex(getStepIndex());
    }
    else
    {
        position_->setArtefactStepIndex(getStepIndex());
    }
}

void DrawingDataObjectArtefact::checkStepIndex(drawingdata::IPosition *position_) const
{
    if(!position_)
    {
        return;
    }

    Q_ASSERT(position_->getArtefactStepIndex() == getStepIndex());
}

bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_)
{
    return left_.m_stepIndex < right_.m_stepIndex;
}
