#ifndef LAYOUT_ENUMS_H
#define LAYOUT_ENUMS_H


namespace layout
{
    enum class JsonTypesEn : int
    {
        jt_null,
        jt_bool,
        jt_int,
        jt_double,
        jt_string,
        jt_datetime,
        jt_array,
        jt_object,
        jt_undefined
    };

    enum class SpecialFieldEn : int
    {
        none,
        appId,
        modelInfo,
        objectModelInfo
    };
}


#endif // LAYOUT_ENUMS_H
