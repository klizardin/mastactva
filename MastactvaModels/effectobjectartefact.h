#ifndef EFFECTARTEFACT_H
#define EFFECTARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/effectartefactargset.h"


class EffectObjectArtefactModel;


class EffectObjectArtefact : public QObject
{
    Q_OBJECT
public:
    explicit EffectObjectArtefact(EffectObjectArtefactModel *parent_ = nullptr);

    Q_PROPERTY(int effectArtefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArtefactEffectObjectId READ effectObjectId WRITE setEffectObjectId NOTIFY effectObjectIdChanged)
    Q_PROPERTY(QVariant effectArtefactArtefact READ artefact WRITE setArtefact NOTIFY artefactChanged)
    Q_PROPERTY(QVariant effectArtefactEffectArtefactArgSet READ effectArtefactArgSet WRITE setEffectArtefactArgSet NOTIFY effectArtefactArgSetChanged)

    class DefaultLayout : public LayoutBase<EffectObjectArtefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-object-artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectObjectArtefact::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectObjectArtefact::m_appId);
            addField<int>("id", "effectArtefactId", &EffectObjectArtefact::id, &EffectObjectArtefact::setId);
            addField<int>("effect_object", "effectArtefactEffectObjectId", &EffectObjectArtefact::effectObjectId, &EffectObjectArtefact::setEffectObjectId);
            addField<int>("artefact", "", &EffectObjectArtefact::artefactId, &EffectObjectArtefact::setArtefactId);
            addModel<ArtefactModel>("effectArtefactArtefact", &EffectObjectArtefact::m_artefactModel, &EffectObjectArtefact::createArtefactModel);
            addModel<EffectArtefactArgSetModel>("effectArtefactEffectArtefactArgSet", &EffectObjectArtefact::m_effectArtefactArgSetModel, &EffectObjectArtefact::createEffectArtefactArgSetModel);
            setIdField("id");
        }
    };

    Q_INVOKABLE int artefactId() const;
    Q_INVOKABLE void setArtefactId(const int &artefactId_);

public:
    int id() const;
    void setId(const int &id_);
    int effectObjectId() const;
    void setEffectObjectId(const int &effectId_);
    QVariant artefact() const;
    void setArtefact(const QVariant &obj_);
    ArtefactModel *getArtefact();
    const ArtefactModel *getArtefact() const;
    QVariant effectArtefactArgSet() const;
    void setEffectArtefactArgSet(const QVariant &obj_);

protected:
    ArtefactModel *createArtefactModel();
    EffectArtefactArgSetModel *createEffectArtefactArgSetModel();

signals:
    void idChanged();
    void effectObjectIdChanged();
    void artefactChanged();
    void effectArtefactArgSetChanged();

private:
    EffectObjectArtefactModel *m_effectArtefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    ArtefactModel *m_artefactModel = nullptr;
    EffectArtefactArgSetModel *m_effectArtefactArgSetModel = nullptr;
};


class EffectObjectArtefactModel : public ListModelBaseOfData<EffectObjectArtefact, EffectObjectArtefactModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectObjectArtefact, EffectObjectArtefactModel>;

public:
    explicit EffectObjectArtefactModel(QObject *parent_ = nullptr);

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


#endif // EFFECTARTEFACT_H
