#ifndef LAYOUT_VARIANT_H
#define LAYOUT_VARIANT_H


#include "QVariant"
#include "../MastactvaBase/timeutils.h"


namespace layout
{
    template<typename T_>
    inline QVariant getValue(const T_ &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(val_);
    }

    inline QVariant getValue(const bool &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue((val_?1:0));
    }

    inline QVariant getValue(const QDateTime &val_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return QVariant::fromValue(date_time::dateTimeToJsonString(val_));
    }

    template<typename T_>
    inline void setValue(const QVariant &var_, T_ &dta_)
    {
        dta_ = qvariant_cast<T_>(var_);
    }
}


#endif // LAYOUT_VARIANT_H
