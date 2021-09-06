/*
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


DrawingDataEffect::DrawingDataEffect(EffectData &&data_)
    : EffectData(std::move(data_))
{
    drawingdata::utils::rebuild(m_effectObjectsData, static_cast<DrawingDataEffectObjects *>(nullptr));
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

void DrawingDataEffect::initialize(drawing_data::QuizImageObjects &data_) const
{
    if(!m_effectObjectsData.operator bool()
            || !m_details.filesource.operator bool())
    {
        return;
    }
    const_cast<DrawingDataEffect *>(this)->m_details.clear();
    using SortedEffectObjects = std::multimap<int, const DrawingDataEffectObjects *>;
    using SortedByProgrammerNameEffectObjects = std::multimap<QString, const DrawingDataEffectObjects *>;
    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    SortedByProgrammerNameEffectObjects sortedByProgrammerNameEffectObjects;
    for(const EffectObjectsData *effectObject_ : *m_effectObjectsData)
    {
        const DrawingDataEffectObjects *effectObject = dynamic_cast<const DrawingDataEffectObjects *>(effectObject_);
        if(!effectObject)
        {
            continue;
        }
        if(effectObject->isMain())
        {
            sortedMainEffectObjects.insert({effectObject->getStepIndex(), effectObject});
        }
        else
        {
            sortedEffectObjects.insert({effectObject->getStepIndex(), effectObject});
            sortedByProgrammerNameEffectObjects.insert({effectObject->getProgrammerName(), effectObject});
        }
    }
    for(const SortedEffectObjects::value_type &v_ : sortedMainEffectObjects)
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details);
    }
    QStringList objectsToRun;
    if(m_details.variables.operator bool()
            && m_details.variables->getObjectsList(objectsToRun))
    {
        auto objectsToRunUnique = getUnique(objectsToRun);
        std::map<QString, int> stepIndexShifts = setupShifts(objectsToRunUnique);
        for(const QString &objectName_ : objectsToRun)
        {
            const auto fitb = sortedByProgrammerNameEffectObjects.lower_bound(objectName_);
            const auto fite = sortedByProgrammerNameEffectObjects.upper_bound(objectName_);
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
    else
    {
        for(const SortedEffectObjects::value_type &v_ : sortedEffectObjects)
        {
            if(!v_.second)
            {
                continue;
            }
            v_.second->addObjects(data_, m_details);
        }
    }
}
