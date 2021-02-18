#ifndef EFFECTOBJECTS_H
#define EFFECTOBJECTS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaModels/objectartefact.h"


class EffectObjectsModel;


class EffectObjects : public QObject
{
    Q_OBJECT
public:
    explicit EffectObjects(EffectObjectsModel *parent_ = nullptr);
    virtual ~EffectObjects() override;

    Q_PROPERTY(int effectObjectsId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectObjectsEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QVariant effectObjectsObjectInfo READ objectInfo WRITE setObjectInfo NOTIFY objectInfoChanged)
    Q_PROPERTY(int effectObjectsStepIndex READ stepIndex WRITE setStepIndex NOTIFY stepIndexChanged)
    Q_PROPERTY(QVariant effectObjectsObjectArtefacts READ objectArtefacts WRITE setObjectArtefacts NOTIFY objectArtefactsChanged)

    class DefaultLayout : public LayoutBase<EffectObjects>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-object");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectObjects::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &EffectObjects::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectObjects::m_appId);
            addField<int>("id", "effectObjectsId", &EffectObjects::id, &EffectObjects::setId);
            addField<int>("effect", "effectObjectsEffectId", &EffectObjects::effectId, &EffectObjects::setEffectId);
            addField<int>("object_info", "", &EffectObjects::objectInfoId, &EffectObjects::setObjectInfoId);
            addField<int>("step_index", "effectObjectsStepIndex", &EffectObjects::stepIndex, &EffectObjects::setStepIndex);
            addModel<ObjectInfoModel>("effectObjectsObjectInfo", &EffectObjects::m_objectInfoModel, &EffectObjects::createObjectInfoModel);
            /* 1:1 */
            addModel<ObjectArtefactModel>("effectObjectsObjectArtefacts", &EffectObjects::m_objectArtefactModel, &EffectObjects::createObjectArtefactModel);
            /* 1:N */
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    int objectInfoId() const;
    void setObjectInfoId(const int &effectObjectInfoId_);
    QVariant objectInfo() const;
    void setObjectInfo(const QVariant &obj_);
    int stepIndex() const;
    void setStepIndex(const int &stepIndex_);
    QVariant objectArtefacts() const;
    void setObjectArtefacts(const QVariant &obj_);
    ObjectArtefactModel *getObjectArtefacts();
    const ObjectArtefactModel *getObjectArtefacts() const;
    ObjectInfoModel *getObjectInfoModel();
    const ObjectInfoModel *getObjectInfoModel() const;

protected:
    ObjectInfoModel *createObjectInfoModel();
    ObjectArtefactModel *createObjectArtefactModel();

signals:
    void idChanged();
    void effectIdChanged();
    void objectInfoChanged();
    void stepIndexChanged();
    void objectArtefactsChanged();

private:
    EffectObjectsModel *m_effectObjectsModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_objectInfoId = -1;
    int m_stepIndex = -1;
    ObjectInfoModel *m_objectInfoModel = nullptr;
    ObjectArtefactModel *m_objectArtefactModel = nullptr;
};


class EffectObjectsModel : public ListModelBaseOfData<EffectObjects, EffectObjectsModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectObjects, EffectObjectsModel>;

public:
    explicit EffectObjectsModel(QObject *parent_ = nullptr);

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


#endif // EFFECTOBJECTS_H
