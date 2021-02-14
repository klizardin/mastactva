#ifndef EFFECTARTEFACTARGSET_H
#define EFFECTARTEFACTARGSET_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/effectartefactarg.h"


class EffectArtefactArgSetModel;


class EffectArtefactArgSet : public QObject
{
    Q_OBJECT
public:
    explicit EffectArtefactArgSet(EffectArtefactArgSetModel *parent_ = nullptr);

    Q_PROPERTY(int effectArtefactArgSetId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArtefactArgSetEffectObjectArtefactId READ effectObjectArtefactId WRITE setEffectObjectArtefactId NOTIFY effectArtefactIdChanged)
    Q_PROPERTY(QString effectArtefactArgSetDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariant effectArtefactArg READ effectArtefactArg WRITE setEffectArtefactArg NOTIFY effectArtefactArgChanged)


    class DefaultLayout : public LayoutBase<EffectArtefactArgSet>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-artefact-arg-set");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArtefactArgSet::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &EffectArtefactArgSet::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArtefactArgSet::m_appId);
            addField<int>("id", "effectArtefactArgSetId", &EffectArtefactArgSet::id, &EffectArtefactArgSet::setId);
            addField<int>("effect_object_artefact", "effectArtefactArgSetEffectArtefactId", &EffectArtefactArgSet::effectObjectArtefactId, &EffectArtefactArgSet::setEffectObjectArtefactId);
            addField<QString>("description", "effectArtefactArgSetDescription", &EffectArtefactArgSet::description, &EffectArtefactArgSet::setDescription);
            addModel<EffectArtefactArgModel>("effectArtefactArgSetEffectArtefactArg", &EffectArtefactArgSet::m_effectArtefactArgModel, &EffectArtefactArgSet::createEffectArtefactArgModel);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectObjectArtefactId() const;
    void setEffectObjectArtefactId(const int &effectArtefactId_);
    QString description() const;
    void setDescription(const QString &description_);
    QVariant effectArtefactArg() const;
    void setEffectArtefactArg(const QVariant &obj_);

protected:
    EffectArtefactArgModel *createEffectArtefactArgModel();

signals:
    void idChanged();
    void effectArtefactIdChanged();
    void descriptionChanged();
    void effectArtefactArgChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectArtefactId = -1;
    QString m_description;
    EffectArtefactArgModel *m_effectArtefactArgModel = nullptr;
    EffectArtefactArgSetModel *m_effectArtefactArgSetModel = nullptr;
};


class EffectArtefactArgSetModel : public ListModelBaseOfData<EffectArtefactArgSet, EffectArtefactArgSetModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArtefactArgSet, EffectArtefactArgSetModel>;

public:
    explicit EffectArtefactArgSetModel(QObject *parent_ = nullptr);

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


#endif // EFFECTARTEFACTARGSET_H
