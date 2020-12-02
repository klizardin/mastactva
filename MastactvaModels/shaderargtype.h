#ifndef SHADERARGTYPE_H
#define SHADERARGTYPE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ShaderArgType : public QObject
{
    Q_OBJECT
public:
    explicit ShaderArgType(QObject *parent_ = nullptr);

    Q_PROPERTY(int shaderArgTypeId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString shaderArgTypeType READ type WRITE setType NOTIFY typeChanged)

    class DefaultLayout : public LayoutBase<ShaderArgType>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("shader-arg-type");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ShaderArgType::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ShaderArgType::m_appId);
            addField<int>("id", "shaderArgTypeId", &ShaderArgType::id, &ShaderArgType::setId);
            addField<QString>("type", "shaderArgTypeType", &ShaderArgType::type, &ShaderArgType::setType);
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


class ShaderArgTypeModel : public ListModelBaseOfData<ShaderArgType, ShaderArgTypeModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ShaderArgType, ShaderArgTypeModel>;

public:
    explicit ShaderArgTypeModel(QObject *parent_ = nullptr);

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
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // SHADERARGTYPE_H