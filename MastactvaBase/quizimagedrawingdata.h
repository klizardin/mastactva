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
    template<typename ItemType_>
    class ItemTypeTraits
    {
    public:
        constexpr static int tupleSize = 0;
        constexpr static int typeIndex = -1;
    };
    template<int TypeIndex_>
    class ItemTypeIndexTraits
    {
    public:
        using type = void;
    };

#define ITEM_TYPE_TRAITS(ItemType_, tupleSize_, typeIndex_)             \
    template<>                                                          \
    class ItemTypeTraits<ItemType_>                                     \
    {                                                                   \
    public:                                                             \
        constexpr static int tupleSize = tupleSize_;                    \
        constexpr static int typeIndex = typeIndex_;                    \
    };                                                                  \
    template<>                                                          \
    class ItemTypeIndexTraits<typeIndex_>                               \
    {                                                                   \
    public:                                                             \
        using type = ItemType_;                                         \
    };                                                                  \
/*end traints macro*/

    ITEM_TYPE_TRAITS(GLfloat,       1,  0)
    ITEM_TYPE_TRAITS(GLint,         1,  1)
    ITEM_TYPE_TRAITS(GLuint,        1,  2)
    ITEM_TYPE_TRAITS(QVector2D,     2,  3)
    ITEM_TYPE_TRAITS(QVector3D,     3,  4)
    ITEM_TYPE_TRAITS(QVector4D,     4,  5)
    ITEM_TYPE_TRAITS(QColor,        3,  6)
    ITEM_TYPE_TRAITS(QPoint,        2,  7)
    ITEM_TYPE_TRAITS(QPointF,       2,  8)
    ITEM_TYPE_TRAITS(QSize,         2,  9)
    ITEM_TYPE_TRAITS(QSizeF,        2,  10)
    ITEM_TYPE_TRAITS(QMatrix2x2,    4,  11)
    ITEM_TYPE_TRAITS(QMatrix2x3,    6,  12)
    ITEM_TYPE_TRAITS(QMatrix2x4,    8,  13)
    ITEM_TYPE_TRAITS(QMatrix3x2,    6,  14)
    ITEM_TYPE_TRAITS(QMatrix3x3,    9,  15)
    ITEM_TYPE_TRAITS(QMatrix3x4,    12, 16)
    ITEM_TYPE_TRAITS(QMatrix4x2,    8,  17)
    ITEM_TYPE_TRAITS(QMatrix4x3,    12, 18)
    ITEM_TYPE_TRAITS(QMatrix4x4,    16, 19)

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

        void set(const std::vector<ItemType_> &value_)
        {
            if(!m_data.operator bool())
            {
                m_data.reset(new std::vector<ItemType_>());
            }
            *m_data.get() = value_;
        }

        template<typename ItemType2_>
        void set(const std::vector<ItemType2_> &value_)
        {
            Q_UNUSED(value_);
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
            Q_UNUSED(value_);
        }

    private:
        QString m_name;
        std::shared_ptr<ItemType_> m_data;
    };


    struct QuizImageObject
    {
        QByteArray vertexShader;
        QByteArray fragmentShader;

        std::vector<std::unique_ptr<IAttribute>> attributes;
        std::vector<std::unique_ptr<IUniform>> uniforms;
        std::vector<QString> textures;

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_)
        {
            for(std::unique_ptr<IAttribute> &attribute_ : attributes)
            {
                if(!attribute_.operator bool()
                        || attribute_->name() != name_
                        || attribute_->typeIndex() != ItemTypeTraits<ItemType_>::typeIndex)
                {
                    continue;
                }
                Attribute<ItemType_> *attr = static_cast<Attribute<ItemType_> *>(attribute_.get());
                attr->set(value_);
            }
        }

        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            for(std::unique_ptr<IUniform> &attribute_ : uniforms)
            {
                if(!attribute_.operator bool()
                        || attribute_->name() != name_
                        || attribute_->typeIndex() != ItemTypeTraits<ItemType_>::typeIndex)
                {
                    continue;
                }
                Uniform<ItemType_> *attr = static_cast<Uniform<ItemType_> *>(attribute_.get());
                attr->set(value_);
            }
        }
    };


    struct QuizImageObjects
    {
        std::vector<std::shared_ptr<QuizImageObject>> objects;

        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_)
        {
            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }
                object_->setAttribute(name_, value_);
            }
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
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
