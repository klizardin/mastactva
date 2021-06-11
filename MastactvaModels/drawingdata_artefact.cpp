#include "drawingdata_artefact.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_artefactarg.h"
#include "../MastactvaBase/utils.h"


DrawingDataArtefact::DrawingDataArtefact(ArtefactData &&data_)
    : ArtefactData(std::move(data_))
{
    drawingdata::utils::rebuild(m_artefactArgData, static_cast<DrawingDataArtefactArg *>(nullptr));
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
        drawing_data::QuizImageObject &object_
        ) const
{
    if(!m_artefactArgData.operator bool())
    {
        return;
    }
    for(const ArtefactArgData *arg_ : *m_artefactArgData)
    {
        auto arg = dynamic_cast<const DrawingDataArtefactArg *>(arg_);
        if(!arg)
        {
            continue;
        }
        arg->addArgument(object_);
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
