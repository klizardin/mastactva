/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef EFFECTOBJECTS_H
#define EFFECTOBJECTS_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/modelhelpers.h"
#include "../MastactvaModels/effectobjects_data.h"
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaModels/objectartefact.h"


class EffectObjectsModel;


class EffectObjects : public QObject, private EffectObjectsData, public IListModelItem
{
    Q_OBJECT
public:
    explicit EffectObjects(EffectObjectsModel *parent_ = nullptr);
    explicit EffectObjects(EffectObjectsData &&dataBase_, EffectObjectsModel *parent_ = nullptr);

    virtual ~EffectObjects() override;

    Q_PROPERTY(int effectObjectsId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectObjectsEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(int effectObjectsObjectInfoId READ objectInfoId WRITE setObjectInfoId NOTIFY objectInfoChanged)
    Q_PROPERTY(QVariant effectObjectsObjectInfo READ objectInfo WRITE setObjectInfo NOTIFY objectInfoChanged)
    Q_PROPERTY(int effectObjectsStepIndex READ stepIndex WRITE setStepIndex NOTIFY stepIndexChanged)
    Q_PROPERTY(QVariant effectObjectsObjectArtefacts READ objectArtefacts WRITE setObjectArtefacts NOTIFY objectArtefactsChanged)
    Q_PROPERTY(MergeId effectObjectsMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

public:
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
            addField<int>("object_info", "effectObjectsObjectInfoId", &EffectObjects::objectInfoId, &EffectObjects::setObjectInfoId);
            addField<int>("step_index", "effectObjectsStepIndex", &EffectObjects::stepIndex, &EffectObjects::setStepIndex);
            addField<MergeId>("mergeid", "effectObjectsMergeId", &EffectObjects::mergeid, &EffectObjects::setMergeid);
            addModel<ObjectInfoModel>("effectObjectsObjectInfo", &EffectObjects::m_objectInfoModel, &EffectObjects::createObjectInfoModel);
            /* 1:1 */
            addModel<ObjectArtefactModel>("effectObjectsObjectArtefacts", &EffectObjects::m_objectArtefactModel, &EffectObjects::createObjectArtefactModel);
            /* 1:N */
            setIdField("id");
        }
    };

    Q_INVOKABLE bool updateObjectInfoId(const QVariant &objectInfo_);
    Q_INVOKABLE void setEffectId(const int &effectId_);

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    int objectInfoId() const;
    void setObjectInfoId(const int &effectObjectInfoId_);
    QVariant objectInfo() const;
    void setObjectInfo(const QVariant &obj_);
    int stepIndex() const;
    void setStepIndex(const int &stepIndex_);
    QVariant objectArtefacts() const;
    void setObjectArtefacts(const QVariant &obj_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);
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
    void mergeidChanged();

private:
    EffectObjectsModel *m_effectObjectsModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    ObjectInfoModel *m_objectInfoModel = nullptr;
    ObjectArtefactModel *m_objectArtefactModel = nullptr;

    friend class ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>;
    friend class SortModelAfterChangeImpl<
            EffectObjectsData,
            EffectObjectsModel,
            ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>,
            EffectObjects>;
};


class EffectObjectsModel :
        public ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>;
    using sortModelAfterChange = SortModelAfterChangeImpl<
        EffectObjectsData,
        EffectObjectsModel,
        ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>,
        EffectObjects>
    ;

public:
    explicit EffectObjectsModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<EffectObjectsData *>> data_ = std::shared_ptr<QVector<EffectObjectsData *>>{nullptr},
            IModelConfig *config_ = &DefaultModelConfig::instance()
            );

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

private:
    bool compareModelItems(const EffectObjects *i1_, const EffectObjects *i2_) const;
    void currentIndexAfterSortChanged();

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
    void objectNameDerivedChanged();
    void error(const QString &errorCode_, const QString &description_);

    friend class SortModelAfterChangeImpl<
            EffectObjectsData,
            EffectObjectsModel,
            ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>,
            EffectObjects>
            ;

private:
    SortModelAfterChangeImpl<
        EffectObjectsData,
        EffectObjectsModel,
        ListModelBaseOfData<EffectObjectsData, EffectObjectsModel, EffectObjects>,
        EffectObjects> m_sort;
};


#endif // EFFECTOBJECTS_H
