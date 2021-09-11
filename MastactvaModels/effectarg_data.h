#ifndef EFFECTARG_DATA_H
#define EFFECTARG_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>


class EffectArgData
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
                  const QDateTime &created_
                  );
    virtual ~EffectArgData() = default;
    EffectArgData(EffectArgData &&data_);
    EffectArgData &operator = (EffectArgData &&data_);
    virtual std::unique_ptr<EffectArgData> getDataCopy() const;

public:
    int m_id = -1;
    int m_effectId = -1;
    int m_objectArtefactId = -1;
    int m_argTypeId = -1;
    int m_argStorageId = -1;
    QString m_name; // args are unique by name inside artefact id
    QString m_defaultValue;
    QString m_description;
    QDateTime m_created;

    EffectArgData(const EffectArgData &data_) = delete;
    EffectArgData &operator = (const EffectArgData &data_) = delete;
};


#endif // EFFECTARG_DATA_H
