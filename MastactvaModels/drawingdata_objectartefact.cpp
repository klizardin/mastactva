/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "drawingdata_objectartefact.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefact.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"


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

bool DrawingDataObjectArtefact::hasArguments() const
{
    return forArtefacts([](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->hasArguments();
    });
}

bool DrawingDataObjectArtefact::hasVariables() const
{
    return forArtefacts([](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->hasVariables();
    });
}

void DrawingDataObjectArtefact::addVariables(
        const drawingdata::Details &details_,
        DrawingDataArgSetsAndArgs *argSetsAndArgs_ /*= nullptr*/
        ) const
{
    if(argSetsAndArgs_)
    {
        argSetsAndArgs_->setObjectArtefactId(m_id);
    }

    (void) forArtefacts([&details_, argSetsAndArgs_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addVariables(details_, argSetsAndArgs_);
        return false;
    });

    if(argSetsAndArgs_)
    {
        argSetsAndArgs_->clearObjectArtefactId();
    }
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

void DrawingDataObjectArtefact::addCalculations(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    (void)forArtefacts([&object_, &details_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addCalculations(object_, details_);
        return false;
    });
}

void DrawingDataObjectArtefact::addMainCalculations(
        drawing_data::QuizImageObjects &objects_,
        const drawingdata::Details &details_
        ) const
{
    (void)forArtefacts([&objects_, &details_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->addMainCalculations(objects_, details_);
        return false;
    });
}

bool DrawingDataObjectArtefact::hasAddon() const
{
    return forArtefacts([](const DrawingDataArtefact *artefact_)->bool
    {
        return artefact_->hasAddon();
    });
}

void DrawingDataObjectArtefact::getAddonNames(QStringList &names_) const
{
    (void)forArtefacts([&names_](const DrawingDataArtefact *artefact_)->bool
    {
        artefact_->getAddonNames(names_);
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
