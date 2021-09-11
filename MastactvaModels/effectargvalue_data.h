#ifndef EFFECTARGVALUE_DATA_H
#define EFFECTARGVALUE_DATA_H


#include <memory>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "../MastactvaModels/effectarg_data.h"


class EffectArgValueData
{
public:
    EffectArgValueData();
    EffectArgValueData(
            int id_,
            int argSetId_,
            int argId_,
            const QString &value_,
            const QString &description_,
            const QDateTime &created_
            );
    virtual ~EffectArgValueData() = default;
    virtual std::unique_ptr<EffectArgValueData> getDataCopy() const;

public:
    int m_id = -1;
    int m_argSetId = -1;
    int m_argId = -1;
    QString m_value;
    QString m_description;
    QDateTime m_created;
    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;
};


#endif // EFFECTARGVALUE_DATA_H
