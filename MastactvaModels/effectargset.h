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

#ifndef EFFECTARGSET_H
#define EFFECTARGSET_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/effectargvalue.h"
#include "../MastactvaModels/easingtype.h"
#include "../MastactvaModels/effectargset_data.h"


class EffectArgSetModel;


class EffectArgSet :
        public QObject,
        protected IListModelInfoObjectImpl,
        public IListModelItem,
        private EffectArgSetData
{
    Q_OBJECT
public:
    explicit EffectArgSet(EffectArgSetModel *parent_ = nullptr);
    EffectArgSet(EffectArgSetData &&data_, EffectArgSetModel *parent_ = nullptr);
    virtual ~EffectArgSet() override;

    Q_PROPERTY(int effectArgSetId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgSetEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QString effectArgSetDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariant effectArgSetValues READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(int effectArgSetEasingId READ easingId WRITE setEasingId NOTIFY easingChanged)
    Q_PROPERTY(QDateTime effectArgSetCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(MergeId effectArgSetMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

    class DefaultLayout : public LayoutBase<EffectArgSet>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg-set");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArgSet::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &EffectArgSet::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArgSet::m_appId);
            addField<int>("id", "effectArgSetId", &EffectArgSet::id, &EffectArgSet::setId);
            addField<int>("effect", "effectArgSetEffectId", &EffectArgSet::effectId, &EffectArgSet::setEffectId);
            addField<int>("easing", "effectArgSetEasingId", &EffectArgSet::easingId, &EffectArgSet::setEasingId);
            addField<QString>("description", "effectArgSetDescription", &EffectArgSet::description, &EffectArgSet::setDescription);
            addField<QDateTime>("created", "effectArgSetCreated", &EffectArgSet::created, &EffectArgSet::setCreated);
            addField<MergeId>("mergeid", "effectArgSetMergeId", &EffectArgSet::mergeid, &EffectArgSet::setMergeid);
            addModel<EffectArgValueModel>("effectArgSetValues", &EffectArgSet::m_affectArgValueModel, &EffectArgSet::createAffectArgValueModel);
            /* 1:N */
            setIdField("id");
        }
    };


public:
    Q_INVOKABLE QVariantList argValuesOfSetIdList();
    Q_INVOKABLE bool isChildrenLoaded();
    EffectArgValueModel *getArgumentValues();
    const EffectArgValueModel *getArgumentValues() const;

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    QString description() const;
    void setDescription(const QString &description_);
    QVariant values() const;
    void setValues(const QVariant &obj_);
    int easingId() const;
    void setEasingId(const int &easingId_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);

protected:
    EffectArgValueModel *createAffectArgValueModel();
    QString getObjectName() const;

    virtual void loadChildrenVF() override;
    virtual void listLoadedVF() override;

signals:
    void idChanged();
    void effectIdChanged();
    void descriptionChanged();
    void valuesChanged();
    void easingChanged();
    void childrenLoaded();
    void createdChanged();
    void mergeidChanged();

private:
    EffectArgSetModel *m_effectArgSetModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    EffectArgValueModel *m_affectArgValueModel = nullptr;

    friend class ListModelBaseOfData<EffectArgSetData, EffectArgSetModel, EffectArgSet>;
};


class EffectArgSetModel : public ListModelBaseOfData<EffectArgSetData, EffectArgSetModel, EffectArgSet>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArgSetData, EffectArgSetModel, EffectArgSet>;

public:
    explicit EffectArgSetModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<EffectArgSetData *>> data_ = std::shared_ptr<QVector<EffectArgSetData *>>{nullptr}
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
};


#endif // EFFECTARGSET_H
