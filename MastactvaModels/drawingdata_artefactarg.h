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
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


class IDrawingDataArtefactArg
{
public:
    virtual ~IDrawingDataArtefactArg() = default;
    virtual void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addVariable(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const = 0;
    virtual void getAddonNames(
            QStringList &names_
            ) const = 0;
    virtual bool isGlobalArgument(
            ) const = 0;
    virtual void addGlobalArgument(
            drawing_data::QuizImageObjects &data_,
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

    void getAddonNames(
            QStringList &names_
            ) const override
    {
        if(g_renderAddonNameName == m_name)
        {
            names_.push_back(m_defaultValue.trimmed());
        }
    }

    bool isGlobalArgument(
            ) const override
    {
        static const char * s_globalArgumnentNames[] =
        {
            g_renderFillColor,
            g_renderGlobalStatesName
        };
        return std::find_if(
                    std::begin(s_globalArgumnentNames),
                    std::end(s_globalArgumnentNames),
                    [this](const char * name_) -> bool
        {
            return m_name == name_;
        }) != std::end(s_globalArgumnentNames);
    }

    void addGlobalArgument(
            drawing_data::QuizImageObjects &data_,
            const drawingdata::Details &details_
            ) const override
    {
        if(!isGlobalArgument())
        {
            return;
        }
        if(g_renderFillColor == m_name)
        {
            auto val = std::make_shared<QVector3D>();
            QVector<float> vec;
            if(details_.variables.operator bool() &&
                    details_.variables->get(m_name, details_.position.get(), vec))
            {
                drawingdata::utils::vecToUniform(vec, *val);
            }
            else
            {
                drawingdata::utils::toUniform(m_defaultValue, *val);
            }
            const float minv = std::min(val->x(), std::min(val->y(), val->z()));
            const float maxv = std::min(val->x(), std::min(val->y(), val->z()));
            if(minv < 0.0 || maxv > 1.0)
            {
                val->setX(val->x() - minv);
                val->setY(val->y() - minv);
                val->setZ(val->z() - minv);
                const float maxv = std::min(val->x(), std::min(val->y(), val->z()));
                val->setX(val->x() / maxv);
                val->setY(val->y() / maxv);
                val->setZ(val->z() / maxv);
            }
            data_.clearColor = QColor(val->x() * 255, val->y() * 255, val->z() * 255);
        }
        else if(g_renderGlobalStatesName == m_name)
        {
            setStringListValue(details_, data_.globalStates);
        }
        else if(g_renderGlobalCalculationsName == m_name)
        {
            setStringListValue(details_, data_.globalCalculations);
        }
    }

protected:
    void setStringListValue(
            const drawingdata::Details &details_,
            std::vector<QString> &states_
            ) const
    {
        QStringList states;
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), states))
        {
            drawingdata::utils::addStates(states, states_);
        }
        else
        {
            drawingdata::utils::splitTo(m_defaultValue, g_renderObjectsStatesSpliter, states_);
        }
    }

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
        case ArtefactArgTypeEn::stringsType:
            return std::make_unique<DrawingDataArtefactArgType_<QString>>(std::move(data_));
            break;
        default:
            Q_ASSERT(false); // unimplemeted types
            return {nullptr};
        }
    }
protected:
    template<typename ArgType_>
    void addVariableImpl(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_,
            ArgType_ *
            ) const
    {
        QString value;
        drawingdata::Position position;
        drawingdata::IPosition *pos = nullptr;
        bool global = global_;
        if(argSetsAndArgs_)
        {
            if(!argSetsAndArgs_->find(m_name))
            {
                return;
            }
            value = argSetsAndArgs_->getValue();
            global = !argSetsAndArgs_->doAddVariableToLocalPosition();
            if(global
                    && details_.position
                    )
            {
                auto posNew = details_.position->getCopyClearObjectIndex();
                position = drawingdata::Position::fromPosition(posNew.get());
                pos = &position;
            }
            else
            {
                pos = details_.position.get();
            }
        }
        else
        {
            QVector<typename ArtefactArgTypeEnTraits<ArgType_>::ItemType> data0;
            if(!global
                    && details_.variables.operator bool()
                    && details_.variables->get(m_name, details_.position.get(), data0)
                    )
            {
                return;
            }
            value = m_defaultValue;
            if(!global)
            {
                pos = details_.position.get();
            }
        }
        QVector<double> data;
        drawingdata::utils::toVec(value, data);
        details_.variables->add(
                    m_name,
                    pos,
                    std::move(data)
                    );
    }

    void addVariableImpl(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_,
            QString *
            ) const
    {
        QString value;
        drawingdata::Position position;
        drawingdata::IPosition *pos = nullptr;
        bool global = global_;
        if(argSetsAndArgs_)
        {
            if(!argSetsAndArgs_->find(m_name))
            {
                return;
            }
            value = argSetsAndArgs_->getValue();
            global = !argSetsAndArgs_->doAddVariableToLocalPosition();
            if(global
                    && details_.position
                    )
            {
                auto posNew = details_.position->getCopyClearObjectIndex();
                position = drawingdata::Position::fromPosition(posNew.get());
                pos = &position;
            }
            else
            {
                pos = details_.position.get();
            }
        }
        else
        {
            QStringList data0;
            if(!global
                    && details_.variables.operator bool()
                    && details_.variables->get(m_name, details_.position.get(), data0)
                    )
            {
                return;
            }
            value = m_defaultValue;
            if(!global)
            {
                pos = details_.position.get();
            }
        }
        QStringList data;
        drawingdata::utils::splitTo(value, QString(g_renderObjectsStatesSpliter), data);
        details_.variables->add(
                    m_name,
                    pos,
                    std::move(data)
                    );
    }

    friend std::unique_ptr<DrawingDataArtefactArg> drawingdata::utils::factory<>(EffectArgumentData &&data_, const DrawingDataArtefactArg *);
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

    void addVariable(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const override
    {
        DrawingDataArtefactArg::addVariableImpl(
                    details_,
                    global_,
                    argSetsAndArgs_,
                    static_cast<ArgType_*>(nullptr)
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
        if(g_renderObjectsStatesName == m_name
                || g_renderObjectCalculationsName == m_name
                )
        {
            Q_ASSERT(false); // wrong type of variable
        }
        else
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
    }

    void addVariable(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const override
    {
        DrawingDataArtefactArg::addVariableImpl(
                    details_,
                    global_,
                    argSetsAndArgs_,
                    static_cast<ArgType_*>(nullptr)
                    );
    }
};

template<>
class DrawingDataArtefactUniformArg<QString> : public DrawingDataArtefactArg
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
        if(g_renderObjectsStatesName == m_name)
        {
            setStringListValue(details_, object_.objectStates);
        }
        else if(g_renderObjectCalculationsName == m_name)
        {
            setStringListValue(details_, object_.objectCalculations);
        }
    }

    void addVariable(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const override
    {
        DrawingDataArtefactArg::addVariableImpl(
                    details_,
                    global_,
                    argSetsAndArgs_,
                    static_cast<QString*>(nullptr)
                    );
    }
};

namespace drawingdata {
namespace utils {

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

}
}


#endif // DRAWINGDATA_ARTEFACTARG_H
