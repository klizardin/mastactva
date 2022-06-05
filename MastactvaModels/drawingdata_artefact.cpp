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

#include "drawingdata_artefact.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefactarg.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/wavefrontobj.h"
#include "../MastactvaBase/luaapi.h"
#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "../MastactvaBase/utils.h"


DrawingDataArtefact::DrawingDataArtefact(ArtefactData &&data_)
    : ArtefactData(std::move(data_))
{
    drawingdata::utils::copy(m_artefactArgData, m_effectArgData);
    drawingdata::utils::rebuild(m_effectArgData, static_cast<DrawingDataArtefactArg *>(nullptr));
}

bool DrawingDataArtefact::setVertexShader(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    if(!details_.filesource
            || to_enum<ArtefactTypeEn>(m_typeId) != ArtefactTypeEn::shaderVertex)
    {
        return false;
    }
    object_.vertexShader = details_.filesource->getText(m_filename).toUtf8();
    return true;
}

bool DrawingDataArtefact::setFragmentShader(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    if(!details_.filesource
            || to_enum<ArtefactTypeEn>(m_typeId) != ArtefactTypeEn::shaderFragmet)
    {
        return false;
    }
    object_.fragmentShader = details_.filesource->getText(m_filename).toUtf8();
    return true;
}

void DrawingDataArtefact::addArguments(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    if(!m_effectArgData.operator bool())
    {
        return;
    }
    for(const EffectArgumentData *arg_ : *m_effectArgData)
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->addArgument(object_, details_);
    }
}

bool DrawingDataArtefact::hasArguments() const
{
    return (m_effectArgData.operator bool()
            && m_effectArgData->size() > 0
            && (to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::shaderVertex
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::shaderFragmet
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::texture1D
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::texture2D
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::texture3D
                )
            ) || hasVariables()
            ;
}

bool DrawingDataArtefact::hasGlobalArguments() const
{
    if(!(m_effectArgData.operator bool()
            && m_effectArgData->size() > 0
            && (to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLua)))
    {
        return false;
    }
    for(const EffectArgumentData *arg_ : *m_effectArgData)
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        if(arg->isGlobalArgument())
        {
            return true;
        }
    }
    return false;
}

bool DrawingDataArtefact::hasVariables() const
{
    return m_effectArgData.operator bool()
            && m_effectArgData->size() > 0
            && (to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLua
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLuaRuntime
                )
            ;
}

void DrawingDataArtefact::addVariables(
        const drawingdata::Details &details_,
        DrawingDataArgSetsAndArgs *argSetsAndArgs_ /*= nullptr*/
        ) const
{
    Q_UNUSED(argSetsAndArgs_);

    if(!(hasVariables()
            || hasArguments()))
    {
        return;
    }

    const bool global = !(
                to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLua
                || to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLuaRuntime
                );

    for(const EffectArgumentData *arg_ : *m_effectArgData)
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->addVariable(details_, global, argSetsAndArgs_);
    }
}

void DrawingDataArtefact::addGlobalArguments(
        drawing_data::QuizImageObjects &data_,
        const drawingdata::Details &details_
        ) const
{
    if(!hasGlobalArguments())
    {
        return;
    }

    for(const EffectArgumentData *arg_ : *m_effectArgData)
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->addGlobalArgument(data_, details_);
    }
}

void DrawingDataArtefact::addTexture(
        drawing_data::QuizImageObject &object_
        ) const
{
    if(to_enum<ArtefactTypeEn>(m_typeId) != ArtefactTypeEn::texture1D
            && to_enum<ArtefactTypeEn>(m_typeId) != ArtefactTypeEn::texture2D
            && to_enum<ArtefactTypeEn>(m_typeId) != ArtefactTypeEn::texture3D)
    {
        return;
    }
    object_.textures.push_back({m_name, static_cast<const QString &>(m_filename)});
}

void DrawingDataArtefact::addData(
        const drawingdata::Details &details_
        ) const
{
    if(!details_.filesource
            || !details_.variables
            || !details_.position
            )
    {
        return;
    }

    const ArtefactTypeEn currentType = to_enum<ArtefactTypeEn>(m_typeId);
    auto artefactTextStr = details_.filesource->getText(m_filename);
    auto artefactText = artefactTextStr.toUtf8();
    switch(currentType)
    {
    case ArtefactTypeEn::dataJson:
    {
        details_.variables->add(
                    QJsonDocument::fromJson(
                        std::move(artefactText)
                        )
                    );
        break;
    }

    case ArtefactTypeEn::dataObj3D:
    {
        details_.variables->add(
                    WavefrontOBJ::graphicsOBJtoJson(
                        std::move(artefactTextStr)
                        )
                    );
        break;
    }

    case ArtefactTypeEn::convertNamesJson:
    {
        details_.variables->addAliases(
                    QJsonDocument::fromJson(
                        std::move(artefactText)
                        ),
                    details_.position.get()
                    );
        break;
    }

    case ArtefactTypeEn::scriptLua:
    {
        LuaAPI luaAPI;
        luaAPI.set(details_.variables);
        luaAPI.load(
                    std::move(artefactTextStr)
                    )
                &&
                luaAPI.callArtefact(
                    details_.position.get()
                    );
        break;
    }

    default:
        break;
    }
}

class LuaRuntimeVariables : public drawingdata::IVariables
{
public:
    LuaRuntimeVariables() = default;
    void setImpl(opengl_drawing::IVariables *variables_);
    void clearImpl();

    bool get(const QString &name_, const drawingdata::IPosition *position_, QVector<int> &data_) const override;
    bool get(const QString &name_, const drawingdata::IPosition *position_, QVector<float> &data_) const override;
    bool get(const QString &name_, const drawingdata::IPosition *position_, QVector<double> &data_) const override;
    bool get(const QString &name_, const drawingdata::IPosition *position_, QStringList &data_) const override;
    void add(const QJsonDocument &data_) override;
    bool add(const QString &name_, const drawingdata::IPosition *position_, const QVector<double> &data_) override;
    bool add(const QString &name_, const drawingdata::IPosition *position_, QVector<double> &&data_) override;
    bool add(const QString &name_, const drawingdata::IPosition *position_, const QStringList &data_) override;
    bool add(const QString &name_, const drawingdata::IPosition *position_, QStringList &&data_) override;
    void addAliases(const QJsonDocument &data_, const drawingdata::IPosition *position_) override;
    bool getObjectsList(QStringList &objects_) const override;
    void clear() override;

private:
    opengl_drawing::IVariables *m_variables = nullptr;
};

void LuaRuntimeVariables::setImpl(opengl_drawing::IVariables *variables_)
{
    m_variables = variables_;
}

void LuaRuntimeVariables::clearImpl()
{
    m_variables = nullptr;
}

bool LuaRuntimeVariables::get(const QString &name_, const drawingdata::IPosition *position_, QVector<int> &data_) const
{
    Q_UNUSED(name_);
    Q_UNUSED(position_);
    Q_UNUSED(data_);
    return false; // no implementation at runtime
}

bool LuaRuntimeVariables::get(const QString &name_, const drawingdata::IPosition *position_, QVector<float> &data_) const
{
    Q_UNUSED(name_);
    Q_UNUSED(position_);
    Q_UNUSED(data_);
    return false; // no implementation at runtime
}

bool LuaRuntimeVariables::get(const QString &name_, const drawingdata::IPosition *position_, QVector<double> &data_) const
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        return m_variables->get(name_, data_);
    }
    else
    {
        return false;
    }
}

bool LuaRuntimeVariables::get(const QString &name_, const drawingdata::IPosition *position_, QStringList &data_) const
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        return m_variables->get(name_, data_);
    }
    else
    {
        return false;
    }
}

void LuaRuntimeVariables::add(const QJsonDocument &data_)
{
    Q_UNUSED(data_);
}

bool LuaRuntimeVariables::add(const QString &name_, const drawingdata::IPosition *position_, const QVector<double> &data_)
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        m_variables->set(name_, data_);
        return true;
    }
    else
    {
        return false;
    }
}

bool LuaRuntimeVariables::add(const QString &name_, const drawingdata::IPosition *position_, QVector<double> &&data_)
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        m_variables->set(name_, std::move(data_));
        return true;
    }
    else
    {
        return false;
    }
}

bool LuaRuntimeVariables::add(const QString &name_, const drawingdata::IPosition *position_, const QStringList &data_)
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        m_variables->set(name_, data_);
        return true;
    }
    else
    {
        return false;
    }
}

bool LuaRuntimeVariables::add(const QString &name_, const drawingdata::IPosition *position_, QStringList &&data_)
{
    Q_UNUSED(position_);
    if(m_variables)
    {
        m_variables->set(name_, data_);
        return true;
    }
    else
    {
        return false;
    }
}

void LuaRuntimeVariables::addAliases(const QJsonDocument &data_, const drawingdata::IPosition *position_)
{
    Q_UNUSED(data_);
    Q_UNUSED(position_);
    // no implementation at runtime
}

bool LuaRuntimeVariables::getObjectsList(QStringList &objects_) const
{
    Q_UNUSED(objects_);
    return false;   // no implementation at runtime
}

void LuaRuntimeVariables::clear()
{
    // no implementation at runtime
}

class TexturesProxy : public drawingdata::ITextures
{
public:
    void setImpl(drawingdata::ITextures *proxy_);
    void clearImpl();

    void setTexture(const QString &textureName_, const QString &newFilename_) override;
    void setTexture(const QString &textureName_, const QString &newFilename_, const QColor &newBackgroundColor_) override;
    void setTextureFromCurrentFrameBuffer(const QString &textureName_) override;
    void setTextureFromCurrentFrameBuffer(const QString &textureName_, const QColor &backgroundColor_) override;

private:
    drawingdata::ITextures *m_proxy = nullptr;
};

void TexturesProxy::setImpl(drawingdata::ITextures *proxy_)
{
    m_proxy = proxy_;
}

void TexturesProxy::clearImpl()
{
    m_proxy = nullptr;
}

void TexturesProxy::setTexture(const QString &textureName_, const QString &newFilename_)
{
    if(!m_proxy)
    {
        return;
    }
    m_proxy->setTexture(textureName_, newFilename_);
}

void TexturesProxy::setTexture(const QString &textureName_, const QString &newFilename_, const QColor &newBackgroundColor_)
{
    if(!m_proxy)
    {
        return;
    }
    m_proxy->setTexture(textureName_, newFilename_, newBackgroundColor_);
}

void TexturesProxy::setTextureFromCurrentFrameBuffer(const QString &textureName_)
{
    if(!m_proxy)
    {
        return;
    }
    m_proxy->setTextureFromCurrentFrameBuffer(textureName_);
}

void TexturesProxy::setTextureFromCurrentFrameBuffer(const QString &textureName_, const QColor &backgroundColor_)
{
    if(!m_proxy)
    {
        return;
    }
    m_proxy->setTextureFromCurrentFrameBuffer(textureName_, backgroundColor_);
}


/*
    the class for runtime calculation of variables with lua scripts
    it calls main function in the lua script
    it is called if at least one of the required variables were changed
*/
class LuaRuntimeCalculations :
        public opengl_drawing::IEffectCalculation,
        protected LuaAPI
{
public:
    LuaRuntimeCalculations(
            const QString &filename_,
            const QString& luaScript_,
            const QStringList &requiredVariables_
            );
    void calculate(opengl_drawing::IVariables *variables_) const;

private:
    mutable std::shared_ptr<drawingdata::IVariables> m_variables;
    mutable std::shared_ptr<drawingdata::ITextures> m_textures;
};

LuaRuntimeCalculations::LuaRuntimeCalculations(
        const QString &filename_,
        const QString& luaScript_,
        const QStringList &requiredVariables_
        )
{
    setFilename(filename_);
    setRequiredVariables(requiredVariables_);
    const bool loadResult = load(luaScript_);
    if(loadResult)
    {
        m_variables = std::make_shared<LuaRuntimeVariables>(); // use of stub variables class to initialize variables usage
        set(m_variables);
        m_textures = std::make_shared<TexturesProxy>();
        set(m_textures);
    }
}

void LuaRuntimeCalculations::calculate(opengl_drawing::IVariables *variables_) const
{
    if(!m_variables
            && dynamic_cast<LuaRuntimeVariables *>(m_variables.get())
            )
    {
        return;
    }
    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_->getRoot());
    if(!objects)
    {
        return; // work only with quiz image opengl drawing data
    }

    dynamic_cast<LuaRuntimeVariables *>(m_variables.get())->setImpl(variables_);
    dynamic_cast<TexturesProxy *>(m_textures.get())->setImpl(static_cast<drawingdata::ITextures *>(objects));
    callArtefactAtRuntime(nullptr);
    dynamic_cast<LuaRuntimeVariables *>(m_variables.get())->clearImpl();
    dynamic_cast<TexturesProxy *>(m_textures.get())->clearImpl();
}


template<typename DataType_>
void addCalculationsT(
        const DrawingDataArtefact *artefact_,
        DataType_ &object_,
        const drawingdata::Details &details_
        )
{
    if(!artefact_
            || !details_.filesource
            || !artefact_->m_effectArgData.operator bool())
    {
        return;
    }
    const ArtefactTypeEn currentType = to_enum<ArtefactTypeEn>(artefact_->m_typeId);
    if(ArtefactTypeEn::scriptLuaRuntime == currentType)
    {
        auto artefactTextStr = details_.filesource->getText(artefact_->m_filename);

        QStringList args;
        args.reserve(artefact_->m_effectArgData->size());

        for(const EffectArgumentData *arg_ : qAsConst(*artefact_->m_effectArgData))
        {
            auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
            if(!arg)
            {
                continue;
            }
            args.push_back(arg->m_name);
        }

        object_.calculations.push_back(
                    std::make_shared<LuaRuntimeCalculations>(
                        artefact_->m_filename,
                        artefactTextStr,
                        args
                        )
                    );
    }
}

void DrawingDataArtefact::addCalculations(
        drawing_data::QuizImageObject &object_,
        const drawingdata::Details &details_
        ) const
{
    addCalculationsT(this, object_, details_);
}

void DrawingDataArtefact::addMainCalculations(
        drawing_data::QuizImageObjects &objects_,
        const drawingdata::Details &details_
        ) const
{
    addCalculationsT(this, objects_, details_);
}

bool DrawingDataArtefact::hasAddon() const
{
    return m_effectArgData.operator bool()
            && m_effectArgData->size() > 0
            && (to_enum<ArtefactTypeEn>(m_typeId) == ArtefactTypeEn::scriptLibrary)
            ;
}

void DrawingDataArtefact::getAddonNames(QStringList &names_) const
{
    if(!hasAddon())
    {
        return;
    }
    for(const EffectArgumentData *arg_ : qAsConst(*m_effectArgData))
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->getAddonNames(names_);
    }
}

void DrawingDataArtefact::runAddons(
        const drawingdata::Details &details_,
        const QStringList &addonNames_
        ) const
{
    if(!hasAddon())
    {
        return;
    }
    QStringList names;
    for(const EffectArgumentData *arg_ : qAsConst(*m_effectArgData))
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->getAddonNames(names);
    }
    const bool exist = std::any_of(
        std::begin(names),
        std::end(names),
        [&addonNames_](const QString &name_)->bool
    {
        return addonNames_.contains(name_);
    });
    if(!exist)
    {
        return;
    }

    const ArtefactTypeEn currentType = to_enum<ArtefactTypeEn>(m_typeId);
    auto artefactTextStr = details_.filesource->getText(m_filename);
    switch(currentType)
    {
    case ArtefactTypeEn::scriptLibrary:
    {
        LuaAPI luaAPI;
        luaAPI.set(details_.variables);
        luaAPI.set(details_.modules);
        // TODO: luaAPI.setEffect();
        luaAPI.load(std::move(artefactTextStr))
            && luaAPI.callArtefact(
                details_.position.get()
                );
        break;
    }

    default:
        break;
    }
}
