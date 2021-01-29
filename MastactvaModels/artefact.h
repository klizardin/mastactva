#ifndef ARTEFACT_H
#define ARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefacttype.h"


class ArtefactModel;


class Artefact : public QObject, protected IListModelInfoObjectImpl
{
    Q_OBJECT
public:
    explicit Artefact(ArtefactModel *parent_ = nullptr);

    Q_PROPERTY(int artefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString artefactFilename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString artefactHash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int artefactTypeId READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString artefactDescription READ description WRITE setDescription NOTIFY descriptionChanged)


    class DefaultLayout : public LayoutBase<Artefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Artefact::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Artefact::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Artefact::m_appId);
            addField<int>("id", "artefactId", &Artefact::id, &Artefact::setId);
            addField<ImageSource>("filename", "artefactFilename", &Artefact::getFilename, &Artefact::setFilename);
            addField<QString>("hash", "artefactHash", &Artefact::hash, &Artefact::setHash);
            addField<int>("type", "artefactTypeId", &Artefact::type, &Artefact::setType);
            addField<QString>("description", "artefactDescription", &Artefact::description, &Artefact::setDescription);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString filename() const;
    ImageSource getFilename() const;
    void setFilename(const QString &filename_);
    void setFilename(const ImageSource &filename_);
    QString hash() const;
    void setHash(const QString &hash_);
    int type() const;
    void setType(const int &type_);
    QString description() const;
    void setDescription(const QString &description_);

protected:
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    QString getObjectName() const;

signals:
    void idChanged();
    void filenameChanged();
    void hashChanged();
    void typeChanged();
    void descriptionChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    ImageSource m_filename;
    QString m_hash;
    int m_typeId = -1;
    QString m_description;
    ArtefactModel *m_artefactModel = nullptr;
};


class ArtefactModel : public ListModelBaseOfData<Artefact, ArtefactModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Artefact, ArtefactModel>;

public:
    explicit ArtefactModel(QObject *parent_ = nullptr);

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


#endif // ARTEFACT_H