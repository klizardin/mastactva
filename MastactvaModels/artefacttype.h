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

#ifndef ARTEFACTTYPE_H
#define ARTEFACTTYPE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefacttype_data.h"


class ArtefactType : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit ArtefactType(QObject *parent_ = nullptr);

    Q_PROPERTY(int artefactTypeId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString artefactTypeType READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool artefactTypeUniqueForObject READ uniqueForObject WRITE setUniqueForObject NOTIFY uniqueForObjectChanged)
    Q_PROPERTY(MergeId artefactTypeMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

    class DefaultLayout : public LayoutBase<ArtefactType>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact-type");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ArtefactType::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ArtefactType::m_appId);
            addField<int>("id", "artefactTypeId", &ArtefactType::id, &ArtefactType::setId);
            addField<QString>("type", "artefactTypeType", &ArtefactType::type, &ArtefactType::setType);
            addField<bool>("unique_for_object", "artefactTypeUniqueForObject", &ArtefactType::uniqueForObject, &ArtefactType::setUniqueForObject);
            addField<MergeId>("mergeid", "artefactTypeMergeId", &ArtefactType::mergeid, &ArtefactType::setMergeid);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString type() const;
    void setType(const QString &type_);
    bool uniqueForObject() const;
    void setUniqueForObject(const bool &uniqueForObject_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);

signals:
    void idChanged();
    void typeChanged();
    void uniqueForObjectChanged();
    void mergeidChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_type;
    MergeId m_mergeid;
    bool m_uniqueForObject = false;
};


class ArtefactTypeModel : public ListModelBaseOfData<ArtefactType, ArtefactTypeModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactType, ArtefactTypeModel>;

public:
    explicit ArtefactTypeModel(
            QObject *parent_ = nullptr,
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


#endif // ARTEFACTTYPE_H
