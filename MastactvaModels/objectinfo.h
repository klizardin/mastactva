#ifndef OBJECTINFO_H
#define OBJECTINFO_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ObjectInfoModel;


class ObjectInfo : public QObject
{
    Q_OBJECT
public:
    explicit ObjectInfo(ObjectInfoModel *parent_ = nullptr);

    Q_PROPERTY(int effectObjectInfoId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString effectObjectInfoName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectObjectInfoProgrammerName READ programmerName WRITE setProgrammerName NOTIFY programmerNameChanged)
    Q_PROPERTY(QString effectObjectInfoDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectObjectInfoCreated READ created WRITE setCreated NOTIFY createdChanged)


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

signals:
    void idChanged();
    void nameChanged();
    void programmerNameChanged();
    void descriptionChanged();
    void createdChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    ObjectInfoModel *m_effectObjectInfoModel = nullptr;
    int m_appId = 0;
    int m_id = -1;
    QString m_name;
    QString m_programmerName;
    QString m_description;
    QDateTime m_created;
};


class ObjectInfoModel : public ListModelBaseOfData<ObjectInfo, ObjectInfoModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ObjectInfo, ObjectInfoModel>;

public:
    explicit ObjectInfoModel(QObject *parent_ = nullptr);

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
