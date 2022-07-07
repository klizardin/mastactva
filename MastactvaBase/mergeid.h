#ifndef MERGEID_H
#define MERGEID_H


#include <QString>
#include <QJsonValue>
#include "../MastactvaBase/layout_enums.h"


/*
 * class with unique id to merge data items
*/
class MergeId : public QString
{
public:
    MergeId();
    void set(const QString &val_);
    void reset();
};


/*
 * layout extensions for MergeId
*/
namespace layout
{
    inline void setValue(const QJsonValue &var_, MergeId &dta_)
    {
        if(var_.isNull() || !var_.isString()) { dta_.reset(); }
        else { dta_.set(var_.toString()); }
    }

    inline QVariant getValue(const MergeId &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(static_cast<const QString &>(val_));
    }

    inline void setValue(const QVariant &var_, MergeId &dta_)
    {
        dta_.set(qvariant_cast<QString>(var_));
    }

    inline JsonTypesEn getJsonType(const MergeId *)
    {
        return JsonTypesEn::jt_string;
    }
}


#endif // MERGEID_H
