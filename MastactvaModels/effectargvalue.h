#ifndef EFFECTARGVALUE_H
#define EFFECTARGVALUE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "effectarg.h"


class EffectArgValueModel;


class EffectArgValue : public QObject
{
    Q_OBJECT
public:
    explicit EffectArgValue(EffectArgValueModel *parent_ = nullptr);

    Q_PROPERTY(int effectArgValueId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgValueArgSetId READ argSetId WRITE setArgSetId NOTIFY argSetIdChanged)
    Q_PROPERTY(QVariant effectArgValueArg READ arg WRITE setArg NOTIFY argChanged)
    Q_PROPERTY(QString effectArgValueValue READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString effectArgValueDescription READ description WRITE setDescription NOTIFY descriptionChanged)

    class DefaultLayout : public LayoutBase<EffectArgValue>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg-value");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArgValue::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArgValue::m_appId);
            addField<int>("id", "effectArgValueId", &EffectArgValue::id, &EffectArgValue::setId);
            addField<int>("arg_set", "effectArgValueArgSetId", &EffectArgValue::argSetId, &EffectArgValue::setArgSetId);
            addField<int>("arg", "", &EffectArgValue::argId, &EffectArgValue::setArgId);
            addModel<EffectArgModel>("effectArgValueArg", &EffectArgValue::m_effectArgModel, &EffectArgValue::createEffectArgModel);
            addField<QString>("value", "effectArgValueValue", &EffectArgValue::value, &EffectArgValue::setValue);
            addField<QString>("description", "effectArgValueDescription", &EffectArgValue::description, &EffectArgValue::setDescription);
        }
    };

public:
    Q_INVOKABLE int argSetId() const;
    Q_INVOKABLE void setArgSetId(const int &argSetId_);
    Q_INVOKABLE int argId() const;
    Q_INVOKABLE void setArgId(const int &argId_);

public:
    int id() const;
    void setId(const int &id_);
    QVariant arg() const;
    void setArg(const QVariant &obj_);
    QString value() const;
    void setValue(const QString &value_);
    QString description() const;
    void setDescription(const QString &description_);

    EffectArgModel *getArg();

protected:
    EffectArgModel *createEffectArgModel();

signals:
    void idChanged();
    void argSetIdChanged();
    void argChanged();
    void valueChanged();
    void descriptionChanged();

private:
    EffectArgValueModel *m_effectArgValueModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_argSetId = -1;
    int m_argId = -1;
    QString m_value;
    QString m_description;
    EffectArgModel *m_effectArgModel = nullptr;
};


class EffectArgValueModel : public ListModelBaseOfData<EffectArgValue, EffectArgValueModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArgValue, EffectArgValueModel>;

public:
    explicit EffectArgValueModel(QObject *parent_ = nullptr);

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
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // EFFECTARGVALUE_H
