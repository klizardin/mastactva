#include "effectargument_data.h"


EffectArgumentData::EffectArgumentData(
        int argStorageId_,
        int argTypeId_,
        const QString &name_,
        const QString &defaultValue_,
        bool hasFileReference_,
        bool hasFile_,
        const FileSource &file_,
        const QString &fileHash_
        )
    : m_argStorageId(argStorageId_),
      m_argTypeId(argTypeId_),
      m_name(name_),
      m_defaultValue(defaultValue_),
      m_hasFileReference(hasFileReference_),
      m_hasFile(hasFile_),
      m_file(file_),
      m_fileHash(fileHash_)
{
    m_file.initObjectDetails(&m_hasFile, &m_hasFileReference, &m_defaultValue);
}

EffectArgumentData::EffectArgumentData(EffectArgumentData &&data_)
{
    operator=(std::move(data_));
}

EffectArgumentData & EffectArgumentData::operator = (EffectArgumentData &&data_)
{
    m_argTypeId = std::move(data_.m_argTypeId);
    m_argStorageId = std::move(data_.m_argStorageId);
    m_name = std::move(data_.m_name);
    m_defaultValue = std::move(data_.m_defaultValue);
    m_hasFileReference = std::move(data_.m_hasFileReference);
    m_hasFile = std::move(data_.m_hasFile);
    m_file = std::move(data_.m_file);
    m_fileHash = std::move(data_.m_fileHash);

    return *this;
}

std::unique_ptr<EffectArgumentData> EffectArgumentData::getDataCopy() const
{
    std::unique_ptr<EffectArgumentData> result = std::make_unique<EffectArgumentData>();

    result->m_argTypeId = m_argTypeId;
    result->m_argStorageId = m_argStorageId;
    result->m_name = m_name;
    result->m_defaultValue = m_defaultValue;
    result->m_hasFileReference = m_hasFileReference;
    result->m_hasFile = m_hasFile;
    result->m_file = m_file;
    result->m_fileHash = m_fileHash;

    return result;
}
