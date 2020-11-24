#ifndef EFFECTARGSET_H
#define EFFECTARGSET_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class EffectArgSetModel;


class EffectArgSet : public QObject
{
    Q_OBJECT
public:
    explicit EffectArgSet(EffectArgSetModel *parent_ = nullptr);

    Q_PROPERTY(int effectArgSetId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgSetEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QString effectArgSetDescription READ description WRITE setDescription NOTIFY descriptionChanged)

    class DefaultLayout : public LayoutBase<EffectArgSet>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg-set");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArgSet::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArgSet::m_appId);
            addField<int>("id", "effectArgSetId", &EffectArgSet::id, &EffectArgSet::setId);
            addField<int>("effect", "effectArgSetEffectId", &EffectArgSet::effectId, &EffectArgSet::setEffectId);
            addField<QString>("description", "effectArgSetDescription", &EffectArgSet::description, &EffectArgSet::setDescription);
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    QString description() const;
    void setDescription(const QString &description_);

signals:
    void idChanged();
    void effectIdChanged();
    void descriptionChanged();

private:
    EffectArgSetModel *m_effectArgSetModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    QString m_description;
};


class EffectArgSetModel : public ListModelBaseOfData<EffectArgSet, EffectArgSetModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArgSet, EffectArgSetModel>;

public:
    explicit EffectArgSetModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void listReloaded();
    void outputModelChanged();
};


#endif // EFFECTARGSET_H
