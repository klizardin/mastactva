#include "drawingdata_effectobjects.h"


#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_objectinfo.h"
#include "../MastactvaModels/drawingdata_objectartefact.h"


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
    void build(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const;

private:
    void findEnd();

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

void ObjectArtefacts::build(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        (*it)->addData(details_);
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        if((*it)->setVertexShader(object_, details_))
        {
            (*it)->addArguments(object_, details_);
            break;
        }
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        if((*it)->setFragmentShader(object_, details_))
        {
            (*it)->addArguments(object_, details_);
            break;
        }
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        (*it)->addTexture(object_);
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


void DrawingDataEffectObjects::addObjects(
        drawing_data::QuizImageObjects &data_,
        const drawingdata::Details &details_
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
        data_.objects.push_back(std::make_shared<drawing_data::QuizImageObject>());
        list.build(*(data_.objects.back()), details_);
    }
}
