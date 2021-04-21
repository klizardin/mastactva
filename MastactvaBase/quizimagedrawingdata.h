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

class ArgumentValueDataArray;
class ArtefactArg;
class ArtefactArgStorageModel;
class ArtefactArgTypeModel;


class ArgumentBase
{
public:
    ArgumentBase() = default;
    virtual ~ArgumentBase() = default;

    const QString &getName() const;
    const QString &getStorage() const;
    const QString &getType() const;
    const QString &getValue() const;
    const QString &getDefaultValue() const;

    bool isValid() const;

    template<class T_>
    void setName(T_ &&name_)
    {
        m_name = std::forward<T_>(name_);
    }
    template<class T_>
    void setStorage(T_ &&storage_)
    {
        m_storage = std::forward<T_>(storage_);
    }
    template<class T_>
    void setType(T_ &&type_)
    {
        m_type = std::forward<T_>(type_);
    }
    template<class T_>
    void setValue(T_ &&value_)
    {
        m_value = std::forward<T_>(value_);
    }
    template<class T_>
    void setDefaultValue(T_ &&defaultValue_)
    {
        m_defaultValue = std::forward<T_>(defaultValue_);
    }

    /* used for scripts arguments
     * isOutput() == true
     *      * ... // TODO
     * isInput() == true
     *      * ... // TODO
    */
    bool isInput() const;
    bool isOutput() const;
    void setInput(bool isInput_);

    ArgumentValueDataArray *createValueDataArray() const;
    static std::unique_ptr<ArgumentBase> fromJson(const QJsonDocument &obj_, bool isInput_ = true);
    static std::unique_ptr<ArgumentBase> fromJson(const QJsonObject &obj_, bool isInput_ = true);

private:
    QString m_name;
    QString m_storage;
    QString m_type;
    QString m_value;
    QString m_defaultValue;
    bool m_isInput = false;
};


namespace drawing_data
{
    class ItemTypeConvert
    {
    public:
        constexpr static int minIndex = 0;
        constexpr static int maxIndex = 19;
    };

    template<typename ItemType_>
    class ItemTypeTraits
    {
    public:
        constexpr static int tupleSize = 0;
        constexpr static int typeIndex = -1;
        using underlayingType = void;
    };
    template<int TypeIndex_>
    class ItemTypeIndexTraits
    {
    public:
        using type = void;
    };

#define ITEM_TYPE_TRAITS(ItemType_, tupleSize_, typeIndex_, underlayingType_)   \
    template<>                                                                  \
    class ItemTypeTraits<ItemType_>                                             \
    {                                                                           \
    public:                                                                     \
        constexpr static int tupleSize = tupleSize_;                            \
        constexpr static int typeIndex = typeIndex_;                            \
        using underlayingType = underlayingType_;                               \
    };                                                                          \
    template<>                                                                  \
    class ItemTypeIndexTraits<typeIndex_>                                       \
    {                                                                           \
    public:                                                                     \
        using type = ItemType_;                                                 \
    };                                                                          \
/*end traints macro*/

    ITEM_TYPE_TRAITS(GLfloat,       1,  0,  void)
    ITEM_TYPE_TRAITS(GLint,         1,  1,  void)
    ITEM_TYPE_TRAITS(GLuint,        1,  2,  void)
    ITEM_TYPE_TRAITS(QVector2D,     2,  3,  GLfloat)
    ITEM_TYPE_TRAITS(QVector3D,     3,  4,  GLfloat)
    ITEM_TYPE_TRAITS(QVector4D,     4,  5,  GLfloat)
    ITEM_TYPE_TRAITS(QColor,        3,  6,  void)
    ITEM_TYPE_TRAITS(QPoint,        2,  7,  void)
    ITEM_TYPE_TRAITS(QPointF,       2,  8,  void)
    ITEM_TYPE_TRAITS(QSize,         2,  9,  void)
    ITEM_TYPE_TRAITS(QSizeF,        2,  10, void)
    ITEM_TYPE_TRAITS(QMatrix2x2,    4,  11, void)
    ITEM_TYPE_TRAITS(QMatrix2x3,    6,  12, void)
    ITEM_TYPE_TRAITS(QMatrix2x4,    8,  13, void)
    ITEM_TYPE_TRAITS(QMatrix3x2,    6,  14, void)
    ITEM_TYPE_TRAITS(QMatrix3x3,    9,  15, void)
    ITEM_TYPE_TRAITS(QMatrix3x4,    12, 16, void)
    ITEM_TYPE_TRAITS(QMatrix4x2,    8,  17, void)
    ITEM_TYPE_TRAITS(QMatrix4x3,    12, 18, void)
    ITEM_TYPE_TRAITS(QMatrix4x4,    16, 19, void)

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
              m_data(data_)
        {
        }

    public:
        virtual const QString &name() const override
        {
            return m_name;
        }

        virtual const GLfloat *constData() const override
        {
            if(!m_data.operator bool()
                    || m_data->empty())
            {
                return nullptr;
            }
            return &(const_cast<Attribute<ItemType_> *>(this)->m_data.get()->at(0)[0]);
        }

        virtual int size() const override
        {
            return m_data.operator bool() ? m_data->size() : 0;
        }

        void set(const std::vector<ItemType_> &value_, int tupleSize_)
        {
            if(!m_data.operator bool())
            {
                m_data = std::make_shared<std::vector<ItemType_>>();
            }
            if(0 == tupleSize_)
            {
                *m_data.get() = value_;
            }
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
                        ,std::is_base_of<typename ItemTypeTraits<ItemType_>::underlayingType, ItemType2_>::value
                            && !std::is_same<typename ItemTypeTraits<ItemType_>::underlayingType, void>::value
                        >::type()
                    );
        }

    private:
        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::false_type)
        {
            if(!m_data.operator bool())
            {
                m_data = std::make_shared<std::vector<ItemType_>>();
            }
            if(0 == tupleSize_)
            {
                m_data->reserve(value_.size());
                m_data->clear();
                for(const ItemType2_ &val_ : value_)
                {
                    m_data->push_back(static_cast<ItemType_>(val_));
                }
            }
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::true_type)
        {
            if(!m_data.operator bool())
            {
                m_data.reset(new std::vector<ItemType_>());
            }
            if(0 < tupleSize_)
            {
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
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::false_type)
        {
            Q_UNUSED(value_);
            Q_UNUSED(tupleSize_);
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::true_type)
        {
            Q_UNUSED(value_);
            Q_UNUSED(tupleSize_);
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
    };


    template<typename ItemType_>
    struct Uniform : public IUniform, public ITypeInfoImpl<ItemType_>
    {
        Uniform(const QString &name_, std::shared_ptr<ItemType_> data_)
            : m_name(name_)
            , m_data(data_)
        {
        }

        virtual const QString &name() const
        {
            return m_name;
        }

        virtual void set(QOpenGLShaderProgram *program, int location_) const
        {
            if(nullptr == program
                    || location_ < 0
                    || !m_data.operator bool()
                    )
            {
                return;
            }
            program->setUniformValue(location_, *m_data.get());
        }

        void set(const ItemType_ &value_)
        {
            if(!m_data.operator bool())
            {
                m_data.reset(new ItemType_());
            }
            *m_data.get() = value_;
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
            value_ = *m_data.get();
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
        template<typename ItemType2_>
        bool getImpl(ItemType2_ &value_, std::true_type) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }
            value_ = static_cast<ItemType2_>(*m_data.get());
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(ItemType2_ &value_, std::false_type) const
        {
            Q_UNUSED(value_);
            return false;
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &value_, std::true_type)
        {
            if(!m_data.operator bool())
            {
                m_data = std::make_shared<ItemType_>();
            }
            *m_data.get() = static_cast<ItemType_>(value_);
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &value_, std::false_type)
        {
            Q_UNUSED(value_);
        }

    private:
        QString m_name;
        std::shared_ptr<ItemType_> m_data;
    };


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
            AttributeType *attr = static_cast<AttributeType *>(interface_);
            if(nullptr == attr)
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
            UniformType *uniform = static_cast<UniformType *>(interface_);
            if(nullptr == uniform)
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
            const UniformType *uniform = static_cast<const UniformType *>(interface_);
            if(nullptr == uniform)
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
        static void set(IAttribute *interface_, int itemIndex_, const std::vector<ItemType_> &value_, int tupleSize_)
        {
            Q_UNUSED(interface_);
            Q_UNUSED(itemIndex_);
            Q_UNUSED(value_);
            Q_UNUSED(tupleSize_);
        }
        template<typename ItemType_>
        static void set(IUniform *interface_, int itemIndex_, const ItemType_ &value_)
        {
            Q_UNUSED(interface_);
            Q_UNUSED(itemIndex_);
            Q_UNUSED(value_);
        }

        template<typename ItemType_>
        static bool get(const IUniform *interface_, int itemIndex_, ItemType_ &value_)
        {
            Q_UNUSED(interface_);
            Q_UNUSED(itemIndex_);
            Q_UNUSED(value_);
            return false;
        }
    };

    using ItemTypeBaseSet = ItemTypeBase<ItemTypeConvert::maxIndex>;


    struct Texture
    {
        QString name;
        QString filename;
    };


    struct QuizImageObject
    {
        QByteArray vertexShader;
        QByteArray fragmentShader;

        std::vector<std::unique_ptr<IAttribute>> attributes;
        std::vector<std::unique_ptr<IUniform>> uniforms;
        std::vector<Texture> textures;

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
                return ItemTypeBaseSet::get(uniform_.get(), uniform_->typeIndex(), value_);
            }
            return false;
        }
    };


    struct QuizImageObjects
    {
        std::vector<std::shared_ptr<QuizImageObject>> objects;

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
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
