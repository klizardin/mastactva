#ifndef QUIZIMAGEDRAWINGDATA_H
#define QUIZIMAGEDRAWINGDATA_H


#include <type_traits>
#include <set>
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


struct QuizImageDrawingData
{
    virtual ~QuizImageDrawingData() = default;
    QByteArray vertexShader;
    QByteArray fragmentShader;
};



#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
