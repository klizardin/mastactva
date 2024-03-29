#include "effectargvalue_data.h"
#include "../MastactvaBase/data_utils.h"


EffectArgValueData::EffectArgValueData()
{
    createArrays();
}

EffectArgValueData::EffectArgValueData(
        int id_,
        int argSetId_,
        int argId_,
        const QString &value_,
        const QString &description_,
        const QDateTime &created_,
        const MergeId &mergeid_,
        bool hasFileReference_,
        bool hasFile_,
        const FileSource &file_,
        const QString &fileHash_
        )
    : m_id(id_),
      m_argSetId(argSetId_),
      m_argId(argId_),
      m_value(value_),
      m_description(description_),
      m_created(created_),
      m_mergeid(mergeid_),
      m_hasFileReference(hasFileReference_),
      m_hasFile(hasFile_),
      m_file(file_),
      m_fileHash(fileHash_)
{
    m_file.initObjectDetails(&m_hasFile, &m_hasFileReference, &m_value);
    createArrays();
}

EffectArgValueData::EffectArgValueData(EffectArgValueData &&data_)
{
    operator=(std::move(data_));
}

EffectArgValueData &EffectArgValueData::operator=(EffectArgValueData &&data_)
{
    m_id = std::move(data_.m_id);
    m_argSetId = std::move(data_.m_argSetId);
    m_argId = std::move(data_.m_argId);
    m_value = std::move(data_.m_value);
    m_description = std::move(data_.m_description);
    m_created = std::move(data_.m_created);
    m_mergeid = std::move(data_.m_mergeid);
    m_effectArgsData = std::move(data_.m_effectArgsData);
    m_effectArgValuesData = std::move(data_.m_effectArgValuesData);
    m_hasFileReference = std::move(data_.m_hasFileReference);
    m_hasFile = std::move(data_.m_hasFile);
    m_file = std::move(data_.m_file);
    m_fileHash = std::move(data_.m_fileHash);

    return *this;
}

std::unique_ptr<EffectArgValueData> EffectArgValueData::getDataCopy() const
{
    std::unique_ptr<EffectArgValueData> result = std::make_unique<EffectArgValueData>();
    result->m_id = m_id;
    result->m_argSetId = m_argSetId;
    result->m_argId = m_argId;
    result->m_value = m_value;
    result->m_description = m_description;
    result->m_created = m_created;
    result->m_mergeid = m_mergeid;
    result->m_hasFileReference = m_hasFileReference;
    result->m_hasFile = m_hasFile;
    result->m_file = m_file;
    result->m_fileHash = m_fileHash;
    ::data_object::utils::copyDataVector(m_effectArgsData.get(), result->m_effectArgsData.get());
    ::data_object::utils::copyDataVector(m_effectArgValuesData.get(), result->m_effectArgValuesData.get());

    return result;
}

void EffectArgValueData::createArrays()
{
    m_effectArgsData = ::data_object::utils::createDataVector(static_cast<const EffectArgData *>(nullptr));
    createValues();
}

void EffectArgValueData::createValues()
{
    m_effectArgValuesData = ::data_object::utils::createDataVector(static_cast<const EffectArgumentData *>(nullptr));
    if(m_effectArgsData)
    {
        for(const EffectArgData *p_ : qAsConst(*m_effectArgsData))
        {
            if(!p_)
            {
                continue;
            }
            auto ptr = std::make_unique<EffectArgumentData>(static_cast<const EffectArgumentData &>(*p_));
            m_effectArgValuesData->push_back(ptr.release());
            m_effectArgValuesData->back()->m_defaultValue = m_value;
            m_effectArgValuesData->back()->m_hasFileReference = m_hasFileReference;
            m_effectArgValuesData->back()->m_hasFile = m_hasFile;
            m_effectArgValuesData->back()->m_file = m_file;
            m_effectArgValuesData->back()->m_fileHash = m_fileHash;
        }
    }
}
