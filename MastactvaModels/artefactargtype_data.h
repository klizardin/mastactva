/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
    ivec2Type,              /* base type int */
    ivec3Type,              /* base type int */
    ivec4Type,              /* base type int */
    floatType,
    vec2Type,               /* base type float */
    vec3Type,               /* base type float */
    vec4Type,               /* base type float */
    mat2Type,               /* base type float */
    mat3Type,               /* base type float */
    mat4Type,               /* base type float */
    stringsType,            /* strings separated by g_renderObjectsStatesSpliter defined in names.h */
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

/*
 * define traits for the used in the application types
*/
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
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec2Type, 2)          // is not used now
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec3Type, 3)          // is not used now
//ARTEFACT_ARG_TYPE_EN_TRAITS(?, int, ArtefactArgTypeEn::ivec4Type, 4)          // is not used now
ARTEFACT_ARG_TYPE_EN_TRAITS(float, float, ArtefactArgTypeEn::floatType, 1)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector2D, float, ArtefactArgTypeEn::vec2Type, 2)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector3D, float, ArtefactArgTypeEn::vec3Type, 3)
ARTEFACT_ARG_TYPE_EN_TRAITS(QVector4D, float, ArtefactArgTypeEn::vec4Type, 4)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix2x2, float, ArtefactArgTypeEn::mat2Type, 2*2)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix3x3, float, ArtefactArgTypeEn::mat3Type, 3*3)
ARTEFACT_ARG_TYPE_EN_TRAITS(QMatrix4x4, float, ArtefactArgTypeEn::mat4Type, 4*4)
//ARTEFACT_ARG_TYPE_EN_TRAITS(QVector<QString>, QString, ArtefactArgTypeEn::stringsType, -1)    // do not define traits for this types
//ARTEFACT_ARG_TYPE_EN_TRAITS(QStringList, QString, ArtefactArgTypeEn::stringsType, -1)    // do not define traits for this types
//ARTEFACT_ARG_TYPE_EN_TRAITS(QString, QString, ArtefactArgTypeEn::textureType, 1)    // do not define traits for this types


#endif // ARTEFACTARGTYPE_DATA_H
