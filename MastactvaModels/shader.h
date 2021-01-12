#ifndef SHADER_H
#define SHADER_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/shadertype.h"


class ShaderModel;


class Shader : public QObject, protected IListModelInfoObjectImpl
{
    Q_OBJECT
public:
    explicit Shader(ShaderModel *parent_ = nullptr);

    Q_PROPERTY(int shaderId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString shaderFilename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString shaderHash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int shaderTypeId READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString shaderDescription READ description WRITE setDescription NOTIFY descriptionChanged)


    class DefaultLayout : public LayoutBase<Shader>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("shader");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Shader::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Shader::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Shader::m_appId);
            addField<int>("id", "shaderId", &Shader::id, &Shader::setId);
            addField<ImageSource>("filename", "shaderFilename", &Shader::getFilename, &Shader::setFilename);
            addField<QString>("hash", "shaderHash", &Shader::hash, &Shader::setHash);
            addField<int>("type", "shaderTypeId", &Shader::type, &Shader::setType);
            addField<QString>("description", "shaderDescription", &Shader::description, &Shader::setDescription);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString filename() const;
    ImageSource getFilename() const;
    void setFilename(const QString &filename_);
    void setFilename(const ImageSource &filename_);
    QString hash() const;
    void setHash(const QString &hash_);
    int type() const;
    void setType(const int &type_);
    QString description() const;
    void setDescription(const QString &description_);

protected:
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    QString getObjectName() const;

signals:
    void idChanged();
    void filenameChanged();
    void hashChanged();
    void typeChanged();
    void descriptionChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    ImageSource m_filename;
    QString m_hash;
    int m_typeId = -1;
    QString m_description;
    ShaderModel *m_shaderModel = nullptr;
};


class ShaderModel : public ListModelBaseOfData<Shader, ShaderModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Shader, ShaderModel>;

public:
    explicit ShaderModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, const QString& errorCodeStr_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // SHADER_H
