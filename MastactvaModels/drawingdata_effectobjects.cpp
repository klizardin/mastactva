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


class ObjectArtefacts
{
private:
    using VectorType = QVector<const DrawingDataObjectArtefact *>;
    using Iterator = QVector<const DrawingDataObjectArtefact *>::const_iterator;

public:
    ObjectArtefacts() = default;
    void populate(const QVector<ObjectArtefactData *> &objectArtefacts_);
    bool isEnd() const;
    void first();
    void next();
    bool build(
            drawing_data::QuizImageObjects &data_,
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_,
            const QStringList &addonNames_ = QStringList{},
            bool mainObjects_ = false
            ) const;
    void addArgs(
            const drawingdata::Details &details_,
            DrawingDataArgSetsAndArgs &argSetsAndArgs_
            );
    void addMainCalculations(
            drawing_data::QuizImageObjects &objects_,
            const drawingdata::Details &details_
            ) const;
    void getAddonNames(
            QStringList &names_
            ) const;

private:
    void findEnd();
    static void updateArtefactStepIndex(
            const drawingdata::Details &details_,
            const DrawingDataObjectArtefact *objectArtefact_,
            bool first_
            );
    static void checkArtefactStepIndex(
            const drawingdata::Details &details_,
            const DrawingDataObjectArtefact *objectArtefact_
            );

private:
    VectorType m_artefacts;
    Iterator m_objectBegin = std::cend(m_artefacts);
    Iterator m_objectEnd = std::cend(m_artefacts);
};

void ObjectArtefacts::populate(const QVector<ObjectArtefactData *> &objectArtefacts_)
{
    m_artefacts.clear();
    m_artefacts.reserve(objectArtefacts_.size());
    for(const ObjectArtefactData *ptr_ : objectArtefacts_)
    {
        auto ptr = dynamic_cast<const DrawingDataObjectArtefact *>(ptr_);
        if(!ptr)
        {
            continue;
        }
        m_artefacts.push_back(ptr);
    }
    std::sort(
                std::begin(m_artefacts),
                std::end(m_artefacts),
                [](const DrawingDataObjectArtefact *left_, const DrawingDataObjectArtefact *right_)->bool
    {
        return left_ && right_ && *left_ < *right_;
    });
    m_objectBegin = std::cend(m_artefacts);
    m_objectEnd = std::cend(m_artefacts);
}

bool ObjectArtefacts::isEnd() const
{
    return std::cend(m_artefacts) == m_objectBegin;
}

void ObjectArtefacts::first()
{
    m_objectBegin = std::cbegin(m_artefacts);
    findEnd();
}

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
    if(std::cend(m_artefacts) != m_objectBegin)
    {
        updateArtefactStepIndex(details_, *m_objectBegin, std::cbegin(m_artefacts) == m_objectBegin);
    }

    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->hasVariables())
        {
            (*it)->addVariables(details_);
        }
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addCalculations(object_, details_);
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addData(details_);
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->setVertexShader(object_, details_))
        {
            (*it)->addArguments(object_, details_);
            break;
        }
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        if((*it)->setFragmentShader(object_, details_))
        {
            (*it)->addArguments(object_, details_);
            break;
        }
    }
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
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        checkArtefactStepIndex(details_, *it);
        (*it)->addTexture(object_);
    }
    // run addons after processing of a whole object
    if(!addonNames_.isEmpty())
    {
        for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
        {
            checkArtefactStepIndex(details_, *it);
            (*it)->runAddons(details_, addonNames_);
        }
    }
    const bool bothShaders =
            !object_.fragmentShader.isEmpty()
            && !object_.vertexShader.isEmpty()
            ;
    const bool anyShader =
            !object_.fragmentShader.isEmpty()
            || !object_.vertexShader.isEmpty()
            ;
    return (object_.hasCalculations() && !anyShader)
            || bothShaders
            ;
}

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
        if(isMain())
        {
            list.addMainCalculations(data_, details_);
        }
        auto obj = std::make_shared<drawing_data::QuizImageObject>();
        if(list.build(data_, *obj, details_, addonNames_, mainObjects_))
        {
            data_.objects.push_back(obj);
        }
    }
}

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
        return g_defaultObjectInfoProgrammerName;
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
