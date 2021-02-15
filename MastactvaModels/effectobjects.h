#ifndef EFFECTOBJECTS_H
#define EFFECTOBJECTS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaModels/effectobjectartefact.h"


class EffectObjectsModel;


class EffectObjects : public QObject
{
    Q_OBJECT
public:
    explicit EffectObjects(EffectObjectsModel *parent_ = nullptr);

    Q_PROPERTY(int effectObjectsId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectObjectsEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QVariant effectObjectsEffectObjectInfo READ effectObjectInfo WRITE setEffectObjectInfo NOTIFY effectObjectInfoChanged)
    Q_PROPERTY(QVariant effectObjectsEffectObjectArtefacts READ effectObjectArtefacts WRITE setEffectObjectArtefacts NOTIFY effectObjectArtefactsChanged)

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
            addField<int>("effect_object_info", "", &EffectObjects::effectObjectInfoId, &EffectObjects::setEffectObjectInfoId);
            addModel<ObjectInfoModel>("effectObjectsEffectObjectInfo", &EffectObjects::m_effectObjectInfoModel, &EffectObjects::createEffectObjectInfoModel);
            addModel<ObjectArtefactModel>("effectObjectsEffectObjectInfo", &EffectObjects::m_effectObjectArtefactModel, &EffectObjects::createEffectObjectArtefactModel);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    int effectObjectInfoId() const;
    void setEffectObjectInfoId(const int &effectObjectInfoId_);
    QVariant effectObjectInfo() const;
    void setEffectObjectInfo(const QVariant &obj_);
    QVariant effectObjectArtefacts() const;
    void setEffectObjectArtefacts(const QVariant &obj_);
    ObjectArtefactModel *getEffectObjectArtefacts();
    const ObjectArtefactModel *getEffectObjectArtefacts() const;

protected:
    ObjectInfoModel *createEffectObjectInfoModel();
    ObjectArtefactModel *createEffectObjectArtefactModel();

signals:
    void idChanged();
    void effectIdChanged();
    void effectObjectInfoChanged();
    void effectObjectArtefactsChanged();

private:
    EffectObjectsModel *m_effectObjectsModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_effectObjectInfoId = -1;
    ObjectInfoModel *m_effectObjectInfoModel = nullptr;
    ObjectArtefactModel *m_effectObjectArtefactModel = nullptr;
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
