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

#include "quizimagedrawingdata.h"
#include "../MastactvaBase/opengldrawing_calculations.h"
#include "../MastactvaBase/utils.h"


drawing_data::detail::Calculations::Calculations(
        ::opengl_drawing::IVariables *ivariables_
        )
    : m_thisIVariables(ivariables_)
{
    Q_ASSERT(m_thisIVariables);
}

bool drawing_data::detail::Calculations::calculate(
        opengl_drawing::IVariables *variables_
        )
{
    if(!m_thisIVariables)
    {
        return false;
    }

    opengl_drawing::VariablesExtended va(m_thisIVariables, variables_);

    std::vector<opengl_drawing::IEffectCalculation *> updated;
    updated.reserve(m_availableCalculations.size());
    for(const auto &calc_ : qAsConst(m_availableCalculations))
    {
        if(calc_.operator bool()
                && (
                    m_thisIVariables->isUpdated(calc_->getRequiredVariables(), nullptr)
                    || calc_->doNeedUpdate()
                    )
                )
        {
            updated.push_back(calc_.get());
        }
    }

    clearUpdated();

    for(const auto &calc_ : qAsConst(updated))
    {
        if(!calc_)
        {
            continue;
        }
        calc_->calculate(&va);
        calc_->setUpdated();

        auto fit = std::find_if(
                    std::begin(m_availableCalculations),
                    std::end(m_availableCalculations),
                    [&calc_](const auto &calc1_)->bool
        {
            return calc_ == calc1_.get();
        });
        m_availableCalculations.erase(fit);
    }

    return !updated.empty();
}

void drawing_data::detail::Calculations::preCalculation()
{
    std::copy(std::begin(calculations), std::end(calculations),
              std::back_inserter(m_availableCalculations));
}

void drawing_data::detail::Calculations::postCalculation()
{
    m_availableCalculations.clear();
    clearUpdated();
}

void drawing_data::detail::Calculations::init(const std::vector<QString> &effectCalculationNames_)
{
    for(const QString &effectCalculateName_ : qAsConst(effectCalculationNames_))
    {
        std::unique_ptr<opengl_drawing::IEffectCalculation>
                calculation = opengl_drawing::getEffectCalculationFactory().get(
                    effectCalculateName_
                    );
        if(!calculation
                || find(calculation.get()))
        {
            continue;
        }
        calculations.push_back(
                    std::shared_ptr<opengl_drawing::IEffectCalculation>{
                        std::move(calculation)
                    });
    }
}

bool drawing_data::detail::Calculations::find(
        const opengl_drawing::IEffectCalculation *calculation_
        ) const
{
    if(!calculation_)
    {
        return false;
    }
    for(const auto &calc_ : qAsConst(calculations))
    {
        if(!calc_)
        {
            continue;
        }
        if(calc_->doExtend(calculation_))
        {
            return true;
        }
    }
    return false;
}

bool drawing_data::detail::Calculations::hasCalculations() const
{
    if(calculations.empty())
    {
        return false;
    }
    return std::any_of(
                std::begin(calculations),
                std::end(calculations),
                [](const std::shared_ptr<opengl_drawing::IEffectCalculation> &calc_)->bool
    {
        return (bool)calc_;
    });
}

void drawing_data::detail::Calculations::setVariable(const QString &name_)
{
    m_updated.insert(name_);
}

bool drawing_data::detail::Calculations::isUpdated(
        const QSet<QString> &vars_,
        ::opengl_drawing::IVariables *base_
        ) const
{
    Q_UNUSED(base_);
    return m_updated.intersects(vars_);
}

void drawing_data::detail::Calculations::clearUpdated()
{
    m_updated.clear();
}


drawing_data::QuizImageObject::QuizImageObject()
    : detail::Calculations(this)
{
}

void drawing_data::QuizImageObject::postInitialization()
{
    if(m_postInitializationDone)
    {
        return;
    }

    detail::Calculations::init(objectCalculations);
    m_postInitializationDone = true;
}

bool drawing_data::QuizImageObject::get(const QString &name_, QVector<double> &data_) const
{
    std::vector<float> data;
    for(const std::unique_ptr<IAttribute> &attribute_ : qAsConst(attributes))
    {
        if(!attribute_.operator bool()
                || attribute_->name() != name_
                )
        {
            continue;
        }

        // return first value
        const bool res = ItemTypeBaseSet::get(attribute_.get(), attribute_->typeIndex(), data, attribute_->tupleSize());
        data_.clear();
        if(res)
        {
            data_.reserve(data.size());
            std::copy(std::begin(data), std::end(data),
                      std::back_inserter(data_));
        }
        return res;
    }
    for(const std::unique_ptr<IUniform> &uniform_ : qAsConst(uniforms))
    {
        if(!uniform_.operator bool()
                || uniform_->name() != name_
                )
        {
            continue;
        }

        // return first value
        const bool res = ItemTypeBaseSet::get(uniform_.get(), uniform_->typeIndex(), data);
        data_.clear();
        if(res)
        {
            data_.reserve(data.size());
            std::copy(std::begin(data), std::end(data),
                      std::back_inserter(data_));
        }
        return res;
    }
    return false;
}

void drawing_data::QuizImageObject::set(const QString &name_, const QVector<double> &data_)
{
    setVariable(name_);

    std::vector<float> data;
    data.reserve(data_.size());
    std::copy(std::begin(data_), std::end(data_)
              , std::back_inserter(data));
    for(std::unique_ptr<IAttribute> &attribute_ : attributes)
    {
        if(!attribute_.operator bool()
                || attribute_->name() != name_
                )
        {
            continue;
        }

        ItemTypeBaseSet::set(attribute_.get(), attribute_->typeIndex(), data, attribute_->tupleSize());
        // set all attributes with this name
    }
    for(std::unique_ptr<IUniform> &uniform_ : uniforms)
    {
        if(!uniform_.operator bool()
                || uniform_->name() != name_
                )
        {
            continue;
        }

        ItemTypeBaseSet::set(uniform_.get(), uniform_->typeIndex(), data);
        // set all uniforms with this name
        // TODO: ptimize for std::shared_ptr data use
    }
}

void drawing_data::QuizImageObject::set(const QString &name_, QVector<double> &&data_)
{
    setVariable(name_);

    QVector<double> data0 = std::move(data_);
    std::vector<float> data;
    data.reserve(data0.size());
    std::copy(std::begin(data0), std::end(data0)
              , std::back_inserter(data));
    for(std::unique_ptr<IAttribute> &attribute_ : attributes)
    {
        if(!attribute_.operator bool()
                || attribute_->name() != name_
                )
        {
            continue;
        }

        ItemTypeBaseSet::set(attribute_.get(), attribute_->typeIndex(), data, attribute_->tupleSize());
        // set all attributes with this name
    }
    for(std::unique_ptr<IUniform> &uniform_ : uniforms)
    {
        if(!uniform_.operator bool()
                || uniform_->name() != name_
                )
        {
            continue;
        }

        ItemTypeBaseSet::set(uniform_.get(), uniform_->typeIndex(), data);
        // set all uniforms with this name
        // TODO: ptimize for std::shared_ptr data use
    }
}

bool drawing_data::QuizImageObject::get(const QString &name_, QStringList &data_) const
{
    const auto fit = strVariables.find(name_);
    if(std::end(strVariables) == fit)
    {
        return false;
    }
    else
    {
        data_ = fit->second;
        return true;
    }
}

void drawing_data::QuizImageObject::set(const QString &name_, const QStringList &data_)
{
    setVariable(name_);

    auto fit = strVariables.find(name_);
    if(std::end(strVariables) == fit)
    {
        strVariables.insert({name_, data_});
    }
    else
    {
        fit->second = data_;
    }
}

void drawing_data::QuizImageObject::set(const QString &name_, QStringList &&data_)
{
    setVariable(name_);

    auto fit = strVariables.find(name_);
    if(std::end(strVariables) == fit)
    {
        strVariables.insert({name_, std::move(data_)});
    }
    else
    {
        fit->second = data_;
    }
}

bool drawing_data::QuizImageObject::isUpdated(const QSet<QString> &vars_, IVariables *base_) const
{
    return detail::Calculations::isUpdated(vars_, base_);
}

bool drawing_data::QuizImageObject::allowedForTime(double t_) const
{
    QVector<double> periods;
    const auto isIncorrectData = [&periods]() ->bool { return periods.size() < 2; };
    if(!get(g_renderPeriodName, periods)
            || isIncorrectData()
            )
    {
        return true;
    }
    for(int i = 0; i + 1 < periods.size(); i+=2)
    {
        if(periods[i] <= t_
                && t_ <= periods[i+1]
                )
        {
            return true;
        }
    }
    return false;
}

bool drawing_data::QuizImageObject::changeAllowedForTime(double told_, double tnew_) const
{
    return allowedForTime(told_) != allowedForTime(tnew_);
}

int drawing_data::QuizImageObject::getAttributeTupleSize(const QString &name_) const
{
    for(const std::unique_ptr<IAttribute> &attribute_ : qAsConst(attributes))
    {
        if(!attribute_.operator bool()
                || attribute_->name() != name_
                )
        {
            continue;
        }

        // return first attribute
        return attribute_->tupleSize();
    }
    return -1;
}

void drawing_data::QuizImageObject::setTexture(const QString &name_, const QString &newFilename_)
{
    auto fit = std::find_if(
                std::begin(textures),
                std::end(textures),
                [&name_](const drawing_data::Texture &texture_)->bool
    {
        return texture_.name == name_;
    });
    if(std::end(textures) == fit)
    {
        return;
    }
    fit->filename = newFilename_;
}

QStringList drawing_data::QuizImageObject::getArgumentNames() const
{
    QStringList result;
    result.reserve(uniforms.size() + attributes.size());
    for(const std::unique_ptr<IUniform> &uniform_ : qAsConst(uniforms))
    {
        if(!uniform_.operator bool())
        {
            continue;
        }
        result.push_back(uniform_->name());
    }
    for(const std::unique_ptr<IAttribute> &attribute_ : qAsConst(attributes))
    {
        if(!attribute_.operator bool())
        {
            continue;
        }
        result.push_back(attribute_->name());
    }
    return result;
}

bool drawing_data::QuizImageObject::getArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const
{
    for(const std::unique_ptr<IUniform> &uniform_ : qAsConst(uniforms))
    {
        if(!uniform_.operator bool()
                || uniform_->name() != name_
                )
        {
            continue;
        }
        uniform_->getVector(values_);
        return true;
    }
    for(const std::unique_ptr<IAttribute> &attribute_ : qAsConst(attributes))
    {
        if(!attribute_.operator bool()
                || attribute_->name() != name_
                )
        {
            continue;
        }
        values_.resize(attribute_->size() * attribute_->tupleSize());
        std::copy(
                    attribute_->constData(),
                    attribute_->constData() + attribute_->size() * attribute_->tupleSize(),
                    values_.begin()
                    );
        return true;
    }
    return false;
}


drawing_data::QuizImageObjects::QuizImageObjects()
    : detail::Calculations(this)
{
}

void drawing_data::QuizImageObjects::postInitialization()
{
    if(m_postInitializationDone)
    {
        return;
    }

    detail::Calculations::init(globalCalculations);
    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->postInitialization();
    }

    m_postInitializationDone = true;
}

bool drawing_data::QuizImageObjects::get(const QString &name_, QVector<double> &data_) const
{
    bool result = false;
    for(const std::shared_ptr<QuizImageObject> &object_ : qAsConst(objects))
    {
        if(!object_.operator bool())
        {
            continue;
        }

        result |= object_->get(name_, data_);
    }
    return result;
}

void drawing_data::QuizImageObjects::set(const QString &name_, const QVector<double> &data_)
{
    setVariable(name_);

    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->set(name_, data_);
    }
}

void drawing_data::QuizImageObjects::set(const QString &name_, QVector<double> &&data_)
{
    setVariable(name_);
    const QVector<double> data = std::move(data_);

    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->set(name_, data);
    }
}

bool drawing_data::QuizImageObjects::get(const QString &name_, QStringList &data_) const
{
    bool result = false;
    for(const std::shared_ptr<QuizImageObject> &object_ : qAsConst(objects))
    {
        if(!object_.operator bool())
        {
            continue;
        }

        result |= object_->get(name_, data_);
    }
    return result;
}

void drawing_data::QuizImageObjects::set(const QString &name_, const QStringList &data_)
{
    setVariable(name_);

    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->set(name_, data_);
    }
}

void drawing_data::QuizImageObjects::set(const QString &name_, QStringList &&data_)
{
    setVariable(name_);
    const QStringList data = std::move(data_);

    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->set(name_, data);
    }
}

bool drawing_data::QuizImageObjects::isUpdated(const QSet<QString> &vars_, IVariables *base_) const
{
    if(detail::Calculations::isUpdated(vars_, base_))
    {
        return true;
    }

    for(const auto &object_ : qAsConst(objects))
    {
        if(object_->isUpdated(vars_, nullptr))
        {
            return true;
        }
    }
    return false;
}

int drawing_data::QuizImageObjects::getAttributeTupleSize(const QString &name_) const
{
    for(const std::shared_ptr<QuizImageObject> &object_ : qAsConst(objects))
    {
        if(!object_.operator bool())
        {
            continue;
        }

        int res = object_->getAttributeTupleSize(name_);
        if(res >= 0)
        {
            return res;
        }
    }
    return 0;
}

void drawing_data::QuizImageObjects::setTexture(const QString &name_, const QString &newFilename_)
{
    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->setTexture(name_, newFilename_);
    }
}

void drawing_data::QuizImageObjects::calculate(opengl_drawing::IVariables *variables_)
{
    const int maxSteps = 1 + [this]() -> int
    {
        const int szMain = calculations.size();
        int sz = szMain;
        for(const auto &object_ : qAsConst(objects))
        {
            sz = std::max(sz, szMain + (int)object_->calculations.size());
        }
        return sz;
    }();

    preCalculation();
    for(const auto &object_ : qAsConst(objects))
    {
        object_->preCalculation();
    }

    for(
        int step = 0;
        calculateStep(variables_) && step < maxSteps;
        ++step)
    {};

    for(const auto &object_ : qAsConst(objects))
    {
        object_->postCalculation();
    }
    postCalculation();
}

QStringList drawing_data::QuizImageObjects::getArgumentNames() const
{
    QStringList result;
    for(const std::shared_ptr<QuizImageObject> &object_ : qAsConst(objects))
    {
        if(!object_.operator bool())
        {
            continue;
        }
        result.append(object_->getArgumentNames());
    }
    result << getPseudoArgumentsNames();
    return result;
}

bool drawing_data::QuizImageObjects::getArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const
{
    for(const std::shared_ptr<QuizImageObject> &object_ : qAsConst(objects))
    {
        if(!object_.operator bool())
        {
            continue;
        }
        if(object_->getArgumentValue(name_, values_))
        {
            return true;
        }
    }
    return getPseudoArgumentValue(name_, values_);
}

QStringList drawing_data::QuizImageObjects::getPseudoArgumentsNames() const
{
    return QStringList{}
        << QString(g_renderGlobalStatesName) + QString(g_renderClearBackgroundColorStateName)
    ;
}

bool drawing_data::QuizImageObjects::getPseudoArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const
{
    if(QString(g_renderGlobalStatesName) + QString(g_renderClearBackgroundColorStateName) == name_)
    {
        values_ = details::toVector<GLfloat, QColor>(clearColor);
        return true;
    }
    return false;
}

bool drawing_data::QuizImageObjects::calculateStep(opengl_drawing::IVariables *variables_)
{
    bool anyProcessed = detail::Calculations::calculate(variables_);

    for(const auto &object_ : qAsConst(objects))
    {
        anyProcessed |= object_->calculate(variables_);
    }

    return anyProcessed;
}
