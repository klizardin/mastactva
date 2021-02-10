#ifndef EFFECTOBJECTS_H
#define EFFECTOBJECTS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/effectobjectinfo.h"


class EffectObjectsModel;


class EffectObjects : public QObject
{
    Q_OBJECT
public:
    explicit EffectObjects(EffectObjectsModel *parent_ = nullptr);

    Q_PROPERTY(int effectObjectsId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectObjectsEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QVariant effectObjectsEffectObjectInfo READ effectObjectInfo WRITE setEffectObjectInfo NOTIFY effectObjectInfoChanged)

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
            addModel<EffectObjectInfoModel>("effectObjectsEffectObjectInfo", &EffectObjects::m_effectObjectInfoModel, &EffectObjects::createEffectObjectInfoModel);
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

protected:
    EffectObjectInfoModel *createEffectObjectInfoModel();


signals:
    void idChanged();
    void effectIdChanged();
    void effectObjectInfoChanged();

private:
    EffectObjectsModel *m_effectObjectsModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_effectObjectInfoId = -1;
    EffectObjectInfoModel *m_effectObjectInfoModel = nullptr;
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
