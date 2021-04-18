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
    class IAttribute
    {
    public:
        virtual ~IAttribute() = default;
        virtual const QString &name() const = 0;
        virtual int tupleSize() const = 0;
        virtual const GLfloat *constData() const = 0;
        virtual int size() const = 0;
    };

    template<typename AttributeItemType_>
    class ItemTypeTraits
    {
    public:
        constexpr static int tuppleSize = 0;
    };

#define ITEM_TYPE_TRAITS(AttributeItemType_, value_)   \
    template<>                                              \
    class ItemTypeTraits<AttributeItemType_>           \
    {                                                       \
    public:                                                 \
        constexpr static int tuppleSize = value_;           \
    };                                                      \
/*end traints macro*/

    ITEM_TYPE_TRAITS(GLfloat, 1)
    ITEM_TYPE_TRAITS(GLint, 1)
    ITEM_TYPE_TRAITS(GLuint, 1)
    ITEM_TYPE_TRAITS(QVector2D, 2)
    ITEM_TYPE_TRAITS(QVector3D, 3)
    ITEM_TYPE_TRAITS(QVector4D, 4)
    ITEM_TYPE_TRAITS(QColor, 3)
    ITEM_TYPE_TRAITS(QPoint, 2)
    ITEM_TYPE_TRAITS(QPointF, 2)
    ITEM_TYPE_TRAITS(QSize, 2)
    ITEM_TYPE_TRAITS(QSizeF, 2)
    ITEM_TYPE_TRAITS(QMatrix2x2, 4)
    ITEM_TYPE_TRAITS(QMatrix2x3, 6)
    ITEM_TYPE_TRAITS(QMatrix2x4, 8)
    ITEM_TYPE_TRAITS(QMatrix3x2, 6)
    ITEM_TYPE_TRAITS(QMatrix3x3, 9)
    ITEM_TYPE_TRAITS(QMatrix3x4, 12)
    ITEM_TYPE_TRAITS(QMatrix4x2, 8)
    ITEM_TYPE_TRAITS(QMatrix4x3, 12)
    ITEM_TYPE_TRAITS(QMatrix4x4, 16)


    template<class ItemType_ = QVector3D>
    class Attribute : public IAttribute
    {
    public:
        Attribute(const QString &name_, std::vector<ItemType_> &&data_)
            : m_name(name_),
              m_data(std::move(data_))
        {
        }

    public:
        virtual const QString &name() const override
        {
            return m_name;
        }

        virtual int tupleSize() const override
        {
            return ItemTypeTraits<ItemType_>::tuppleSize;
        }

        virtual const GLfloat *constData() const override
        {
            if(m_data.empty())
            {
                return nullptr;
            }
            return &(const_cast<Attribute<ItemType_> *>(this)->m_data[0][0]);
        }

        virtual int size() const override
        {
            return m_data.size();
        }

    private:
        QString m_name;
        std::vector<ItemType_> m_data;
    };

    class IUniform
    {
    public:
        virtual ~IUniform() = default;
        virtual const QString &name() const = 0;
        virtual void set(QOpenGLShaderProgram *program, int location_) const = 0;
    };

    template<typename ItemType_>
    struct Uniform : public IUniform
    {
        Uniform(const QString &name_, ItemType_ &&data_)
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
            if(nullptr == program
                    || location_ < 0)
            {
                return;
            }
            program->setUniformValue(location_, m_data);
        }

    private:
        QString m_name;
        ItemType_ m_data;
    };

    struct QuizImageObject
    {
        QByteArray vertexShader;
        QByteArray fragmentShader;

        std::vector<std::unique_ptr<IAttribute>> attributes;
        std::vector<std::unique_ptr<IUniform>> uniforms;
    };

    struct QuizImageObjects
    {
        std::vector<std::shared_ptr<QuizImageObject>> objects;
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
