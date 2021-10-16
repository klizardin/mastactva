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

#ifndef OBJECTINFO_H
#define OBJECTINFO_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/objectinfo_data.h"


class ObjectInfoModel;


class ObjectInfo : public QObject, public IListModelItem, private ObjectInfoData
{
    Q_OBJECT
public:
    explicit ObjectInfo(ObjectInfoModel *parent_ = nullptr);
    explicit ObjectInfo(ObjectInfoData &&data_, ObjectInfoModel *parent_ = nullptr);

    Q_PROPERTY(int effectObjectInfoId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString effectObjectInfoName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectObjectInfoProgrammerName READ programmerName WRITE setProgrammerName NOTIFY programmerNameChanged)
    Q_PROPERTY(QString effectObjectInfoDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectObjectInfoCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(MergeId effectObjectMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)


    class DefaultLayout : public LayoutBase<ObjectInfo>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("object-info");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ObjectInfo::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ObjectInfo::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &ObjectInfo::m_objectModelInfo);
            addField<int>("id", "effectObjectInfoId", &ObjectInfo::id, &ObjectInfo::setId);
            addField<QString>("name", "effectObjectInfoName", &ObjectInfo::name, &ObjectInfo::setName);
            addField<QString>("programmer_name", "effectObjectInfoProgrammerName", &ObjectInfo::programmerName, &ObjectInfo::setProgrammerName);
            addField<QString>("description", "effectObjectInfoDescription", &ObjectInfo::description, &ObjectInfo::setDescription);
            addField<QDateTime>("created", "effectObjectInfoCreated", &ObjectInfo::created, &ObjectInfo::setCreated);
            addField<MergeId>("mergeid", "effectObjectMergeId", &ObjectInfo::mergeid, &ObjectInfo::setMergeid);
            setIdField("id");
        }
    };

    Q_INVOKABLE bool isInitializeObject() const;


    int id() const;
    void setId(const int &id_);
    QString name() const;
    void setName(const QString &name_);
    QString programmerName() const;
    void setProgrammerName(const QString &programmerName_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);

signals:
    void idChanged();
    void nameChanged();
    void programmerNameChanged();
    void descriptionChanged();
    void createdChanged();
    void mergeidChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    ObjectInfoModel *m_effectObjectInfoModel = nullptr;
    int m_appId = 0;

    friend class ListModelBaseOfData<ObjectInfoData, ObjectInfoModel, ObjectInfo>;
};


class ObjectInfoModel : public ListModelBaseOfData<ObjectInfoData, ObjectInfoModel, ObjectInfo>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ObjectInfoData, ObjectInfoModel, ObjectInfo>;

public:
    explicit ObjectInfoModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<ObjectInfoData *>> data_ = std::shared_ptr<QVector<ObjectInfoData *>>{nullptr},
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


#endif // OBJECTINFO_H
