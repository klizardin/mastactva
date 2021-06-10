#include "drawingdata_artefactarg.h"


DrawingDataArtefactArg::DrawingDataArtefactArg(ArtefactArgData &&data_)
    : ArtefactArgData(std::move(data_))
{
}

ArtefactArgData * DrawingDataArtefactArg::getData()
{
    return this;
}

void DrawingDataArtefactArg::addArgument(drawing_data::QuizImageObject &object_) const
{
    if(to_enum<ArtefactArgStorageEn>(m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
    {
        addAttribute(object_);
    }
    else if(to_enum<ArtefactArgStorageEn>(m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
    {
        addUniform(object_);
    }
}

template<typename Type_>
void toAttribute(const QString &val_, std::vector<Type_> &);

template<>
void toAttribute(const QString &val_, std::vector<QVector3D> &data_)
{
}

void DrawingDataArtefactArg::addAttribute(drawing_data::QuizImageObject &object_) const
{
    switch(to_enum<ArtefactArgTypeEn>(m_argTypeId))
    {
    case ArtefactArgTypeEn::vec3Type:
    {
        auto val = std::make_shared<std::vector<QVector3D>>();
        toAttribute(m_defaultValue, *val);
        object_.attributes.push_back(
                    std::make_unique<drawing_data::Attribute<QVector3D>>(
                        m_name,
                        val
                        )
                    );
    } break;
    default:
        Q_ASSERT(false); // not impelented
        break;
    };
}

template<typename Type_>
void toUniform(const QString &str_, Type_ &);

template<>
void toUniform(const QString &str_, QMatrix4x4 &mat_)
{
}

void DrawingDataArtefactArg::addUniform(drawing_data::QuizImageObject &object_) const
{
    switch(to_enum<ArtefactArgTypeEn>(m_argTypeId))
    {
    case ArtefactArgTypeEn::mat4Type:
    {
        auto val = std::make_shared<QMatrix4x4>();
        toUniform(m_defaultValue, *val);
        object_.uniforms.push_back(
                    std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                        m_name,
                        val
                        )
                    );
    } break;
    default:
        Q_ASSERT(false); // not impelented
        break;
    };
}
