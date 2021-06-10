#include "drawingdata_artefactarg.h"
#include <array>


DrawingDataArtefactArg::DrawingDataArtefactArg(ArtefactArgData &&data_)
    : ArtefactArgData(std::move(data_))
{
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
    QString str(val_);
    QTextStream s(&str);
    for(std::size_t i = 0; !s.atEnd(); ++i)
    {
        std::array<float, 3> v;
        for(std::size_t j = 0; j < v.size(); ++j)
        {
            s >> v.at(j);
        }
        if(s.atEnd())
        {
            return;
        }
        QVector3D item(v.at(0), v.at(1), v.at(2));
        data_.push_back(item);
    }
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
bool getArray(const QString &str_, std::vector<Type_> &array_)
{
    QString str(str_);
    QTextStream s(&str);
    std::size_t i = 0;
    for(i = 0; i < array_.size() && !s.atEnd(); ++i)
    {
        s >> array_.at(i);
    }
    return i >= array_.size();
}

template<typename Type_>
void toUniform(const QString &str_, Type_ &);

template<>
void toUniform(const QString &str_, QMatrix4x4 &mat_)
{
    const std::size_t size = 16;
    std::vector<float> vec;
    vec.resize(size);
    (void)getArray(str_, vec);
    for(std::size_t i = 0; i < vec.size() && i < size; ++i)
    {
        mat_.data()[i] = vec.at(i);
    }
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
