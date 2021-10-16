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
            const QDateTime &created_,
            const QString &mergeid_
            );
    EffectArgSetData(EffectArgSetData &&data_);
    virtual ~EffectArgSetData() = default;
    EffectArgSetData &operator=(EffectArgSetData &&data_);
    virtual std::unique_ptr<EffectArgSetData> getDataCopy() const;

private:
    void createArrays();

public:
    int m_id = -1;
    int m_effectId = -1;
    int m_easingTypeId = -1;
    QString m_description;
    QDateTime m_created;
    QString m_mergeid;

    std::shared_ptr<QVector<EffectArgValueData *>> m_effectArgValuesData;

    EffectArgSetData(const EffectArgData &data_) = delete;
    EffectArgSetData &operator = (const EffectArgData &data_) = delete;
};


#endif // EFFECTARGSET_DATA_H
