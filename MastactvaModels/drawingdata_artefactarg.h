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


/*
 * interface for drawing data artefact's argument.
 * it is required because there are several implementations of
 * this interface.
*/
class IDrawingDataArtefactArg
{
public:
    virtual ~IDrawingDataArtefactArg() = default;

    /*
     * the function to add argument value to the object's arguments.
     * the argument in the object is a value that is used for shaders
     * or calculations. if there is a variable for this argument then
     * the value of variable is used. the argument itself holds a default
     * value for object's arguments.
    */
    virtual void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * the function to add variable to the data of the effect.
     * variables are used for scripts and for setup of arguments of drawing objects.
     * variables can be taken arg set as a default value instead of an argument value
     * of an artefact itself. If arg sets data is not specified then an artefact's value
     * is used as a default value and a variable's value is used as a main candidate (
     * before a default value).
    */
    virtual void addVariable(
            const drawingdata::Details &details_,
            bool global_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const = 0;

    /*
     * the function that returns addon's names.
     * if an artefact's argument is a special value - name of addons
     * (g_renderAddonNameName).
    */
    virtual void getAddonNames(
            QStringList &names_
            ) const = 0;

    /*
     * the function returns true if the argument is a special value
     * of the global arguments. Now it are g_renderFillColor,
     * g_renderGlobalStatesName, and g_renderGlobalCalculationsName.
    */
    virtual bool isGlobalArgument(
            ) const = 0;

    /*
     * the function setups the global objects arguments if the argument
     * is a special value - a global argument.
     * global values are hold as a special values inside the effect objects data.
    */
    virtual void addGlobalArgument(
            drawing_data::QuizImageObjects &data_,
            const drawingdata::Details &details_
            ) const = 0;
};


/*
 * base implemtation for the inteface IDrawingDataArtefactArg for the
 * EffectArgumentData data.
*/
class DrawingDataArtefactArg :
        public EffectArgumentData,
        public IDrawingDataArtefactArg
{
public:
    DrawingDataArtefactArg() = default;
    DrawingDataArtefactArg(EffectArgumentData &&data_);

    /*
     * implementation of getAddonNames() function: return addons names to start from argument
     * it it is the special argument (see g_renderAddonNameName)
    */
    void getAddonNames(
            QStringList &names_
            ) const override
    {
        if(g_renderAddonNameName == m_name)
        {
            names_.push_back(m_defaultValue.trimmed());
        }
    }

    /*
     * implementation of the isGlobalArgument() function: return true
     * if the argument name is one of the special argument names
     * (see
     *      * g_renderFillColor -  object backgroud color that is holded at the image objects level variable
     *      * g_renderGlobalStatesName - global states names
     *      * g_renderGlobalCalculationsName - global calculations names (possibly with calculation arguments)
     * )
    */
    bool isGlobalArgument(
            ) const override
    {
        static const char * s_globalArgumnentNames[] =
        {
            g_renderFillColor,
            g_renderGlobalStatesName,
            g_renderGlobalCalculationsName,
            g_renderTexturesListName
        };
        return std::find_if(
                    std::begin(s_globalArgumnentNames),
                    std::end(s_globalArgumnentNames),
                    [this](const char * name_) -> bool
        {
            return m_name == name_;
        }) != std::end(s_globalArgumnentNames);
    }

    /*
     * function to set up the global arguments to the image objects data
    */
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
        else if(g_renderTexturesListName == m_name)
        {
            const QStringList textures = setStringListValue(details_, data_.targetTextures);
            for(const QString &textureTargetName : textures)
            {
                if(details_.textureTargets
                    && !details_.textureTargets->contains(textureTargetName)
                    )
                {
                    *(details_.textureTargets) << textureTargetName;
                }
            }
        }
    }

protected:
    /*
     * the helper function to setup some value of the vector<string> type.
    */
    QStringList setStringListValue(
            const drawingdata::Details &details_,
            std::vector<QString> &states_
            ) const
    {
        QStringList states;
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), states))
        {
            drawingdata::utils::addStates(states, states_);
            return states;
        }
        else
        {
            return drawingdata::utils::splitTo(m_defaultValue, g_renderObjectsStatesSpliter, states_);
        }
    }

private:
    /*
     * the helper function to choose specialization by the type for the attribute storage arguments.
     * it limits the available types for attributes.
    */
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

    /*
     * the helper function to choose specialization by the type for the uniform storage arguments.
     * it limits the available types for uniforms.
    */
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
    /*
     * the template implementation of the function to setup variable
    */
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
            // check if there is a value in the argument set
            if(!argSetsAndArgs_->find(m_name))
            {
                return;
            }
            // get founded value
            value = argSetsAndArgs_->getValue();
            // should we use the value as global one or should use just for the current object.
            // rewrite global argument with value from the pipeline
            global = !argSetsAndArgs_->doAddVariableToLocalPosition();
            if(global
                    && details_.position
                    )
            {
                // clear object index. the value can be used for all such objects
                auto posNew = details_.position->getCopyClearObjectIndex();
                position = drawingdata::Position::fromPosition(posNew.get());
                pos = &position;
            }
            else
            {
                // use stric position that points to the current object
                pos = details_.position.get();
            }
        }
        else
        {
            QVector<typename ArtefactArgTypeEnTraits<ArgType_>::ItemType> data0;
            // if exists variable or argument is not global
            if(!global
                    && details_.variables.operator bool()
                    && details_.variables->get(m_name, details_.position.get(), data0)
                    )
            {
                // do not reinitialize variable for the current position
                return;
            }
            // use dafault value
            value = m_defaultValue;
            if(!global)
            {
                // TODO: fix logic
                pos = details_.position.get();
            }
        }
        // setup the variable
        QVector<double> data;
        drawingdata::utils::toVec(value, data);
        details_.variables->add(
                    m_name,
                    pos,
                    std::move(data)
                    );
    }

    /*
     * the specialization of the template implementation of the function to setup variable for the string types
    */
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
                // do not reinitialize variable for the current position
                return;
            }
            value = m_defaultValue;
            if(!global)
            {
                // TODO: fix logic
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


/*
 * specialization for the attribute storages.
 * attributes is an array of variable size of some item types.
*/
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
        // main value is an array of some types. this attribute storage values are used by shaders (and by calculations).
        // it is optimized for opengl usage.
        auto val = std::make_shared<std::vector<ArgType_>>();
        QVector<typename ArtefactArgTypeEnTraits<ArgType_>::ItemType> data;
        // check if the variable value exists
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), data))
        {
            // use value of variable
            drawingdata::utils::vecToAttribute(data, *val);
        }
        else
        {
            // use dafault this value
            drawingdata::utils::toAttribute(m_defaultValue, *val);
        }
        // setup attribute value of a drawing object
        object_.attributes.push_back(
                    std::make_unique<drawing_data::Attribute<ArgType_>>(
                        m_name,
                        val
                        )
                    );

        // duplicate string value as a string value to make happy calculations by scripts (only) that uses some times
        // arrays of strings as it's arguments.

        // possibly, this part is not very needed
        QStringList strData;
        // check if variable exists
        if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), strData))
        {
            //it's ok we initilize strData while get
        }
        else if(val->empty())
        {
            // use default this value
            drawingdata::utils::splitTo(m_defaultValue, QString(g_renderObjectsStatesSpliter), strData);
        }
        // if there is only default this value then setup vector<string> for a drawing object
        if(!strData.isEmpty())
        {
            object_.set(m_name, std::move(strData));
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


/*
 * specialization for the uniform storage.
*/
template<typename ArgType_>
class DrawingDataArtefactUniformArg : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactUniformArg() = default;
    DrawingDataArtefactUniformArg(EffectArgumentData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    /*
     * add argument value to a draing object for the uniform storage for generic (not string) types.
    */
    void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override
    {
        // this variables should be of string type only.
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
            // check if exist variable
            if(details_.variables.operator bool() &&
                    details_.variables->get(m_name, details_.position.get(), vec))
            {
                // use variable
                drawingdata::utils::vecToUniform(vec, *val);
            }
            else
            {
                // use default this value
                drawingdata::utils::toUniform(m_defaultValue, *val);
            }
            // set uniform value to a drawing object
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

/*
 * specialization for the string type of the uniform storage.
*/
template<>
class DrawingDataArtefactUniformArg<QString> : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactUniformArg() = default;
    DrawingDataArtefactUniformArg(EffectArgumentData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    /*
     * setup of arguments with the string type of the uniform storage.
    */
    void addArgument(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override
    {
        // special setup for special variables
        if(g_renderObjectsStatesName == m_name)
        {
            setStringListValue(details_, object_.objectStates);
        }
        else if(g_renderObjectCalculationsName == m_name)
        {
            setStringListValue(details_, object_.objectCalculations);
        }
        else
        {
            // default setup for string arguments.
            QStringList strData;
            if(details_.variables.operator bool() &&
                details_.variables->get(m_name, details_.position.get(), strData))
            {
                // it's ok we initialize strData while get
            }
            else
            {
                drawingdata::utils::splitTo(m_defaultValue, QString(g_renderObjectsStatesSpliter), strData);
            }
            if(!strData.isEmpty())
            {
                object_.set(m_name, std::move(strData));
            }
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

/*
 * factory of the DrawingDataArtefactArg.
 * in the top level it chooses by a storage type more deeper by type of item(s).
*/
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
