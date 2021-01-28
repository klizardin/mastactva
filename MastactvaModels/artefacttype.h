#ifndef SHADERTYPE_H
#define SHADERTYPE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ShaderType : public QObject
{
    Q_OBJECT
public:
    explicit ShaderType(QObject *parent_ = nullptr);

    Q_PROPERTY(int artefactTypeId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString shaderTypeType READ type WRITE setType NOTIFY typeChanged)

    class DefaultLayout : public LayoutBase<ShaderType>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("shader-type");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ShaderType::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ShaderType::m_appId);
            addField<int>("id", "artefactTypeId", &ShaderType::id, &ShaderType::setId);
            addField<QString>("type", "shaderTypeType", &ShaderType::type, &ShaderType::setType);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString type() const;
    void setType(const QString &type_);

signals:
    void idChanged();
    void typeChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_type;
};


class ShaderTypeModel : public ListModelBaseOfData<ShaderType, ShaderTypeModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ShaderType, ShaderTypeModel>;

public:
    explicit ShaderTypeModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString& errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
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


#endif // SHADERTYPE_H
