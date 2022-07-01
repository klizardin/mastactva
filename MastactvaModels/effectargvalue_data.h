#ifndef EFFECTARGVALUE_DATA_H
#define EFFECTARGVALUE_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaBase/imagesource.h"


struct EffectArgValueData
{
public:
    EffectArgValueData();
    EffectArgValueData(
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
            );
    EffectArgValueData(EffectArgValueData &&data_);
    virtual ~EffectArgValueData() = default;
    EffectArgValueData &operator=(EffectArgValueData &&data_);
    virtual std::unique_ptr<EffectArgValueData> getDataCopy() const;

public:
    int m_id = -1;
    int m_argSetId = -1;
    int m_argId = -1;
    QString m_value;
    QString m_description;
    QDateTime m_created;
    MergeId m_mergeid;
    bool m_hasFileReference = false;
    bool m_hasFile = false;
    FileSourceReference m_file;
    QString m_fileHash;

    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;
    std::shared_ptr<QVector<EffectArgumentData *>> m_effectArgValuesData;

    EffectArgValueData(const EffectArgValueData &data_) = delete;
    EffectArgValueData &operator = (const EffectArgValueData &data_) = delete;

private:
    void createArrays();
    void createValues();
};


#endif // EFFECTARGVALUE_DATA_H
