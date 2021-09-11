#ifndef EFFECTARGSET_DATA_H
#define EFFECTARGSET_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../MastactvaModels/effectargvalue_data.h"


class EffectArgSetData
{
public:
    EffectArgSetData();
    EffectArgSetData(
            int id_,
            int effectId_,
            int easingTypeId_,
            const QString &description_,
            const QDateTime &created_
            );
    virtual ~EffectArgSetData() = default;
    virtual std::unique_ptr<EffectArgSetData> getDataCopy() const;

public:
    int m_id = -1;
    int m_effectId = -1;
    int m_easingTypeId = -1;
    QString m_description;
    QDateTime m_created;

    std::shared_ptr<QVector<EffectArgValueData *>> m_effectArgValuesData;
};


#endif // EFFECTARGSET_DATA_H
