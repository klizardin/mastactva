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

bool drawing_data::QuizImageObjects::calculate(opengl_drawing::IVariables *variables_)
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
