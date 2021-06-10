#include "effectobjects_data.h"
#include <map>
#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaBase/data_utils.h"


EffectObjectsData::EffectObjectsData()
{
    m_objectInfoData = data_object::utils::createDataVector(static_cast<const ObjectInfoData *>(nullptr));
    m_objectArtefactData = data_object::utils::createDataVector(static_cast<const ObjectArtefactData *>(nullptr));
}

EffectObjectsData::EffectObjectsData(
        int id_,
        int effectId_,
        int objectInfoId_,
        int stepIndex_
        )
    : m_id(id_),
      m_effectId(effectId_),
      m_objectInfoId(objectInfoId_),
      m_stepIndex(stepIndex_)
{
    m_objectInfoData = data_object::utils::createDataVector(static_cast<const ObjectInfoData *>(nullptr));
    m_objectArtefactData = data_object::utils::createDataVector(static_cast<const ObjectArtefactData *>(nullptr));
}

EffectObjectsData::EffectObjectsData(EffectObjectsData &&data_)
{
    operator=(std::move(data_));
}

EffectObjectsData & EffectObjectsData::operator = (EffectObjectsData &&data_)
{
    m_id = std::move(data_.m_id);
    m_effectId = std::move(data_.m_effectId);
    m_objectInfoId = std::move(data_.m_objectInfoId);
    m_stepIndex = std::move(data_.m_stepIndex);
    m_objectInfoData = std::move(data_.m_objectInfoData);
    m_objectArtefactData = std::move(data_.m_objectArtefactData);
    return *this;
}

std::unique_ptr<EffectObjectsData> EffectObjectsData::copy() const
{
    std::unique_ptr<EffectObjectsData> result = std::make_unique<EffectObjectsData>();
    result->m_id = m_id;
    result->m_effectId = m_effectId;
    result->m_objectInfoId = m_objectInfoId;
    result->m_stepIndex = m_stepIndex;
    data_object::utils::copyDataVector(m_objectInfoData.get(), result->m_objectInfoData.get());
    data_object::utils::copyDataVector(m_objectArtefactData.get(), result->m_objectArtefactData.get());
    return result;
}

class ObjectArtefacts
{
private:
    using VectorType = QVector<const ObjectArtefactData *>;
    using Iterator = QVector<const ObjectArtefactData *>::const_iterator;

public:
    ObjectArtefacts() = default;
    void populate(const QVector<ObjectArtefactData *> &objectArtefacts_);
    bool isEnd() const;
    void first();
    void next();
    void build(
            drawing_data::QuizImageObject &object_,
            drawingdata::IFileSource *filesource_
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
    std::copy_if(
                std::cbegin(objectArtefacts_),
                std::cend(objectArtefacts_),
                std::back_inserter(m_artefacts),
                [](const ObjectArtefactData * ptr_)->bool
    {
        return ptr_;
    });
    std::sort(
                std::begin(m_artefacts),
                std::end(m_artefacts),
                [](const ObjectArtefactData *left_, const ObjectArtefactData *right_)->bool
    {
        return left_ && right_ && left_->m_stepIndex < right_->m_stepIndex;
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
        drawingdata::IFileSource *filesource_
        ) const
{
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        //if((*it)->setVertexShader(object_, filesource_))
        //{
        //    (*it)->addArguments(object_);
        //    break;
        //}
    }
    for(Iterator it = m_objectBegin; it != m_objectEnd; ++it)
    {
        //if((*it)->setFragmentShader(object_, filesource_))
        //{
        //    (*it)->addArguments(object_);
        //    break;
        //}
    }
}

void ObjectArtefacts::findEnd()
{
    if(!isEnd())
    {
        m_objectEnd = std::upper_bound(
                    m_objectBegin,
                    std::cend(m_artefacts),
                    (*m_objectBegin)->m_stepIndex,
                    [](int stepIndex_, const ObjectArtefactData *ptr_)->bool
        {
            return ptr_ && stepIndex_ < ptr_->m_stepIndex;
        });
    }
    else
    {
        m_objectEnd = std::cend(m_artefacts);
    }
}

void EffectObjectsData::addObjects(
        drawing_data::QuizImageObjects &data_,
        drawingdata::IFileSource *filesource_
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
        list.build(*(data_.objects.back()), filesource_);
    }
}
