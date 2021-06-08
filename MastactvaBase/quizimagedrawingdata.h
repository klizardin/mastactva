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
        virtual int typeIndex() const
        {
            static_assert(
                sizeof(decltype(ItemTypeTraits<ItemType_>::typeIndex)) <= sizeof(int),
                "underlying type greater then int"
                );
            return ItemTypeTraits<ItemType_>::typeIndex;
        }

        virtual int tupleSize() const override
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
        void setImpl(const ItemType2_ &value_, std::true_type)
        {
            createData();

            *m_data = static_cast<ItemType_>(value_);
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &, std::false_type)
        {
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


    struct QuizImageObject
    {
    public:
        QByteArray vertexShader;
        QByteArray fragmentShader;

        std::vector<std::unique_ptr<IAttribute>> attributes;
        std::vector<std::unique_ptr<IUniform>> uniforms;
        std::vector<Texture> textures;

    public:
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
    };


    struct QuizImageObjects
    {
    public:
        QColor clearColor{255, 255, 255};
        std::vector<std::shared_ptr<QuizImageObject>> objects;

    public:
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
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
