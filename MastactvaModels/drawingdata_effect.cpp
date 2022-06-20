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

void DrawingDataEffect::init(std::shared_ptr<IAddonModules> modules_)
{
    m_details.modules = std::move(modules_);
}

std::set<QString> getUnique(const QStringList &list_)
{
    std::set<QString> result;
    for(const QString &key_ : qAsConst(list_))
    {
        result.insert(key_);
    }
    return result;
}

std::map<QString, int> setupShifts(const std::set<QString> &unique_)
{
    std::map<QString, int> result;
    for(const QString &key_ : qAsConst(unique_))
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

    // clean up details
    const_cast<DrawingDataEffect *>(this)->m_details.clear();

    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    SortedByProgrammerNameEffectObjects sortedByProgrammerNameEffectObjects;

    // extract main objects from the list of all objects
    extractMainObjects(sortedEffectObjects, sortedMainEffectObjects, sortedByProgrammerNameEffectObjects);

    // initialize helper data holder object to pass as an argument
    DrawingDataArgSetsAndArgs argSetsAndArgs;
    argSetsAndArgs.m_effectArgSetsData = m_effectArgSetsData;
    argSetsAndArgs.m_effectArgsData = m_effectArgsData;

    // initialize arguments from arg sets as variables
    addArguments(argSetsAndArgs, sortedMainEffectObjects, true);    // add local object variables
    addArguments(argSetsAndArgs, sortedEffectObjects, false);       // add global objects variables
    // set up as global objects needed because of possibility of existing of objects list
    // where we can not determine exact position of the object (just template of object)
    // and so all variables are set for all objects (variable for specific template of object, not for exact object)
    // such variables set up before processing of any object

    // value of variable can be changed while processing of objects
    // and the latest value of variables will be used

    // process main objects
    runObjects(data_, sortedMainEffectObjects, true);

    QStringList objectsToRun;
    // check if we have an objects list
    // this is the list of objects build by main objects
    // that describe actual objects chain from template objects
    if(m_details.variables.operator bool()
            && m_details.variables->getObjectsList(objectsToRun))
    {
        // process objects with the objects list
        // already existing objects are used as template objects
        // in the object list possibly contains several objects of same object template in different order
        processByObjectListOrder(data_, objectsToRun, sortedByProgrammerNameEffectObjects);
    }
    else
    {
        // process objects without the objects list
        runObjects(data_, sortedEffectObjects);
    }
}

/*
 * return all possible addon names for the whole effect
*/
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

    // addon names can be specified just for main objects
    getAddonNames(sortedMainEffectObjects, names_);
}

/*
 * the function to run effect's addons with specific names (by the function's argument)
 * the function reinitialize details of the effect data, so it cleans the current state
*/
void DrawingDataEffect::runAddons(const QStringList &names_) const
{
    drawing_data::QuizImageObjects data_;
    //int argsSetIndex_ = 0;

    if(!m_effectObjectsData.operator bool()
            || !m_details.filesource.operator bool())
    {
        return;
    }

    // clean effect state
    const_cast<DrawingDataEffect *>(this)->m_details.clear();

    SortedEffectObjects sortedMainEffectObjects;
    SortedEffectObjects sortedEffectObjects;
    SortedByProgrammerNameEffectObjects sortedByProgrammerNameEffectObjects;

    // extract main objects
    extractMainObjects(sortedEffectObjects, sortedMainEffectObjects, sortedByProgrammerNameEffectObjects);

    // set up helper data holder object for arg sets
    DrawingDataArgSetsAndArgs argSetsAndArgs;
    argSetsAndArgs.m_effectArgSetsData = m_effectArgSetsData;
    argSetsAndArgs.m_effectArgsData = m_effectArgsData;

    // initialize variables
    addArguments(argSetsAndArgs, sortedMainEffectObjects, true);
    addArguments(argSetsAndArgs, sortedEffectObjects, false);

    // addons can be specified just for main objects
    // run main objects
    // and run addons
    runObjectsAndAddons(data_, sortedMainEffectObjects, names_);
}

/*
 * build list of
 *   * main objects - to do preparations before run not main objects
 *   * all other objects - to main effect objects presentation
 *   * all other objects sorted by programmer name - to better process objects list - where objects specified by its names
*/
void DrawingDataEffect::extractMainObjects(
        SortedEffectObjects &sortedEffectObjects_,
        SortedEffectObjects &sortedMainEffectObjects_,
        SortedByProgrammerNameEffectObjects &sortedByProgrammerNameEffectObjects_
        ) const
{
    for(const EffectObjectsData *effectObject_ : qAsConst(*m_effectObjectsData))
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
        SortedEffectObjects &objects_,
        bool mainObjects_ /*= false*/
        ) const
{
    for(const SortedEffectObjects::value_type &v_ : qAsConst(objects_))
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details, 0, QStringList{}, mainObjects_);
    }
}

void DrawingDataEffect::addArguments(
        DrawingDataArgSetsAndArgs &argSetsAndArgs_,
        SortedEffectObjects &objects_,
        bool local_
        ) const
{
    argSetsAndArgs_.setObjectLocalPosition(local_);
    for(const SortedEffectObjects::value_type &v_ : qAsConst(objects_))
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
    // list of objects that are used in the object list
    auto objectsToRunUnique = getUnique(objectsToRun_);
    // set up index shifts
    std::map<QString, int> stepIndexShifts = setupShifts(objectsToRunUnique);
    for(const QString &objectName_ : qAsConst(objectsToRun_))
    {
        // get all template objects for the given object name in the objects list
        // it can be several template objects with the same name
        const auto fitb = sortedByProgrammerNameEffectObjects_.lower_bound(objectName_);
        const auto fite = sortedByProgrammerNameEffectObjects_.upper_bound(objectName_);
        std::size_t objectsInPack = std::distance(fitb, fite);
        Q_ASSERT(stepIndexShifts.find(objectName_) != std::end(stepIndexShifts));
        // get the current objects shift for the specific object name
        const int stepIndexShift = stepIndexShifts[objectName_];
        for(auto it = fitb; it != fite; ++it)
        {
            // add objects from the template objects
            it->second->addObjects(data_, m_details, stepIndexShift);
        }
        // correct index shift for the current object name
        stepIndexShifts[objectName_] += objectsInPack;
    }
}

void DrawingDataEffect::getAddonNames(
        SortedEffectObjects &objects_,
        QStringList &names_
        ) const
{
    for(const SortedEffectObjects::value_type &v_ : qAsConst(objects_))
    {
        if(!v_.second)
        {
            continue;
        }
        Q_UNUSED(names_);
        v_.second->getAddonNames(names_);
    }
}

void DrawingDataEffect::runObjectsAndAddons(
        drawing_data::QuizImageObjects &data_,
        SortedEffectObjects &objects_,
        const QStringList &addonNames_
        ) const
{
    for(const SortedEffectObjects::value_type &v_ : qAsConst(objects_))
    {
        if(!v_.second)
        {
            continue;
        }
        v_.second->addObjects(data_, m_details, 0, addonNames_);
    }
}
