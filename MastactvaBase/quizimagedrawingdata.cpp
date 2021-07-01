#include "quizimagedrawingdata.h"
#include "../MastactvaBase/utils.h"


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

// TODO: refactoring extend base class
bool drawing_data::QuizImageObject::calculate(opengl_drawing::IVariables *variables_)
{
    opengl_drawing::VariablesExtended va(this, variables_);

    std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> updated;
    std::copy_if(
                std::begin(m_availableCalculations),
                std::end(m_availableCalculations),
                std::back_inserter(updated),
                [this](const std::shared_ptr<opengl_drawing::IEffectCalculation> &calc_)->bool
    {
        return isUpdated(calc_->getRequiredVariables(), nullptr);
    });

    clearUpdated();

    for(const auto &calc_ : updated)
    {
        if(!calc_.operator bool())
        {
            continue;
        }
        calc_->calculate(&va);

        auto fit = std::find_if(
                    std::begin(m_availableCalculations),
                    std::end(m_availableCalculations),
                    [&calc_](const auto &calc1_)->bool
        {
            return calc_.get() == calc1_.get();
        });
        m_availableCalculations.erase(fit);
    }

    return !updated.empty();
}

void drawing_data::QuizImageObject::preCalculation()
{
    std::copy(std::begin(calculations), std::end(calculations),
              std::back_inserter(m_availableCalculations));
}

void drawing_data::QuizImageObject::postCalculation()
{
    m_availableCalculations.clear();
    clearUpdated();
}

bool drawing_data::QuizImageObject::isUpdated(const QStringList &vars_, IVariables *base_) const
{
    Q_UNUSED(base_);
    for(const QString &var_ : m_updated)
    {
        if(vars_.contains(var_))
        {
            return true;
        }
    }
    return false;
}

void drawing_data::QuizImageObject::clearUpdated()
{
    m_updated.clear();
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
    for(const auto &object_ : objects)
    {
        object_->preCalculation();
    }
    std::copy(std::begin(calculations), std::end(calculations),
              std::back_inserter(m_availableCalculations));
    for(
        int step = 0;
        calculateStep(variables_) && step < maxSteps;
        ++step)
    {};
    for(const auto &object_ : objects)
    {
        object_->postCalculation();
    }
    m_availableCalculations.clear();
    clearUpdated();
}

bool drawing_data::QuizImageObjects::isUpdated(const QStringList &vars_, IVariables *base_) const
{
    Q_UNUSED(base_);
    for(const QString &var_ : m_updated)
    {
        if(vars_.contains(var_))
        {
            return true;
        }
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
    opengl_drawing::VariablesExtended va(this, variables_);

    std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> updated;
    std::copy_if(
                std::begin(m_availableCalculations),
                std::end(m_availableCalculations),
                std::back_inserter(updated),
                [this](const std::shared_ptr<opengl_drawing::IEffectCalculation> &calc_)->bool
    {
        return isUpdated(calc_->getRequiredVariables(), nullptr);
    });

    clearUpdated();

    for(const auto &calc_ : updated)
    {
        if(!calc_.operator bool())
        {
            continue;
        }
        calc_->calculate(&va);

        auto fit = std::find_if(
                    std::begin(m_availableCalculations),
                    std::end(m_availableCalculations),
                    [&calc_](const auto &calc1_)->bool
        {
            return calc_.get() == calc1_.get();
        });
        m_availableCalculations.erase(fit);
    }

    bool anyProcessed = !updated.empty();
    for(const auto &object_ : objects)
    {
        anyProcessed |= object_->calculate(variables_);
    }

    return anyProcessed;
}

void drawing_data::QuizImageObjects::clearUpdated()
{
    m_updated.clear();
}
