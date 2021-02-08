#ifndef EFFECTARG_H
#define EFFECTARG_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefactargtype.h"


class EffectArgModel;


class EffectArg : public QObject
{
    Q_OBJECT
public:
    explicit EffectArg(EffectArgModel *parent_ = nullptr);

    Q_PROPERTY(int effectArgId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(int effectArgArgTypeId READ argTypeId WRITE setArgTypeId NOTIFY argTypeChanged)
    Q_PROPERTY(int effectArgArgStorageId READ argStorageId WRITE setArgStorageId NOTIFY argStorageChanged)
    Q_PROPERTY(QString effectArgName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectArgDefaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString effectArgDescription READ description WRITE setDescription NOTIFY descriptionChanged)

    class DefaultLayout : public LayoutBase<EffectArg>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArg::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArg::m_appId);
            addField<int>("id", "effectArgId", &EffectArg::id, &EffectArg::setId);
            addField<int>("effect", "effectArgEffectId", &EffectArg::effectId, &EffectArg::setEffectId);
            addField<int>("arg_type", "effectArgArgTypeId", &EffectArg::argTypeId, &EffectArg::setArgTypeId);
            addField<int>("arg_storage", "effectArgArgStorageId", &EffectArg::argStorageId, &EffectArg::setArgStorageId);
            addField<QString>("name", "effectArgName", &EffectArg::name, &EffectArg::setName);
            addField<QString>("default_value", "effectArgDefaultValue", &EffectArg::defaultValue, &EffectArg::setDefaultValue);
            addField<QString>("description", "effectArgDescription", &EffectArg::description, &EffectArg::setDescription);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    int argTypeId() const;
    void setArgTypeId(const int &argTypeId_);
    int argStorageId() const;
    void setArgStorageId(const int &argStorageId_);
    QString name() const;
    void setName(const QString &name_);
    QString defaultValue() const;
    void setDefaultValue(const QString &defaultValue_);
    QString description() const;
    void setDescription(const QString &description_);

signals:
    void idChanged();
    void effectIdChanged();
    void argTypeChanged();
    void argStorageChanged();
    void nameChanged();
    void defaultValueChanged();
    void descriptionChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    EffectArgModel *m_effectArgModel = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_argTypeId = -1;
    int m_argStorageId = -1;
    QString m_name;
    QString m_defaultValue;
    QString m_description;
};


class EffectArgModel : public ListModelBaseOfData<EffectArg, EffectArgModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArg, EffectArgModel>;

public:
    explicit EffectArgModel(QObject *parent_ = nullptr);

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


#endif // EFFECTARG_H
