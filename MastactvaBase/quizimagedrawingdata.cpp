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

bool drawing_data::QuizImageObject::calculate(opengl_drawing::IVariables *variables_)
{
    opengl_drawing::VariablesExtended va(this, variables_);

    for(const auto &calc_ : calculations)
    {
        if(!calc_.operator bool())
        {
            continue;
        }
        calc_->calculate(&va);
    }
    return false;
}

void drawing_data::QuizImageObject::preCalculation()
{
}

void drawing_data::QuizImageObject::postCalculation()
{
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
    for(
        int step = 0;
        calculateStep(variables_) && step < maxSteps;
        ++step)
    {};
    for(const auto &object_ : objects)
    {
        object_->postCalculation();
    }
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

    for(const auto &calc_ : calculations)
    {
        if(!calc_.operator bool())
        {
            continue;
        }
        calc_->calculate(&va);
    }
    for(const auto &object_ : objects)
    {
        object_->calculate(variables_);
    }
    return false;
}

void drawing_data::QuizImageObjects::clearUpdated()
{
    m_updated.clear();
}
