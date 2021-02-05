#ifndef EFFECTARTEFACT_H
#define EFFECTARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/effectartefactargset.h"


class EffectArtefactModel;


class EffectArtefact : public QObject
{
    Q_OBJECT
public:
    explicit EffectArtefact(EffectArtefactModel *parent_ = nullptr);

    Q_PROPERTY(int effectArtefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArtefactEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QVariant effectArtefactArtefact READ artefact WRITE setArtefact NOTIFY artefactChanged)
    Q_PROPERTY(QVariant effectArtefactEffectArtefactArgSet READ effectArtefactArgSet WRITE setEffectArtefactArgSet NOTIFY effectArtefactArgSetChanged)

    class DefaultLayout : public LayoutBase<EffectArtefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArtefact::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArtefact::m_appId);
            addField<int>("id", "effectArtefactId", &EffectArtefact::id, &EffectArtefact::setId);
            addField<int>("effect", "effectArtefactEffectId", &EffectArtefact::effectId, &EffectArtefact::setEffectId);
            addField<int>("artefact", "", &EffectArtefact::artefactId, &EffectArtefact::setArtefactId);
            addModel<ArtefactModel>("effectArtefactArtefact", &EffectArtefact::m_artefactModel, &EffectArtefact::createArtefactModel);
            addModel<EffectArtefactArgSetModel>("effectArtefactEffectArtefactArgSet", &EffectArtefact::m_effectArtefactArgSetModel, &EffectArtefact::createEffectArtefactArgSetModel);
            setIdField("id");
        }
    };

    Q_INVOKABLE int artefactId() const;
    Q_INVOKABLE void setArtefactId(const int &artefactId_);

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
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
    void effectIdChanged();
    void artefactChanged();
    void effectArtefactArgSetChanged();

private:
    EffectArtefactModel *m_effectArtefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    ArtefactModel *m_artefactModel = nullptr;
    EffectArtefactArgSetModel *m_effectArtefactArgSetModel = nullptr;
};


class EffectArtefactModel : public ListModelBaseOfData<EffectArtefact, EffectArtefactModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArtefact, EffectArtefactModel>;

public:
    explicit EffectArtefactModel(QObject *parent_ = nullptr);

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
