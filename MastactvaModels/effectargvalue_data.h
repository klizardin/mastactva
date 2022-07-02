#ifndef EFFECTARGVALUE_DATA_H
#define EFFECTARGVALUE_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaBase/imagesource.h"


/*
 * structure of the effect argument value
 * (data holder)
*/
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
    int m_argSetId = -1;                // argumengt set id
    int m_argId = -1;                   // argument id
    QString m_value;                    // value
    QString m_description;              // description
    QDateTime m_created;                // creation date and time
    MergeId m_mergeid;                  // unique id to merge
    bool m_hasFileReference = false;    // has file reference TODO: explain (why we need duplications)
    bool m_hasFile = false;             // has file TODO: explain (why we need duplications)
    FileSourceReference m_file;         // file source TODO: explain (why we need duplications)
    QString m_fileHash;                 // file hash value TODO: explain (why we need duplications)

    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;              // reference to the effect argument data
    std::shared_ptr<QVector<EffectArgumentData *>> m_effectArgValuesData;    // effect argument default values

    EffectArgValueData(const EffectArgValueData &data_) = delete;
    EffectArgValueData &operator = (const EffectArgValueData &data_) = delete;

private:
    void createArrays();
    void createValues();
};


#endif // EFFECTARGVALUE_DATA_H
