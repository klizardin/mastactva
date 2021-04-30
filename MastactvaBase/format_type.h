#ifndef FORMAT_TYPE_H
#define FORMAT_TYPE_H


#include <QString>


namespace fmt
{

    template<typename Type_> inline
    QString toString(const Type_ &val_);

    template<typename DestType_, typename SrcType_> inline
    DestType_ toType(const SrcType_ &val_, const DestType_ &defaultValue_);

}

#endif // FORMAT_TYPE_H
