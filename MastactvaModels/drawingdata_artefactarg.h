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

#ifndef DRAWINGDATA_ARTEFACTARG_H
#define DRAWINGDATA_ARTEFACTARG_H


#include <array>
#include "../MastactvaModels/artefactarg_data.h"
#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaModels/effectargvalue_data.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/utils.h"


class IDrawingDataArtefactArg
{
public:
    virtual ~IDrawingDataArtefactArg() = default;
    virtual void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
};


class DrawingDataArtefactArg :
        public EffectArgumentData,
        public IDrawingDataArtefactArg
{
public:
    DrawingDataArtefactArg() = default;
    DrawingDataArtefactArg(EffectArgumentData &&data_);

private:
    template<template <typename> class DrawingDataArtefactArgType_>
    static
    std::unique_ptr<DrawingDataArtefactArg> createForAttributeTypes(EffectArgumentData &&data_)
    {
        switch(to_enum<ArtefactArgTypeEn>(data_.m_argTypeId))
        {
        case ArtefactArgTypeEn::vec2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector2D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector3D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector4D>>(std::move(data_));
            break;
        default:
            Q_ASSERT(false); // unimplemeted types
            return {nullptr};
        }
    }

    template<template <typename> class DrawingDataArtefactArgType_>
    static
    std::unique_ptr<DrawingDataArtefactArg> createForUniformTypes(EffectArgumentData &&data_)
    {
        switch(to_enum<ArtefactArgTypeEn>(data_.m_argTypeId))
        {
        case ArtefactArgTypeEn::intType:
            return std::make_unique<DrawingDataArtefactArgType_<int>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::floatType:
            return std::make_unique<DrawingDataArtefactArgType_<float>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector2D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector3D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector4D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix2x2>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix3x3>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix4x4>>(std::move(data_));
            break;
        default:
            Q_ASSERT(false); // unimplemeted types
            return {nullptr};
        }
    }

    friend std::unique_ptr<DrawingDataArtefactArg> drawingdata::utils::factory<>(ArtefactArgData &&data_, const DrawingDataArtefactArg *);
    friend std::unique_ptr<DrawingDataArtefactArg> drawingdata::utils::factory<>(EffectArgData &&data_, const DrawingDataArtefactArg *);
    friend std::unique_ptr<DrawingDataArtefactArg> drawingdata::utils::factory<>(EffectArgumentData &&data_, const DrawingDataArtefactArg *);
    //friend std::unique_ptr<DrawingDataArtefactArg> drawingdata::utils::factory<>(EffectArgValueData &&data_, const DrawingDataArtefactArg *);
};


template<typename ArgType_>
class DrawingDataArtefactAtrributeArg : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactAtrributeArg() = default;
    DrawingDataArtefactAtrributeArg(EffectArgumentData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override
    {
        auto val = std::make_shared<std::vector<ArgType_>>();
        QVector<typename ArtefactArgTypeEnTraits<ArgType_>::ItemType> data;
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), data))
        {
            drawingdata::utils::vecToAttribute(data, *val);
        }
        else
        {
            drawingdata::utils::toAttribute(m_defaultValue, *val);
        }
        object_.attributes.push_back(
                    std::make_unique<drawing_data::Attribute<ArgType_>>(
                        m_name,
                        val
                        )
                    );
    }
};

template<typename ArgType_>
class DrawingDataArtefactUniformArg : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactUniformArg() = default;
    DrawingDataArtefactUniformArg(EffectArgumentData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override
    {
        auto val = std::make_shared<ArgType_>();
        QVector<typename ArtefactArgTypeEnTraits<ArgType_>::ItemType> vec;
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), vec))
        {
            drawingdata::utils::vecToUniform(vec, *val);
        }
        else
        {
            drawingdata::utils::toUniform(m_defaultValue, *val);
        }
        object_.uniforms.push_back(
                    std::make_unique<drawing_data::Uniform<ArgType_>>(
                        m_name,
                        val
                        )
                    );
    }
};

namespace drawingdata {
namespace utils {

template<> inline
std::unique_ptr<DrawingDataArtefactArg> factory(ArtefactArgData &&data_, const DrawingDataArtefactArg *)
{
    ArtefactArgData data = std::move(data_);
    EffectArgumentData &argument = data;

    //return std::make_unique<DrawingDataArtefactArg>(std::move(data_));
    if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
    {
        return DrawingDataArtefactArg::createForAttributeTypes<DrawingDataArtefactAtrributeArg>(std::move(argument));
    }
    else if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
    {
        return DrawingDataArtefactArg::createForUniformTypes<DrawingDataArtefactUniformArg>(std::move(argument));
    }
    else
    {
        return {nullptr};
    }
}

template<> inline
std::unique_ptr<DrawingDataArtefactArg> factory(EffectArgData &&data_, const DrawingDataArtefactArg *)
{
    EffectArgData data = std::move(data_);
    EffectArgumentData &argument = data;

    //return std::make_unique<DrawingDataArtefactArg>(std::move(data_));
    if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
    {
        return DrawingDataArtefactArg::createForAttributeTypes<DrawingDataArtefactAtrributeArg>(std::move(argument));
    }
    else if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
    {
        return DrawingDataArtefactArg::createForUniformTypes<DrawingDataArtefactUniformArg>(std::move(argument));
    }
    else
    {
        return {nullptr};
    }
}

template<> inline
std::unique_ptr<DrawingDataArtefactArg> factory(EffectArgumentData &&data_, const DrawingDataArtefactArg *)
{
    EffectArgumentData data = std::move(data_);
    EffectArgumentData &argument = data;

    //return std::make_unique<DrawingDataArtefactArg>(std::move(data_));
    if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
    {
        return DrawingDataArtefactArg::createForAttributeTypes<DrawingDataArtefactAtrributeArg>(std::move(argument));
    }
    else if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
    {
        return DrawingDataArtefactArg::createForUniformTypes<DrawingDataArtefactUniformArg>(std::move(argument));
    }
    else
    {
        return {nullptr};
    }
}

/*template<> inline
std::unique_ptr<DrawingDataArtefactArg> factory(EffectArgValueData &&data_, const DrawingDataArtefactArg *)
{
    EffectArgValueData data = std::move(data_);
    Q_ASSERT(data.m_effectArgValuesData->size() <= 1);
    for(EffectArgData *p_ : *(data.m_effectArgValuesData))
    {
        EffectArgumentData &argument = *p_;

        //return std::make_unique<DrawingDataArtefactArg>(std::move(data_));
        if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
        {
            return DrawingDataArtefactArg::createForAttributeTypes<DrawingDataArtefactAtrributeArg>(std::move(argument));
        }
        else if(to_enum<ArtefactArgStorageEn>(argument.m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
        {
            return DrawingDataArtefactArg::createForUniformTypes<DrawingDataArtefactUniformArg>(std::move(argument));
        }
        else
        {
            return {nullptr};
        }
    }
    return {nullptr};
}*/


}
}


#endif // DRAWINGDATA_ARTEFACTARG_H
