#ifndef OBJECTARTEFACT_H
#define OBJECTARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefact.h"


class ObjectArtefactModel;


class ObjectArtefact : public QObject
{
    Q_OBJECT
public:
    explicit ObjectArtefact(ObjectArtefactModel *parent_ = nullptr);

    Q_PROPERTY(int effectArtefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArtefactEffectObjectId READ effectObjectId WRITE setEffectObjectId NOTIFY effectObjectIdChanged)
    Q_PROPERTY(QVariant effectArtefactArtefact READ artefact WRITE setArtefact NOTIFY artefactChanged)

    class DefaultLayout : public LayoutBase<ObjectArtefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-object-artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ObjectArtefact::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ObjectArtefact::m_appId);
            addField<int>("id", "effectArtefactId", &ObjectArtefact::id, &ObjectArtefact::setId);
            addField<int>("effect_object", "effectArtefactEffectObjectId", &ObjectArtefact::effectObjectId, &ObjectArtefact::setEffectObjectId);
            addField<int>("artefact", "", &ObjectArtefact::artefactId, &ObjectArtefact::setArtefactId);
            addModel<ArtefactModel>("effectArtefactArtefact", &ObjectArtefact::m_artefactModel, &ObjectArtefact::createArtefactModel);
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

signals:
    void idChanged();
    void effectObjectIdChanged();
    void artefactChanged();
    void effectArtefactArgSetChanged();

private:
    ObjectArtefactModel *m_effectArtefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    ArtefactModel *m_artefactModel = nullptr;
};


class ObjectArtefactModel : public ListModelBaseOfData<ObjectArtefact, ObjectArtefactModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ObjectArtefact, ObjectArtefactModel>;

public:
    explicit ObjectArtefactModel(QObject *parent_ = nullptr);

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


#endif // OBJECTARTEFACT_H
