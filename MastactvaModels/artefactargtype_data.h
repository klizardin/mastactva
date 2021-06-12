#ifndef ARTEFACTARGTYPE_DATA_H
#define ARTEFACTARGTYPE_DATA_H


#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QVector>
#include <QString>
#include <QStringList>


enum class ArtefactArgTypeEn
{
    intType = 1,
    ivec2Type,
    ivec3Type,
    ivec4Type,
    floatType,
    vec2Type,
    vec3Type,
    vec4Type,
    mat2Type,
    mat3Type,
    mat4Type,
    stringsType,
    textureType,
    undefinedType
};


template<typename Type_>
class ArtefactArgTypeEnTraits{
public:
    using Type = Type_;
    using ItemType = Type_;
    constexpr static ArtefactArgTypeEn val = ArtefactArgTypeEn::undefinedType;
    constexpr static int size = 0;
};

#define ARTEFACT_ARG_TYPE_EN_TRAITS(Type_, ItemType_, val_, size_)  \
template<>                                                          \
class ArtefactArgTypeEnTraits<Type_>{                               \
public:                                                             \
    using Type = Type_;                                             \
    using ItemType = ItemType_;                                     \
    constexpr static ArtefactArgTypeEn val = val_;                  \
    constexpr static int size = size_;                              \
};                                                                  \
/*end macro*/

ARTEFACT_ARG_TYPE_EN_TRAITS(int, int, ArtefactArgTypeEn::intType, 1)
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec2Type, 2)
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec3Type, 3)
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec4Type, 4)
ARTEFACT_ARG_TYPE_EN_TRAITS(float, float, ArtefactArgTypeEn::floatType, 1)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector2D, float, ArtefactArgTypeEn::vec2Type, 2)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector3D, float, ArtefactArgTypeEn::vec3Type, 3)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector4D, float, ArtefactArgTypeEn::vec4Type, 4)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix2x2, float, ArtefactArgTypeEn::mat2Type, 2*2)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix3x3, float, ArtefactArgTypeEn::mat3Type, 3*3)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix4x4, float, ArtefactArgTypeEn::mat4Type, 4*4)
//ARTEFACT_ARG_TYPE_EN_TRAITS(QVector<QString>, QString, ArtefactArgTypeEn::stringsType, -1)
//ARTEFACT_ARG_TYPE_EN_TRAITS(QStringList, QString, ArtefactArgTypeEn::stringsType, -1)
//ARTEFACT_ARG_TYPE_EN_TRAITS(QString, QString, ArtefactArgTypeEn::textureType, 1)


#endif // ARTEFACTARGTYPE_DATA_H
