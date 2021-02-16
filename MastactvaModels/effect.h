#ifndef EFFECT_H
#define EFFECT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
//#include "../MastactvaModels/effectartefact.h"
#include "../MastactvaModels/effectarg.h"
#include "../MastactvaModels/effectargset.h"
#include "../MastactvaModels/effectobjects.h"


class EffectModel;


class Effect : public QObject, protected IListModelInfoObjectImpl
{
    Q_OBJECT

public:
    explicit Effect(EffectModel *parent_ = nullptr);

    Q_PROPERTY(int effectId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString effectName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant effectObjects READ effectObjects WRITE setEffectObjectss NOTIFY effectObjectsChanged)
    Q_PROPERTY(QVariant effectArgs READ args WRITE setArgs NOTIFY argsChanged)
    Q_PROPERTY(QVariant effectArgSets READ argSets WRITE setArgSets NOTIFY argSetsChanged)

    class DefaultLayout : public LayoutBase<Effect>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Effect::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Effect::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Effect::m_appId);
            addField<int>("id", "effectId", &Effect::id, &Effect::setId);
            addField<QString>("name", "effectName", &Effect::name, &Effect::setName);
            addField<QString>("description", "effectDescription", &Effect::description, &Effect::setDescription);
            addField<QDateTime>("created", "effectCreated", &Effect::created, &Effect::setCreated);
            addModel<EffectObjectsModel>("effectArtefacts", &Effect::m_effectObjectsModel, &Effect::createEffectObjectsModel);
            /* 1:N */
            addModel<EffectArgModel>("effectArgs", &Effect::m_effectArgModel, &Effect::createEffectArgModel);
            /* 1:N */
            addModel<EffectArgSetModel>("effectArgSets", &Effect::m_effectArgSetModel, &Effect::createEffectArgSetModel);
            /* 1:N */
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString name() const;
    void setName(const QString &name_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant effectObjects() const;
    void setEffectObjectss(const QVariant &obj_);
    QVariant args() const;
    void setArgs(const QVariant &obj_);
    QVariant argSets() const;
    void setArgSets(const QVariant &obj_);

public:
    EffectObjectsModel *getEffectObjects();
    const EffectObjectsModel *getEffectObjects() const;
    EffectArgModel *getEffectArguments();
    const EffectArgModel *getEffectArguments() const;

public:
    Q_INVOKABLE bool startRefreshArguments();
    Q_INVOKABLE void cancelRefreshArguments();
    Q_INVOKABLE void applyRefreshArguments();
    Q_INVOKABLE void loadChildren();
    Q_INVOKABLE bool isChildrenLoaded() const;

protected slots:
    void refreshArgumentsArtefactDownloadedSlot(const QString &url_);
    void refreshArgumentsProgressSlot();
    void itemAddedSlot();
    void itemSetSlot();
    void itemDeletedSlot();

protected:
    EffectObjectsModel *createEffectObjectsModel();
    EffectArgModel *createEffectArgModel();
    EffectArgSetModel *createEffectArgSetModel();
    void applyRefreshArgumentsStep();
    QString getObjectName() const;

    virtual void loadChildrenVF() override;
    virtual void listLoadedVF() override;

signals:
    void idChanged();
    void nameChanged();
    void descriptionChanged();
    void createdChanged();
    void effectObjectsChanged();
    void argsChanged();
    void argSetsChanged();
    void refreshArgumentsProgress(bool download_, qreal rate_);
    void refreshArgumentsBeforeApply();
    void refreshArgumentsAfterApply();
    void childrenLoaded();

private:
    EffectModel *m_effectModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_name;
    QString m_description;
    QDateTime m_created;
    EffectObjectsModel *m_effectObjectsModel = nullptr;
    EffectArgModel *m_effectArgModel = nullptr;
    EffectArgSetModel *m_effectArgSetModel = nullptr;

    QStringList m_artefactsUrls;
    QHash<QString, QString> m_artefactsLocalUrls;
    QList<EffectArg *> m_itemsToSet;
    QList<EffectArg *> m_itemsToDel;
    QList<EffectArg *> m_itemsToAdd;
    int m_itemsToChangeCount = 0;
};


class EffectModel : public ListModelBaseOfData<Effect, EffectModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Effect, EffectModel>;

public:
    explicit EffectModel(QObject *parent_ = nullptr);

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


#endif // EFFECT_H
