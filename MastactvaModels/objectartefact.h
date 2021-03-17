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
    virtual ~ObjectArtefact() override;

    Q_PROPERTY(int objectArtefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int objectArtefactObjectInfoId READ objectInfoId WRITE setObjectInfoId NOTIFY objectInfoIdChanged)
    Q_PROPERTY(QVariant objectArtefactArtefact READ artefact WRITE setArtefact NOTIFY artefactChanged)
    Q_PROPERTY(int objectArtefactStepIndex READ stepIndex WRITE setStepIndex NOTIFY stepIndexChanged)

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
            addField<int>("artefact", "", &ObjectArtefact::artefactId, &ObjectArtefact::setArtefactId);
            addField<int>("step_index", "objectArtefactStepIndex", &ObjectArtefact::stepIndex, &ObjectArtefact::setStepIndex);
            addModel<ArtefactModel>("objectArtefactArtefact", &ObjectArtefact::m_artefactModel, &ObjectArtefact::createArtefactModel);
            /* 1:1 */
            setIdField("id");
        }
    };

    Q_INVOKABLE int artefactId() const;
    Q_INVOKABLE void setArtefactId(const int &artefactId_);

public:
    int id() const;
    void setId(const int &id_);
    int objectInfoId() const;
    void setObjectInfoId(const int &effectId_);
    QVariant artefact() const;
    void setArtefact(const QVariant &obj_);
    ArtefactModel *getArtefact();
    const ArtefactModel *getArtefact() const;
    int stepIndex() const;
    void setStepIndex(const int &stepIndex_);

protected:
    ArtefactModel *createArtefactModel();

signals:
    void idChanged();
    void objectInfoIdChanged();
    void artefactChanged();
    void stepIndexChanged();

private:
    ObjectArtefactModel *m_effectArtefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_artefactId = -1;
    int m_stepIndex = -1;
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