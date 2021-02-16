#ifndef ARTEFACTARG_H
#define ARTEFACTARG_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ArtefactArgModel;


class ArtefactArg : public QObject
{
    Q_OBJECT
public:
    explicit ArtefactArg(ArtefactArgModel *parent_ = nullptr);

    Q_PROPERTY(int artefactArgId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int artefactArgEffectArtefactArgSetId READ effectArtefactArgSetId WRITE setEffectArtefactArgSetId NOTIFY effectArtefactArgSetIdChanged)
    Q_PROPERTY(int artefactArgArgTypeId READ argTypeId WRITE setArgTypeId NOTIFY argTypeChanged)
    Q_PROPERTY(int artefactArgArgStorageId READ argStorageId WRITE setArgStorageId NOTIFY argStorageChanged)
    Q_PROPERTY(QString artefactArgName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString artefactArgDefaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString artefactArgDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime artefactArgCreated READ created WRITE setCreated NOTIFY createdChanged)


    class DefaultLayout : public LayoutBase<ArtefactArg>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact-arg");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ArtefactArg::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &ArtefactArg::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ArtefactArg::m_appId);
            addField<int>("id", "artefactArgId", &ArtefactArg::id, &ArtefactArg::setId);
            addField<int>("effect_artefact_arg_set", "artefactArgEffectArtefactArgSetId", &ArtefactArg::effectArtefactArgSetId, &ArtefactArg::setEffectArtefactArgSetId);
            addField<int>("arg_type", "artefactArgArgTypeId", &ArtefactArg::argTypeId, &ArtefactArg::setArgTypeId);
            addField<int>("arg_storage", "artefactArgArgStorageId", &ArtefactArg::argStorageId, &ArtefactArg::setArgStorageId);
            addField<QString>("name", "artefactArgName", &ArtefactArg::name, &ArtefactArg::setName);
            addField<QString>("default_value", "artefactArgDefaultValue", &ArtefactArg::defaultValue, &ArtefactArg::setDefaultValue);
            addField<QString>("description", "artefactArgDescription", &ArtefactArg::description, &ArtefactArg::setDescription);
            addField<QDateTime>("created", "artefactArgCreated", &ArtefactArg::created, &ArtefactArg::setCreated);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectArtefactArgSetId() const;
    void setEffectArtefactArgSetId(const int &effectArtefactArgSetId_);
    int argTypeId() const;
    void setArgTypeId(const int &argTypeId_);
    int argStorageId() const;
    void setArgStorageId(const int &argStorageId_);
    QString name() const;
    void setName(const QString &name_);
    QString defaultValue() const;
    void setDefaultValue(const QString &defaultValue_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);

signals:
    void idChanged();
    void effectArtefactArgSetIdChanged();
    void argTypeChanged();
    void argStorageChanged();
    void nameChanged();
    void defaultValueChanged();
    void descriptionChanged();
    void createdChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectArtefactArgSetId = -1;
    int m_argTypeId = -1;
    int m_argStorageId = -1;
    QString m_name;
    QString m_defaultValue;
    QString m_description;
    QDateTime m_created;
    ArtefactArgModel *m_artefactArgModel = nullptr;
};


class ArtefactArgModel : public ListModelBaseOfData<ArtefactArg, ArtefactArgModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactArg, ArtefactArgModel>;

public:
    explicit ArtefactArgModel(QObject *parent_ = nullptr);

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


#endif // ARTEFACTARG_H
