#ifndef EFFECT_DATA_H
#define EFFECT_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include "../MastactvaModels/effectobjects_data.h"


struct EffectData
{
public:
    EffectData();
    EffectData(
            int id_,
            const QString &name_,
            const QString &description_,
            const QDateTime &created_
            );
    EffectData(EffectData &&data_);
    virtual ~EffectData() = default;

    EffectData &operator = (EffectData &&data_);

    // to extract data from model object
    std::unique_ptr<EffectData> copy() const;

public:
    int m_id = -1;
    QString m_name;
    QString m_description;
    QDateTime m_created;

    std::shared_ptr<QVector<EffectObjectsData *>> m_effectObjectsData;
    //std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;
    //std::shared_ptr<QVector<EffectArgSetData *>> m_effectArgSetsData;

    EffectData(const EffectData &data_) = delete;
    EffectData &operator = (const EffectData &data_) = delete;
};



#endif // EFFECT_DATA_H
