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

#include "drawingdata_effectobjects.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_objectinfo.h"
#include "../MastactvaModels/drawingdata_objectartefact.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"


DrawingDataEffectObjects::DrawingDataEffectObjects(EffectObjectsData &&data_)
    : EffectObjectsData(std::move(data_))
{
    drawingdata::utils::rebuild(m_objectInfoData, static_cast<DrawingDataObjectInfo *>(nullptr));
    drawingdata::utils::rebuild(m_objectArtefactData, static_cast<DrawingDataObjectArtefact *>(nullptr));
}


/*
 * class to process object's artefacts
*/
class ObjectArtefacts
{
private:
    using VectorType = QVector<const DrawingDataObjectArtefact *>;
    using Iterator = QVector<const DrawingDataObjectArtefact *>::const_iterator;

public:
    ObjectArtefacts() = default;

    /*
     * populate this class with artefacts and build iterator over the artefacts
    */
    void populate(const QVector<ObjectArtefactData *> &objectArtefacts_);

    /*
     * is end of the artefacts
    */
    bool isEnd() const;

    /*
     * move to the first artefact
    */
    void first();

    /*
     * move to te next artefact
    */
    void next();

    /*
     * build object and fulfill the image object data
     * return true if object can be added to the list of image objects
    */
    bool build(
            drawing_data::QuizImageObjects &data_,      // image objects
            drawing_data::QuizImageObject &object_,     // this image object to fulfill
            const drawingdata::Details &details_,       // details of the parent class to share data between objects
            const QStringList &addonNames_ = QStringList{},     // addon names to run
            bool mainObjects_ = false                   // is it the main object
            ) const;

    /*
     * add arguments to the details data
    */
    void addArgs(
            const drawingdata::Details &details_,       // details of the parent class to share data between objects
            DrawingDataArgSetsAndArgs &argSetsAndArgs_  // arg sets
            );

    /*
     * add main calculations
    */
    void addMainCalculations(
            drawing_data::QuizImageObjects &objects_,   // image objects
            const drawingdata::Details &details_        // details from the parent class
            ) const;

    /*
     * return addons names
    */
    void getAddonNames(
            QStringList &names_
            ) const;

private:
    /*
     * helper function to initialize iterator
    */
    void findEnd();

    /*
     * helper function to update detail's artefact step index
    */
    static void updateArtefactStepIndex(
            const drawingdata::Details &details_,               // details from the parent class
            const DrawingDataObjectArtefact *objectArtefact_,   // artefact
            bool first_                                         // is the first artefact
            );

    /*
     * do runtime checks
    */
    static void checkArtefactStepIndex(
            const drawingdata::Details &details_,
            const DrawingDataObjectArtefact *objectArtefact_
            );

private:
    VectorType m_artefacts;                                 // list of artefacts
    Iterator m_objectBegin = std::cend(m_artefacts);        // begin iterator
    Iterator m_objectEnd = std::cend(m_artefacts);          // end iterator
};

void ObjectArtefacts::populate(const QVector<ObjectArtefactData *> &objectArtefacts_)
{
    m_artefacts.clear();
    m_artefacts.reserve(objectArtefacts_.size());
    // push all artefacts
    for(const ObjectArtefactData *ptr_ : objectArtefacts_)
    {
        auto ptr = dynamic_cast<const DrawingDataObjectArtefact *>(ptr_);
        if(!ptr)
        {
            continue;
        }
        m_artefacts.push_back(ptr);
    }
    // sort artefacts by step index of artefact
    std::sort(
                std::begin(m_artefacts),
                std::end(m_artefacts),
                [](const DrawingDataObjectArtefact *left_, const DrawingDataObjectArtefact *right_)->bool
    {
        return left_ && right_ && *left_ < *right_;
    });
    // initialize iterators
    m_objectBegin = std::cend(m_artefacts);
    m_objectEnd = std::cend(m_artefacts);
}

bool ObjectArtefacts::isEnd() const
{
    return std::cend(m_artefacts) == m_objectBegin;
}

/*
 * set up to the first step inedx
*/
void ObjectArtefacts::first()
{
    m_objectBegin = std::cbegin(m_artefacts);
    findEnd();
}

/*
 * set up to the next step index
*/
void ObjectArtefacts::next()
{
    m_objectBegin = m_objectEnd;
    findEnd();
}

bool ObjectArtefacts::build(
        drawing_data::QuizImageObjects &data_,
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_,
        const QStringList &addonNames_ /*= QStringList{}*/,
        bool mainObjects_ /*= false*/
        ) const
{
    // set up for details current step index
    if(std::cend(m_artefacts) != m_objectBegin)
    {
        updateArtefactStepIndex(details_, *m_objectBegin, std::cbegin(m_artefacts) == m_objectBegin);
    }

    // add all variables of the artefacts of the current step index
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->hasVariables())
        {
            (*it)->addVariables(details_);
        }
    }
    // add all calculations of the artefacts of the current step index
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addCalculations(object_, details_);
    }
    // add data from all the artefacts from the current step index
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addData(details_);
    }
    // add first vertex shader from the artefacts of the current step index
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->setVertexShader(object_, details_))
        {
            // add arguments for this shader
            (*it)->addArguments(object_, details_);
            break;
        }
    }
    // add first fragment shader from the artefacts of the current step index
    // artefact of the fisrt fragment shader can be not equal to the artefact of the first vertex shader - TODO: maybe fix it
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->setFragmentShader(object_, details_))
        {
            // add arguments for this shader
            (*it)->addArguments(object_, details_);
            break;
        }
    }
    // add global arguments if these are the main object's artefacts
    if(mainObjects_)
    {
        for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
        {
            checkArtefactStepIndex(details_, *it);
            if((*it)->hasGlobalArguments())
            {
                (*it)->addGlobalArguments(data_, details_);
                break;
            }
        }
    }
    // add textures for all artefacts of the current step index
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addTexture(object_);
    }
    // run addons after processing of a all object's artefacts of the current step index
    if(!addonNames_.isEmpty())
    {
        for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
        {
            checkArtefactStepIndex(details_, *it);
            (*it)->runAddons(details_, addonNames_);
        }
        return false; // do not need to add this object, just process it state
    }
    // we generate separate image objects for the artefacts of each step index of the drawing object
    const bool bothShaders =
            !object_.fragmentShader.isEmpty()
            && !object_.vertexShader.isEmpty()
            ;
    const bool anyShader =
            !object_.fragmentShader.isEmpty()
            || !object_.vertexShader.isEmpty()
            ;
    // should we add new object or now:
    // yes - if it can draw or calculate in the runtime
    // throw it away - if all processing is done and nothing to do at runtime
    return (object_.hasCalculations() && !anyShader)
            || bothShaders
            ;
}

/*
 * simplified implemenattion for adding variables
*/
void ObjectArtefacts::addArgs(
        const drawingdata::Details &details_,
        DrawingDataArgSetsAndArgs &argSetsAndArgs_
        )
{
    if(std::cend(m_artefacts) != m_objectBegin)
    {
        updateArtefactStepIndex(details_, *m_objectBegin, std::cbegin(m_artefacts) == m_objectBegin);
    }

    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->hasArguments())
        {
            (*it)->addVariables(details_, &argSetsAndArgs_);
        }
    }
}

/*
 * simpilfied implementation for returning of addon names
*/
void ObjectArtefacts::getAddonNames(
        QStringList &names_
        ) const
{
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        if((*it)->hasAddon())
        {
            (*it)->getAddonNames(names_);
        }
    }
}

/*
 * simplified implementation for adding the main calculations
*/
void ObjectArtefacts::addMainCalculations(
        drawing_data::QuizImageObjects &objects_,
        const drawingdata::Details &details_
        ) const
{
    if(std::cend(m_artefacts) != m_objectBegin)
    {
        updateArtefactStepIndex(details_, *m_objectBegin, std::cbegin(m_artefacts) == m_objectBegin);
    }

    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addMainCalculations(objects_, details_);
    }
}

/*
 * find end index of artefacts with current step index (step index of the artefact in the m_objectBegin)
*/
void ObjectArtefacts::findEnd()
{
    if(!isEnd())
    {
        m_objectEnd = std::upper_bound(
                    m_objectBegin,
                    std::cend(m_artefacts),
                    (*m_objectBegin)->getStepIndex(),
                    [](int stepIndex_, const DrawingDataObjectArtefact *ptr_)->bool
        {
            return ptr_ && stepIndex_ < ptr_->getStepIndex();
        });
    }
    else
    {
        m_objectEnd = std::cend(m_artefacts);
    }
}

void ObjectArtefacts::updateArtefactStepIndex(
        const drawingdata::Details &details_,
        const DrawingDataObjectArtefact *objectArtefact_,
        bool first_
        )
{
    if(!details_.position.operator bool()
            || !objectArtefact_)
    {
        return;
    }
    objectArtefact_->updateStepIndex(details_.position.get(), first_);
}

void ObjectArtefacts::checkArtefactStepIndex(
        const drawingdata::Details &details_,
        const DrawingDataObjectArtefact *objectArtefact_
        )
{
    if(!details_.position.operator bool()
            || !objectArtefact_)
    {
        return;
    }
    objectArtefact_->checkStepIndex(details_.position.get());
}


void DrawingDataEffectObjects::addObjects(
        drawing_data::QuizImageObjects &data_,
        const drawingdata::Details &details_,
        int stepIndexShift_ /*= 0*/,
        const QStringList &addonNames_ /*= QStringList{}*/,
        bool mainObjects_ /*= false*/
        ) const
{
    if(!m_objectArtefactData.operator bool())
    {
        return;
    }

    setupPosition(details_, stepIndexShift_);

    ObjectArtefacts list;
    list.populate(*m_objectArtefactData);
    for(list.first(); !list.isEnd(); list.next())
    {
        // iterate through step indexes
        // add separate image object for each step index of the artefacts of the current drawing object
        if(isMain())
        {
            list.addMainCalculations(data_, details_);
        }
        // create temperary image object
        auto obj = std::make_shared<drawing_data::QuizImageObject>();
        // build image object
        if(list.build(data_, *obj, details_, addonNames_, mainObjects_))
        {
            // if we need to add new image object we do it
            data_.objects.push_back(obj);
        }
        // else throw away temperary image object
    }
}

/*
 * simplified implemenattion for adding variables
 * do not create objects
*/
void DrawingDataEffectObjects::addArgs(
        const drawingdata::Details &details_,
        DrawingDataArgSetsAndArgs &argSetsAndArgs_,
        int stepIndexShift_ /*= 0*/
        ) const
{
    if(!m_objectArtefactData.operator bool())
    {
        return;
    }

    setupPosition(details_, stepIndexShift_);

    ObjectArtefacts list;
    list.populate(*m_objectArtefactData);
    for(list.first(); !list.isEnd(); list.next())
    {
        list.addArgs(details_, argSetsAndArgs_);
    }
}

/*
 * simpilfied implementation for returning of addon names
 * do not create objects
*/
void DrawingDataEffectObjects::getAddonNames(
        QStringList &names_
        ) const
{
    if(!m_objectArtefactData.operator bool())
    {
        return;
    }

    ObjectArtefacts list;
    list.populate(*m_objectArtefactData);
    for(list.first(); !list.isEnd(); list.next())
    {
        list.getAddonNames(names_);
    }
}

/*
 * object is main if it's programmer name starts with g_defaultObjectInfoProgrammerName
*/
bool DrawingDataEffectObjects::isMain() const
{
    return m_objectInfoData.operator bool()
            && 1 == m_objectInfoData->size()
            && m_objectInfoData->front()->m_programmerName.startsWith(g_defaultObjectInfoProgrammerName)
            ;
}

int DrawingDataEffectObjects::getStepIndex() const
{
    return m_stepIndex;
}

QString DrawingDataEffectObjects::getProgrammerName() const
{
    if(m_objectInfoData.operator bool()
            && 1 == m_objectInfoData->size())
    {
        return m_objectInfoData->front()->m_programmerName;
    }
    else
    {
        return g_defaultObjectInfoProgrammerName; // not initialized objects are main objects
    }
}

void DrawingDataEffectObjects::setupPosition(const drawingdata::Details &details_, int stepIndexShift_) const
{
    if(!details_.position.operator bool())
    {
        return;
    }
    details_.position->setObject(getProgrammerName(), m_stepIndex + stepIndexShift_);
}
