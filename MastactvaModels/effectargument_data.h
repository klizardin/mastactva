#ifndef EFFECTARGUMENT_DATA_H
#define EFFECTARGUMENT_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaBase/imagesource.h"


class EffectArgumentData
{
public:
    EffectArgumentData() = default;
    EffectArgumentData(
            int argStorageId_,
            int argTypeId_,
            const QString &name_,
            const QString &defaultValue_,
            bool hasFileReference_,
            bool hasFile_,
            const FileSource &file_,
            const QString &fileHash_
            );
    EffectArgumentData(EffectArgumentData &&data_);
    virtual ~EffectArgumentData() = default;
    EffectArgumentData & operator = (EffectArgumentData &&data_);
    std::unique_ptr<EffectArgumentData> getDataCopy() const;
    EffectArgumentData(const EffectArgumentData &data_) = default;
    EffectArgumentData &operator = (const EffectArgumentData &data_) = default;

public:
    int m_argStorageId = -1;
    int m_argTypeId = -1;
    QString m_name;
    QString m_defaultValue;
    bool m_hasFileReference = false;
    bool m_hasFile = false;
    FileSourceReference m_file;
    QString m_fileHash;
};


#endif // EFFECTARGUMENT_DATA_H
