#ifndef EFFECTARGUMENT_DATA_H
#define EFFECTARGUMENT_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaBase/imagesource.h"


/*
 * Seffect argument holder
*/
struct EffectArgumentData
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
    int m_argStorageId = -1;            /* storage type */
    int m_argTypeId = -1;               /* argument type */
    QString m_name;                     /* name of the argument */
    QString m_defaultValue;             /* default value of the argument */
    bool m_hasFileReference = false;    /* does argument have a file reference TODO: explain */
    bool m_hasFile = false;             /* does argument have a fie TODO: explain */
    FileSourceReference m_file;         /* file of the argument data(?) TODO: explain */
    QString m_fileHash;                 /* hash of file data of the argument */
};


#endif // EFFECTARGUMENT_DATA_H
