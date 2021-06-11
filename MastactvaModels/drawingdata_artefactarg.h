#ifndef DRAWINGDATA_ARTEFACTARG_H
#define DRAWINGDATA_ARTEFACTARG_H


#include <array>
#include "../MastactvaModels/artefactarg_data.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/utils.h"


class IDrawingDataArtefactArg
{
public:
    virtual ~IDrawingDataArtefactArg() = default;
    virtual void addArgument(drawing_data::QuizImageObject &object_) const = 0;
};


class DrawingDataArtefactArg :
        public ArtefactArgData,
        public IDrawingDataArtefactArg
{
public:
    DrawingDataArtefactArg() = default;
    DrawingDataArtefactArg(ArtefactArgData &&data_);

    template<template <typename> class DrawingDataArtefactArgType_>
    static
    std::unique_ptr<DrawingDataArtefactArg> createForAttributeTypes(ArtefactArgData &&data_)
    {
        switch(to_enum<ArtefactArgTypeEn>(data_.m_argTypeId))
        {
        case ArtefactArgTypeEn::vec2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector2D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector3D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector4D>>(std::move(data_));
            break;
        default:
            Q_ASSERT(false); // unimplemeted types
            return {nullptr};
        }
    }

    template<template <typename> class DrawingDataArtefactArgType_>
    static
    std::unique_ptr<DrawingDataArtefactArg> createForUniformaTypes(ArtefactArgData &&data_)
    {
        switch(to_enum<ArtefactArgTypeEn>(data_.m_argTypeId))
        {
        case ArtefactArgTypeEn::intType:
            return std::make_unique<DrawingDataArtefactArgType_<int>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::floatType:
            return std::make_unique<DrawingDataArtefactArgType_<float>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector2D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector3D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::vec4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QVector4D>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat2Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix2x2>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat3Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix3x3>>(std::move(data_));
            break;
        case ArtefactArgTypeEn::mat4Type:
            return std::make_unique<DrawingDataArtefactArgType_<QMatrix4x4>>(std::move(data_));
            break;
        default:
            Q_ASSERT(false); // unimplemeted types
            return {nullptr};
        }
    }
private:
    void addAttribute(drawing_data::QuizImageObject &object_) const;
    void addUniform(drawing_data::QuizImageObject &object_) const;
};

namespace drawingdata {
namespace utils {

template<typename Type_> inline
void toAttribute(const QString &val_, std::vector<Type_> &)
{
    Q_UNUSED(val_);
    Q_ASSERT(false); // not implemented
}

template<> inline
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

template<typename Type_> inline
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

template<typename Type_> inline
void toUniform(const QString &str_, Type_ &)
{
    Q_UNUSED(str_);
    Q_ASSERT(false); // not implemented
}

template<> inline
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

}}

template<typename ArgType_>
class DrawingDataArtefactAtrributeArg : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactAtrributeArg() = default;
    DrawingDataArtefactAtrributeArg(ArtefactArgData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    void addArgument(drawing_data::QuizImageObject &object_) const override
    {
        auto val = std::make_shared<std::vector<ArgType_>>();
        drawingdata::utils::toAttribute(m_defaultValue, *val);
        object_.attributes.push_back(
                    std::make_unique<drawing_data::Attribute<ArgType_>>(
                        m_name,
                        val
                        )
                    );
    }
};

template<typename ArgType_>
class DrawingDataArtefactUniformArg : public DrawingDataArtefactArg
{
public:
    DrawingDataArtefactUniformArg() = default;
    DrawingDataArtefactUniformArg(ArtefactArgData &&data_)
        :DrawingDataArtefactArg(std::move(data_))
    {
    }

    void addArgument(drawing_data::QuizImageObject &object_) const override
    {
        auto val = std::make_shared<ArgType_>();
        drawingdata::utils::toUniform(m_defaultValue, *val);
        object_.uniforms.push_back(
                    std::make_unique<drawing_data::Uniform<ArgType_>>(
                        m_name,
                        val
                        )
                    );
    }
};

namespace drawingdata {
namespace utils {

template<> inline
std::unique_ptr<DrawingDataArtefactArg> factory(ArtefactArgData &&data_, const DrawingDataArtefactArg *)
{
    //return std::make_unique<DrawingDataArtefactArg>(std::move(data_));
    if(to_enum<ArtefactArgStorageEn>(data_.m_argStorageId) == ArtefactArgStorageEn::attributeStorage)
    {
        return DrawingDataArtefactArg::createForAttributeTypes<DrawingDataArtefactAtrributeArg>(std::move(data_));
    }
    else if(to_enum<ArtefactArgStorageEn>(data_.m_argStorageId) == ArtefactArgStorageEn::uniformStorage)
    {
        return DrawingDataArtefactArg::createForUniformaTypes<DrawingDataArtefactUniformArg>(std::move(data_));
    }
    else
    {
        return {nullptr};
    }
}

}
}


#endif // DRAWINGDATA_ARTEFACTARG_H
