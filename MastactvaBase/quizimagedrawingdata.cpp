#include "quizimagedrawingdata.h"
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
    for(const auto &calc_ : m_availableCalculations)
    {
        if(calc_.operator bool()
                && m_thisIVariables->isUpdated(calc_->getRequiredVariables(), nullptr)
                )
        {
            updated.push_back(calc_.get());
        }
    }

    clearUpdated();

    for(const auto &calc_ : updated)
    {
        if(!calc_)
        {
            continue;
        }
        calc_->calculate(&va);

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

void drawing_data::detail::Calculations::setVariable(const QString &name_)
{
    m_updated.insert(name_);
}

bool drawing_data::detail::Calculations::isUpdated(
        const QStringList &vars_,
        ::opengl_drawing::IVariables *base_
        ) const
{
    Q_UNUSED(base_);
    QSet<QString> vars;
    for(const QString &s_ : vars_)
    {
        vars.insert(s_);
    }
    return m_updated.intersects(vars);
}

void drawing_data::detail::Calculations::clearUpdated()
{
    m_updated.clear();
}


drawing_data::QuizImageObject::QuizImageObject()
    : detail::Calculations(this)
{
}

bool drawing_data::QuizImageObject::get(const QString &name_, QVector<double> &data_) const
{
    std::vector<float> data;
    for(const std::unique_ptr<IAttribute> &attribute_ : attributes)
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
    for(const std::unique_ptr<IUniform> &uniform_ : uniforms)
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

bool drawing_data::QuizImageObject::isUpdated(const QStringList &vars_, IVariables *base_) const
{
    return detail::Calculations::isUpdated(vars_, base_);
}

int drawing_data::QuizImageObject::getAttributeTupleSize(const QString &name_) const
{
    for(const std::unique_ptr<IAttribute> &attribute_ : attributes)
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


drawing_data::QuizImageObjects::QuizImageObjects()
    : detail::Calculations(this)
{
}

bool drawing_data::QuizImageObjects::get(const QString &name_, QVector<double> &data_) const
{
    bool result = false;
    for(const std::shared_ptr<QuizImageObject> &object_ : objects)
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

    for(std::shared_ptr<QuizImageObject> &object_ : objects)
    {
        if(!object_.operator bool())
        {
            continue;
        }

        object_->set(name_, std::move(data_));
    }
}

bool drawing_data::QuizImageObjects::isUpdated(const QStringList &vars_, IVariables *base_) const
{
    if(detail::Calculations::isUpdated(vars_, base_))
    {
        return true;
    }

    for(const auto &object_ : objects)
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
    for(const std::shared_ptr<QuizImageObject> &object_ : objects)
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
        for(const auto &object_ : objects)
        {
            sz = std::max(sz, szMain + (int)object_->calculations.size());
        }
        return sz;
    }();

    preCalculation();
    for(const auto &object_ : objects)
    {
        object_->preCalculation();
    }

    for(
        int step = 0;
        calculateStep(variables_) && step < maxSteps;
        ++step)
    {};

    for(const auto &object_ : objects)
    {
        object_->postCalculation();
    }
    postCalculation();
}

bool drawing_data::QuizImageObjects::calculateStep(opengl_drawing::IVariables *variables_)
{
    bool anyProcessed = detail::Calculations::calculate(variables_);

    for(const auto &object_ : objects)
    {
        anyProcessed |= object_->calculate(variables_);
    }

    return anyProcessed;
}
