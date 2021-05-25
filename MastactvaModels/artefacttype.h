#ifndef ARTEFACTTYPE_H
#define ARTEFACTTYPE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


enum class ArtefactTypeEn : int
{
    Vertex = 1,
    Fragment = 2,
    Texture1D = 3,
    Texture2D = 4,
    Texture3D = 5,
    DataJson = 6,
    DataObj = 7,
    ConvertJson = 8,
    ScriptLua = 9
};


class ArtefactType : public QObject
{
    Q_OBJECT
public:
    explicit ArtefactType(QObject *parent_ = nullptr);

    Q_PROPERTY(int artefactTypeId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString artefactTypeType READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool artefactTypeUniqueForObject READ uniqueForObject WRITE setUniqueForObject NOTIFY uniqueForObjectChanged)

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

signals:
    void idChanged();
    void typeChanged();
    void uniqueForObjectChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_type;
    bool m_uniqueForObject = false;
};


class ArtefactTypeModel : public ListModelBaseOfData<ArtefactType, ArtefactTypeModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactType, ArtefactTypeModel>;

public:
    explicit ArtefactTypeModel(QObject *parent_ = nullptr);

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
