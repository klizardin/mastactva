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

#ifndef OBJECTARTEFACT_H
#define OBJECTARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaModels/artefact.h"


class ObjectArtefactModel;


class ObjectArtefact : public QObject, public IListModelItem, private ObjectArtefactData
{
    Q_OBJECT
public:
    explicit ObjectArtefact(ObjectArtefactModel *parent_ = nullptr);
    explicit ObjectArtefact(ObjectArtefactData &&data_, ObjectArtefactModel *parent_ = nullptr);
    virtual ~ObjectArtefact() override;

    Q_PROPERTY(int objectArtefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int objectArtefactObjectInfoId READ objectInfoId WRITE setObjectInfoId NOTIFY objectInfoIdChanged)
    Q_PROPERTY(int objectArtefactArtefactId READ artefactId WRITE setArtefactId NOTIFY artefactChanged)
    Q_PROPERTY(QVariant objectArtefactArtefact READ artefact WRITE setArtefact NOTIFY artefactChanged)
    Q_PROPERTY(int objectArtefactStepIndex READ stepIndex WRITE setStepIndex NOTIFY stepIndexChanged)
    Q_PROPERTY(MergeId objectArtefactMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

    class DefaultLayout : public LayoutBase<ObjectArtefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("object-artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ObjectArtefact::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ObjectArtefact::m_appId);
            addField<int>("id", "objectArtefactId", &ObjectArtefact::id, &ObjectArtefact::setId);
            addField<int>("object_info", "objectArtefactObjectInfoId", &ObjectArtefact::objectInfoId, &ObjectArtefact::setObjectInfoId);
            addField<int>("artefact", "objectArtefactArtefactId", &ObjectArtefact::artefactId, &ObjectArtefact::setArtefactId);
            addField<int>("step_index", "objectArtefactStepIndex", &ObjectArtefact::stepIndex, &ObjectArtefact::setStepIndex);
            addField<MergeId>("mergeid", "objectArtefactMergeId", &ObjectArtefact::mergeid, &ObjectArtefact::setMergeid);
            addModel<ArtefactModel>("objectArtefactArtefact", &ObjectArtefact::m_artefactModel, &ObjectArtefact::createArtefactModel);
            /* 1:1 */
            setIdField("id");
        }
    };

    Q_INVOKABLE void setArtefactId(const int &artefactId_);
    Q_INVOKABLE bool updateArtefactId(const QVariant &artefact_);
    Q_INVOKABLE void updateObjectInfoId(int objectInfoId_);

public:
    int id() const;
    void setId(const int &id_);
    int artefactId() const;
    int objectInfoId() const;
    void setObjectInfoId(const int &objectInfoId_);
    QVariant artefact() const;
    void setArtefact(const QVariant &obj_);
    ArtefactModel *getArtefact();
    const ArtefactModel *getArtefact() const;
    int stepIndex() const;
    void setStepIndex(const int &stepIndex_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);

protected:
    ArtefactModel *createArtefactModel();

signals:
    void idChanged();
    void objectInfoIdChanged();
    void artefactChanged();
    void stepIndexChanged();
    void mergeidChanged();

private:
    ObjectArtefactModel *m_effectArtefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    ArtefactModel *m_artefactModel = nullptr;

    friend class ListModelBaseOfData<ObjectArtefactData, ObjectArtefactModel, ObjectArtefact>;
};


class ObjectArtefactModel : public ListModelBaseOfData<ObjectArtefactData, ObjectArtefactModel, ObjectArtefact>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ObjectArtefactData, ObjectArtefactModel, ObjectArtefact>;

public:
    explicit ObjectArtefactModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<ObjectArtefactData *>> data_ = std::shared_ptr<QVector<ObjectArtefactData *>>{nullptr},
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


#endif // OBJECTARTEFACT_H
