#ifndef QUIZIMAGEDRAWINGDATA_H
#define QUIZIMAGEDRAWINGDATA_H


#include <type_traits>
#include <set>
#include <vector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


#if QT_CONFIG(opengl)


namespace drawing_data
{
    enum class ItemTypesEn
    {
        none,
        fromItem,
        GLfloat,
        GLint,
        GLuint,
        QVector2D,
        QVector3D,
        QVector4D,
        QColor,
        QPoint,
        QPointF,
        QSize,
        QSizeF,
        QMatrix2x2,
        QMatrix2x3,
        QMatrix2x4,
        QMatrix3x2,
        QMatrix3x3,
        QMatrix3x4,
        QMatrix4x2,
        QMatrix4x3,
        QMatrix4x4,
        toItem
    };

    namespace details
    {

    template<typename ResultType_, typename ItemType_> inline
    ResultType_ fromVector(const std::vector<ItemType_> &);
    template<typename ResultType_, typename ItemType_> inline
    std::vector<ResultType_> toVector(const ItemType_ &);

    template<> inline
    GLfloat fromVector<GLfloat, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLfloat{};
    }

    template<> inline
    GLint fromVector<GLint, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? static_cast<GLint>(data_.at(0)) : GLint{};
    }

    template<> inline
    GLuint fromVector<GLuint, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? static_cast<GLuint>(data_.at(0)) : GLuint{};
    }

    template<> inline
    GLint fromVector<GLint, GLint>(const std::vector<GLint> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLint{};
    }

    template<> inline
    GLuint fromVector<GLuint, GLuint>(const std::vector<GLuint> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLuint{};
    }

    template<> inline
    QVector2D fromVector<QVector2D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2
                ? QVector2D{data_.at(0), data_.at(1)}
                : QVector2D{}
                  ;
    }

    template<> inline
    QColor fromVector<QColor, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QColor result;
        if(data_.size() >= 3)
        {
            result.setRedF(data_.at(0));
            result.setGreenF(data_.at(1));
            result.setBlueF(data_.at(2));
        }
        if(data_.size() >= 4)
        {
            result.setAlphaF(data_.at(3));
        }
        return result;
    }

    template<> inline
    QVector3D fromVector<QVector3D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3
                ? QVector3D{data_.at(0), data_.at(1), data_.at(2)}
                : QVector3D{}
                  ;
    }

    template<> inline
    QVector4D fromVector<QVector4D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4
                ? QVector4D{data_.at(0), data_.at(1), data_.at(2), data_.at(3)}
                : QVector4D{}
                  ;
    }

    template<> inline
    QMatrix2x2 fromVector<QMatrix2x2, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2*2
                ? QMatrix2x2{&data_[0]}
                : QMatrix2x2{}
                  ;
    }

    template<> inline
    QMatrix3x3 fromVector<QMatrix3x3, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3*3
                ? QMatrix3x3{&data_[0]}
                : QMatrix3x3{}
                  ;
    }

    template<> inline
    QMatrix4x4 fromVector<QMatrix4x4, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4*4
                ? QMatrix4x4{&data_[0]}
                : QMatrix4x4{}
                  ;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLfloat>(const GLfloat &val_)
    {
        std::vector<GLfloat> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLint>(const GLint &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_)};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLuint>(const GLuint &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_)};
        return result;
    }

    template<> inline
    std::vector<GLint> toVector<GLint, GLint>(const GLint &val_)
    {
        std::vector<GLint> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLuint> toVector<GLuint, GLuint>(const GLuint &val_)
    {
        std::vector<GLuint> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector2D>(const QVector2D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector3D>(const QVector3D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y(), val_.z()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector4D>(const QVector4D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y(), val_.z(), val_.w()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix2x2>(const QMatrix2x2 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(2*2);
        std::copy(val_.constData(), val_.constData() + 2*2, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix3x3>(const QMatrix3x3 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(3*3);
        std::copy(val_.constData(), val_.constData() + 3*3, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix4x4>(const QMatrix4x4 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(4*4);
        std::copy(val_.constData(), val_.constData() + 4*4, std::back_inserter(result));
        return result;
    }

    }

    class ItemTypeConvert
    {
    public:
        constexpr static int minIndex{ to_underlying(ItemTypesEn::fromItem) + 1 };
        constexpr static int maxIndex{ to_underlying(ItemTypesEn::toItem) - 1 };
    };


    template<typename ItemType_>
    class ItemTypeTraits
    {
    public:
        constexpr static int tupleSize{ 0 };
        constexpr static int typeIndex{ to_underlying(ItemTypesEn::none) };
        using underlayingType = void;
    };


    template<std::underlying_type_t<ItemTypesEn> TypeIndex_>
    class ItemTypeIndexTraits
    {
    public:
        using type = void;
    };


#define DRAWING_DATA_ITEM_TYPE_TRAITS(ItemType_, tupleSize_, underlayingType_)      \
    template<>                                                                      \
    class ItemTypeTraits<ItemType_>                                                 \
    {                                                                               \
    public:                                                                         \
        constexpr static int tupleSize{tupleSize_};                                 \
        constexpr static std::underlying_type_t<ItemTypesEn> typeIndex{             \
                                        to_underlying(ItemTypesEn::ItemType_)};     \
        using underlayingType = underlayingType_;                                   \
    };                                                                              \
    template<>                                                                      \
    class ItemTypeIndexTraits<to_underlying(ItemTypesEn::ItemType_)>                \
    {                                                                               \
    public:                                                                         \
        using type = ItemType_;                                                     \
    };                                                                              \
/*end traints macro*/


    DRAWING_DATA_ITEM_TYPE_TRAITS(GLfloat,       1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(GLint,         1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(GLuint,        1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector2D,     2,  GLfloat)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector3D,     3,  GLfloat)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector4D,     4,  GLfloat)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QColor,        3,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QPoint,        2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QPointF,       2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QSize,         2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QSizeF,        2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x2,    4,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x3,    6,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x4,    8,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x2,    6,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x3,    9,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x4,    12, void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x2,    8,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x3,    12, void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x4,    16, void)

#undef DRAWING_DATA_ITEM_TYPE_TRAITS

    class ITypeInfo
    {
    public:
        virtual ~ITypeInfo() = default;
        virtual int typeIndex() const = 0;
        virtual int tupleSize() const = 0;
    };


    template<typename ItemType_>
    class ITypeInfoImpl : public virtual ITypeInfo
    {
    public:
        int typeIndex() const override
        {
            static_assert(
                sizeof(decltype(ItemTypeTraits<ItemType_>::typeIndex)) <= sizeof(int),
                "underlying type greater then int"
                );
            return ItemTypeTraits<ItemType_>::typeIndex;
        }

        int tupleSize() const override
        {
            return ItemTypeTraits<ItemType_>::tupleSize;
        }
    };


    class IAttribute : public virtual ITypeInfo
    {
    public:
        virtual ~IAttribute() = default;
        virtual const QString &name() const = 0;
        virtual const GLfloat *constData() const = 0;
        virtual int size() const = 0;
    };


    template<class ItemType_>
    class Attribute : public IAttribute, public ITypeInfoImpl<ItemType_>
    {
    public:
        Attribute(const QString &name_, std::shared_ptr<std::vector<ItemType_>> data_)
            : m_name(name_),
              m_data(std::move(data_))
        {}

    public:
        const QString &name() const override
        {
            return m_name;
        }

        const GLfloat *constData() const override
        {
            if(!m_data.operator bool()
                    || m_data->empty())
            {
                return nullptr;
            }
            return &(const_cast<Attribute<ItemType_> *>(this)->m_data.get()->at(0)[0]);
        }

        int size() const override
        {
            return m_data.operator bool() ? m_data->size() : 0;
        }

        void set(const std::vector<ItemType_> &value_, int tupleSize_)
        {
            createData();

            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            *m_data = value_;
        }

        bool get(std::vector<ItemType_> &value_, int tupleSize_) const
        {
            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_ = *m_data;
            return true;
        }

        template<typename ItemType2_>
        void set(const std::vector<ItemType2_> &value_, int tupleSize_)
        {
            setImpl(value_, tupleSize_,
                    typename std::integral_constant<
                        bool
                        ,std::is_convertible<ItemType2_, ItemType_>::value
                        >::type(),
                    typename std::integral_constant<
                        bool
                        , std::is_same<typename ItemTypeTraits<ItemType_>::underlayingType, ItemType2_>::value
                        >::type()
                    );
        }

        template<typename ItemType2_>
        bool get(std::vector<ItemType2_> &value_, int tupleSize_) const
        {
            return getImpl(value_, tupleSize_,
                    typename std::integral_constant<
                        bool
                        ,std::is_convertible<ItemType_, ItemType2_>::value
                        >::type(),
                    typename std::integral_constant<
                        bool
                        , std::is_same<typename ItemTypeTraits<ItemType2_>::underlayingType, ItemType_>::value
                        >::type()
                    );
        }

    private:
        void createData()
        {
            if(m_data.operator bool())
            {
                return;
            }

            m_data = std::make_shared<std::vector<ItemType_>>();
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::false_type)
        {
            createData();

            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            m_data->reserve(value_.size());
            m_data->clear();
            for(const ItemType2_ &val_ : value_)
            {
                m_data->push_back(static_cast<ItemType_>(val_));
            }
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::true_type)
        {
            createData();

            const bool cond = 0 < tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            m_data->resize(value_.size() / tupleSize_);
            for(std::size_t i = 0; i < m_data->size(); ++i)
            {
                for(std::size_t j = 0; j < std::size_t(tupleSize_); ++j)
                {
                    (*m_data)[i][j] = static_cast<
                            typename ItemTypeTraits<ItemType_>::underlayingType
                            >(
                                value_[i*tupleSize_ + j]
                            );
                }
            }
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &, int, std::false_type, std::false_type)
        {
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &, int, std::true_type, std::true_type)
        {
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::false_type) const
        {
            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_.reserve(m_data->size());
            value_.clear();
            for(const ItemType_ &val_ : *m_data)
            {
                value_.push_back(static_cast<ItemType2_>(val_));
            }
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::true_type) const
        {
            const bool cond = 0 < tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_.resize(m_data->size() / tupleSize_);
            for(std::size_t i = 0; i < m_data->size(); ++i)
            {
                for(std::size_t j = 0; j < std::size_t(tupleSize_); ++j)
                {
                    value_[i*tupleSize_ + j]
                        = static_cast<ItemType2_>(
                                (*m_data)[i][j]
                            );
                }
            }
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &, int, std::false_type, std::false_type) const
        {
            return false;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &, int, std::true_type, std::true_type) const
        {
            return false;
        }

    private:
        QString m_name;
        std::shared_ptr<std::vector<ItemType_>> m_data;
    };


    class IUniform : public virtual ITypeInfo
    {
    public:
        virtual ~IUniform() = default;
        virtual const QString &name() const = 0;
        virtual void set(QOpenGLShaderProgram *program, int location_) const = 0;
        // break SOLID - not a simple data
    };


    template<typename ItemType_>
    struct Uniform : public IUniform, public ITypeInfoImpl<ItemType_>
    {
        Uniform(const QString &name_, std::shared_ptr<ItemType_> data_)
            : m_name(name_)
            , m_data(std::move(data_))
        {
        }

        virtual const QString &name() const
        {
            return m_name;
        }

        virtual void set(QOpenGLShaderProgram *program, int location_) const
        {
            if(!program
                    || location_ < 0
                    || !m_data.operator bool()
                    )
            {
                return;
            }

            program->setUniformValue(location_, *m_data);
        }

        void set(const ItemType_ &value_)
        {
            createData();

            *m_data = value_;
        }

        template<typename ItemType2_>
        void set(const ItemType2_ &value_)
        {
            return setImpl(value_,
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType2_,
                                    ItemType_
                                    >::value
                                >::type());
        }

        template<typename ItemType2_>
        void set(const std::vector<ItemType2_> &value_)
        {
            return setImpl(value_,
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType2_,
                                    ItemType_
                                    >::value
                                >::type());
        }

        bool get(ItemType_ &value_) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = *m_data;
            return true;
        }

        template<typename ItemType2_>
        bool get(ItemType2_ &value_) const
        {
            return getImpl(value_,
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType_,
                                    ItemType2_
                                    >::value
                                >::type());
        }

        template<typename ItemType2_>
        bool get(std::vector<ItemType2_> &value_) const
        {
            return getImpl(value_,
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType_,
                                    ItemType2_
                                    >::value
                                >::type());
        }

    private:
        void createData()
        {
            if(m_data.operator bool())
            {
                return;
            }

            m_data = std::make_shared<ItemType_>();
        }

        template<typename ItemType2_>
        bool getImpl(ItemType2_ &value_, std::true_type) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = static_cast<ItemType2_>(*m_data);
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(ItemType2_ &, std::false_type) const
        {
            return false;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, std::true_type) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = details::toVector<ItemType2_, ItemType_>(*m_data);
            return true;
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &value_, std::true_type)
        {
            createData();

            *m_data = static_cast<ItemType_>(value_);
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &, std::false_type)
        {
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, std::true_type)
        {
            createData();

            *m_data = details::fromVector<ItemType_, ItemType2_>(value_);
        }

    private:
        QString m_name;
        std::shared_ptr<ItemType_> m_data;
    };


    namespace detail
    {
        template<int index_>
        class ItemTypeBase : public ItemTypeBase<index_ - 1>
        {
        public:
            using AttributeType = Attribute<typename ItemTypeIndexTraits<index_>::type>;
            using UniformType = Uniform<typename ItemTypeIndexTraits<index_>::type>;
            using base = ItemTypeBase<index_ - 1>;

            template<typename ItemType_>
            static void set(IAttribute *interface_, int itemIndex_, const std::vector<ItemType_> &value_, int tupleSize_)
            {
                if(itemIndex_ != index_)
                {
                    base::set(interface_, itemIndex_, value_, tupleSize_);
                    return;
                }

                AttributeType *attr = dynamic_cast<AttributeType *>(interface_);
                if(!attr)
                {
                    return;
                }
                attr->set(value_, tupleSize_);
            }

            template<typename ItemType_>
            static bool get(const IAttribute *interface_, int itemIndex_, std::vector<ItemType_> &value_, int tupleSize_)
            {
                if(itemIndex_ != index_)
                {
                    return base::get(interface_, itemIndex_, value_, tupleSize_);
                }

                const AttributeType *attr = dynamic_cast<const AttributeType *>(interface_);
                if(!attr)
                {
                    return false;
                }
                return attr->get(value_, tupleSize_);
            }

            template<typename ItemType_>
            static void set(IUniform *interface_, int itemIndex_, const ItemType_ &value_)
            {
                if(itemIndex_ != index_)
                {
                    base::set(interface_, itemIndex_, value_);
                    return;
                }

                UniformType *uniform = dynamic_cast<UniformType *>(interface_);
                if(!uniform)
                {
                    return;
                }
                uniform->set(value_);
            }

            template<typename ItemType_>
            static bool get(const IUniform *interface_, int itemIndex_, ItemType_ &value_)
            {
                if(itemIndex_ != index_)
                {
                    return base::get(interface_, itemIndex_, value_);
                }

                const UniformType *uniform = dynamic_cast<const UniformType *>(interface_);
                if(!uniform)
                {
                    return false;
                }
                return uniform->get(value_);
            }
        };


        template<>
        class ItemTypeBase<ItemTypeConvert::minIndex - 1>
        {
        public:
            template<typename ItemType_>
            static void set(IAttribute *, int , const std::vector<ItemType_> &, int )
            {
            }

            template<typename ItemType_>
            static bool get(const IAttribute *, int , std::vector<ItemType_> &, int )
            {
                return false;
            }

            template<typename ItemType_>
            static void set(IUniform *, int , const ItemType_ &)
            {
            }

            template<typename ItemType_>
            static bool get(const IUniform *, int , ItemType_ &)
            {
                return false;
            }
        };
    } // namespace detail


    using ItemTypeBaseSet = detail::ItemTypeBase<ItemTypeConvert::maxIndex>;


    struct Texture
    {
        QString name;
        QString filename;
    };


    struct QuizImageObject : public ::opengl_drawing::IVariables
    {
    public:
        QByteArray vertexShader;
        QByteArray fragmentShader;

        std::vector<std::unique_ptr<IAttribute>> attributes;
        std::vector<std::unique_ptr<IUniform>> uniforms;
        std::vector<Texture> textures;
        std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> calculations;

    public:
        bool get(const QString &name_, QVector<double> &data_) const override
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

        void set(const QString &name_, const QVector<double> &data_) override
        {
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

        void set(const QString &name_, QVector<double> &&data_) override
        {
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

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            for(std::unique_ptr<IAttribute> &attribute_ : attributes)
            {
                if(!attribute_.operator bool()
                        || attribute_->name() != name_
                        )
                {
                    continue;
                }

                ItemTypeBaseSet::set(attribute_.get(), attribute_->typeIndex(), value_, tupleSize_);
                // set all attributes with this name
            }
        }

        int getAttributeTupleSize(const QString &name_) const
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

        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            for(std::unique_ptr<IUniform> &uniform_ : uniforms)
            {
                if(!uniform_.operator bool()
                        || uniform_->name() != name_
                        )
                {
                    continue;
                }

                ItemTypeBaseSet::set(uniform_.get(), uniform_->typeIndex(), value_);
                // set all uniforms with this name
                // TODO: ptimize for std::shared_ptr data use
            }
        }

        template<typename ItemType_>
        bool getUniform(const QString &name_, ItemType_ &value_) const
        {
            for(const std::unique_ptr<IUniform> &uniform_ : uniforms)
            {
                if(!uniform_.operator bool()
                        || uniform_->name() != name_
                        )
                {
                    continue;
                }

                // return first value
                return ItemTypeBaseSet::get(uniform_.get(), uniform_->typeIndex(), value_);
            }
            return false;
        }

        void setTexture(const QString &name_, const QString &newFilename_);
        bool calculate(opengl_drawing::IVariables *variables_);
    };


    struct QuizImageObjects : public ::opengl_drawing::IVariables
    {
    public:
        QColor clearColor{255, 255, 255};
        std::vector<std::shared_ptr<QuizImageObject>> objects;
        std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> calculations;

    public:
        bool get(const QString &name_, QVector<double> &data_) const override
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

        void set(const QString &name_, const QVector<double> &data_) override
        {
            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->set(name_, data_);
            }
        }

        void set(const QString &name_, QVector<double> &&data_) override
        {
            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->set(name_, std::move(data_));
            }
        }

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->setAttribute(name_, value_, tupleSize_);
            }
        }

        int getAttributeTupleSize(const QString &name_) const
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

        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->setUniform(name_, value_);
            }
        }

        template<typename ItemType_>
        bool getUniform(const QString &name_, ItemType_ &value_) const
        {
            bool res = false;
            for(const std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                res |= object_->getUniform(name_, value_);
                if(res)
                {
                    // return first value from list
                    break;
                }
            }
            return res;
        }

        void setTexture(const QString &name_, const QString &newFilename_);
        bool calculate(opengl_drawing::IVariables *variables_);
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
