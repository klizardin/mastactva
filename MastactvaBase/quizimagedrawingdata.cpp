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

bool drawing_data::QuizImageObject::isUpdated(const QStringList &vars_, IVariables *base_) const
{
    return detail::Calculations::isUpdated(vars_, base_);
}


drawing_data::QuizImageObjects::QuizImageObjects()
    : detail::Calculations(this)
{
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

bool drawing_data::QuizImageObjects::calculateStep(opengl_drawing::IVariables *variables_)
{
    bool anyProcessed = detail::Calculations::calculate(variables_);

    for(const auto &object_ : objects)
    {
        anyProcessed |= object_->calculate(variables_);
    }

    return anyProcessed;
}
