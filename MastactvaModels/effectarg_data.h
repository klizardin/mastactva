#ifndef EFFECTARG_DATA_H
#define EFFECTARG_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaModels/effectargument_data.h"
#include "../MastactvaBase/imagesource.h"


/*
 * structure that scontains effect argument data
 * (data holder)
*/
struct EffectArgData : public EffectArgumentData
{
public:
    EffectArgData() = default;
    EffectArgData(int id_,
                  int effectId_,
                  int objectArtefactId_,
                  int argTypeId_,
                  int argStorageId_,
                  const QString &name_,
                  const QString &defaultValue_,
                  const QString &description_,
                  const QDateTime &created_,
                  const MergeId &mergeid_,
                  bool hasFileReference_,
                  bool hasFile_,
                  const FileSource &file_,
                  const QString &fileHash_
                  );
    virtual ~EffectArgData() = default;
    EffectArgData(EffectArgData &&data_);
    EffectArgData &operator = (EffectArgData &&data_);
    virtual std::unique_ptr<EffectArgData> getDataCopy() const;

public:
    int m_id = -1;
    int m_effectId = -1;                            // reference to the effect id
    int m_objectArtefactId = -1;                    // reference to the object artefact id
    // args are unique by name inside artefact id
    QString m_description;                          // description
    QDateTime m_created;                            // creation date and time
    MergeId m_mergeid;                              // unique id to merge

    EffectArgData(const EffectArgData &data_) = delete;
    EffectArgData &operator = (const EffectArgData &data_) = delete;
};


#endif // EFFECTARG_DATA_H
