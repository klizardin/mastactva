#ifndef EFFECTOBJECTINFO_H
#define EFFECTOBJECTINFO_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class EffectObjectInfoModel;


class EffectObjectInfo : public QObject
{
    Q_OBJECT
public:
    explicit EffectObjectInfo(EffectObjectInfoModel *parent_ = nullptr);

    Q_PROPERTY(int effectObjectInfoId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString effectObjectInfoName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectObjectInfoProgrammerName READ programmerName WRITE setProgrammerName NOTIFY programmerNameChanged)
    Q_PROPERTY(QString effectObjectInfoDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectObjectInfoCreated READ created WRITE setCreated NOTIFY createdChanged)


    class DefaultLayout : public LayoutBase<EffectObjectInfo>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-object-info");
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectObjectInfo::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectObjectInfo::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &EffectObjectInfo::m_objectModelInfo);
            addField<int>("id", "effectObjectInfoId", &EffectObjectInfo::id, &EffectObjectInfo::setId);
            addField<QString>("name", "effectObjectInfoName", &EffectObjectInfo::name, &EffectObjectInfo::setName);
            addField<QString>("programmer_name", "effectObjectInfoProgrammerName", &EffectObjectInfo::programmerName, &EffectObjectInfo::setProgrammerName);
            addField<QString>("description", "effectObjectInfoDescription", &EffectObjectInfo::description, &EffectObjectInfo::setDescription);
            addField<QDateTime>("created", "effectObjectInfoCreated", &EffectObjectInfo::created, &EffectObjectInfo::setCreated);
            setIdField("id");
        }
    };


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

signals:
    void idChanged();
    void nameChanged();
    void programmerNameChanged();
    void descriptionChanged();
    void createdChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    EffectObjectInfoModel *m_effectObjectInfoModel = nullptr;
    int m_appId = 0;
    int m_id = -1;
    QString m_name;
    QString m_programmerName;
    QString m_description;
    QDateTime m_created;
};


class EffectObjectInfoModel : public ListModelBaseOfData<EffectObjectInfo, EffectObjectInfoModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectObjectInfo, EffectObjectInfoModel>;

public:
    explicit EffectObjectInfoModel(QObject *parent_ = nullptr);

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


#endif // EFFECTOBJECTINFO_H
