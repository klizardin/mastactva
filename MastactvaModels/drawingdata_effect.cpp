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

#include "drawingdata_effect.h"
#include <map>
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_effectobjects.h"
#include "../MastactvaModels/drawingdata_effectarg.h"
#include "../MastactvaModels/drawingdata_effectargset.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"


DrawingDataEffect::DrawingDataEffect(EffectData &&data_)
    : EffectData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectObjectsData, static_cast<DrawingDataEffectObjects *>(nullptr));
    drawingdata::utils::rebuild(m_effectArgsData, static_cast<DrawingDataEffectArg *>(nullptr));
    drawingdata::utils::rebuild(m_effectArgSetsData, static_cast<DrawingDataEffectArgSet *>(nullptr));
}

void DrawingDataEffect::init(std::shared_ptr<drawingdata::IFileSource> filesources_)
{
    m_details.filesource = std::move(filesources_);
}

std::set<QString> getUnique(const QStringList &list_)
{
    std::set<QString> result;
    for(const QString &key_ : list_)
    {
        result.insert(key_);
    }
    return result;
}

std::map<QString, int> setupShifts(const std::set<QString> &unique_)
{
    std::map<QString, int> result;
    for(const QString &key_ : unique_)
    {
        result.insert({key_, 0});
    }
    return result;
}

void DrawingDataEffect::initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    if(!m_effectObjectsData.operator bool()
            || !m_details.filesource.operator bool())
    {
        return;
    }

    const_cast<DrawingDataEffect *>(this)->m_details.clear();

    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    SortedByProgrammerNameEffectObjects sortedByProgrammerNameEffectObjects;

    extractMainObjects(sortedEffectObjects, sortedMainEffectObjects, sortedByProgrammerNameEffectObjects);

    DrawingDataArgSetsAndArgs argSetsAndArgs;
    argSetsAndArgs.m_effectArgSetsData = m_effectArgSetsData;
    argSetsAndArgs.m_effectArgsData = m_effectArgsData;

    addArguments(argSetsAndArgs, sortedMainEffectObjects, true);
    addArguments(argSetsAndArgs, sortedEffectObjects, false);

    runObjects(data_, sortedMainEffectObjects);

    QStringList objectsToRun;
    if(m_details.variables.operator bool()
            && m_details.variables->getObjectsList(objectsToRun))
    {
        processByObjectListOrder(data_, objectsToRun, sortedByProgrammerNameEffectObjects);
    }
    else
    {
        runObjects(data_, sortedEffectObjects);
    }
}

void DrawingDataEffect::getAddonNames(QStringList &names_) const
{
    if(!m_effectObjectsData.operator bool()
            || !m_details.filesource.operator bool())
    {
        return;
    }

    const_cast<DrawingDataEffect *>(this)->m_details.clear();

    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    SortedByProgrammerNameEffectObjects sortedByProgrammerNameEffectObjects;

    extractMainObjects(sortedEffectObjects, sortedMainEffectObjects, sortedByProgrammerNameEffectObjects);

    getAddonNames(sortedMainEffectObjects, names_);
}

void DrawingDataEffect::extractMainObjects(
        SortedEffectObjects &sortedEffectObjects_,
        SortedEffectObjects &sortedMainEffectObjects_,
        SortedByProgrammerNameEffectObjects &sortedByProgrammerNameEffectObjects_
        ) const
{
    for(const EffectObjectsData *effectObject_ : *m_effectObjectsData)
    {
        const DrawingDataEffectObjects *effectObject = dynamic_cast<const DrawingDataEffectObjects *>(effectObject_);
        if(!effectObject)
        {
            continue;
        }
        if(effectObject->isMain())
        {
            sortedMainEffectObjects_.insert({effectObject->getStepIndex(), effectObject});
        }
        else
        {
            sortedEffectObjects_.insert({effectObject->getStepIndex(), effectObject});
            sortedByProgrammerNameEffectObjects_.insert({effectObject->getProgrammerName(), effectObject});
        }
    }
}

void DrawingDataEffect::runObjects(
        drawing_data::QuizImageObjects &data_,
        SortedEffectObjects &objects_
        ) const
{
    for(const SortedEffectObjects::value_type &v_ : objects_)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details);
    }
}

void DrawingDataEffect::addArguments(
        DrawingDataArgSetsAndArgs &argSetsAndArgs_,
        SortedEffectObjects &objects_,
        bool local_
        ) const
{
    argSetsAndArgs_.setObjectLocalPosition(local_);
    for(const SortedEffectObjects::value_type &v_ : objects_)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addArgs(m_details, argSetsAndArgs_);
    }
}

void DrawingDataEffect::processByObjectListOrder(
        drawing_data::QuizImageObjects &data_,
        const QStringList &objectsToRun_,
        const SortedByProgrammerNameEffectObjects &sortedByProgrammerNameEffectObjects_
        ) const
{
    auto objectsToRunUnique = getUnique(objectsToRun_);
    std::map<QString, int> stepIndexShifts = setupShifts(objectsToRunUnique);
    for(const QString &objectName_ : objectsToRun_)
    {
        const auto fitb = sortedByProgrammerNameEffectObjects_.lower_bound(objectName_);
        const auto fite = sortedByProgrammerNameEffectObjects_.upper_bound(objectName_);
        std::size_t objectsInPack = std::distance(fitb, fite);
        Q_ASSERT(stepIndexShifts.find(objectName_) != std::end(stepIndexShifts));
        const int stepIndexShift = stepIndexShifts[objectName_];
        for(auto it = fitb; it != fite; ++it)
        {
            it->second->addObjects(data_, m_details, stepIndexShift);
        }
        stepIndexShifts[objectName_] += objectsInPack;
    }
}

void DrawingDataEffect::getAddonNames(
        SortedEffectObjects &objects_,
        QStringList &names_
        ) const
{
    for(const SortedEffectObjects::value_type &v_ : objects_)
    {
        if(!v_.second)
        {
            continue;
        }
        Q_UNUSED(names_);
        v_.second->getAddonNames(names_);
    }
}

