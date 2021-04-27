#ifndef LAYOUT_TYPE_H
#define LAYOUT_TYPE_H


#include <QString>


namespace layout
{
    template<typename T_>
    inline int compareValues(const T_ &val1_, const T_ &val2_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return val1_ < val2_ ? -1 : val2_ < val1_  ? 1 : 0;
    }
}


#endif // LAYOUT_TYPE_H
