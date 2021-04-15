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
    void setName(QString name_);
    const QString &getStorage() const;
    void setStorage(QString storage_);
    const QString &getType() const;
    void setType(QString type_);
    const QString &getValue() const;
    void setValue(QString value_);
    const QString &getDefaultValue() const;
    void setDefaultValue(QString defaultValue_);

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



#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
