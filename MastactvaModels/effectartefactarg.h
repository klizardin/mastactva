#ifndef EFFECTARTEFACTARG_H
#define EFFECTARTEFACTARG_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class EffectArtefactArgModel;


class EffectArtefactArg : public QObject
{
    Q_OBJECT
public:
    explicit EffectArtefactArg(EffectArtefactArgModel *parent_ = nullptr);

    Q_PROPERTY(int effectArtefactArgId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArtefactArgEffectArtefactId READ effectArtefactId WRITE setEffectArtefactId NOTIFY effectArtefactIdChanged)
    Q_PROPERTY(QString effectArtefactArgDescription READ description WRITE setDescription NOTIFY descriptionChanged)


    class DefaultLayout : public LayoutBase<EffectArtefactArg>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-artefact-arg");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArtefactArg::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &EffectArtefactArg::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArtefactArg::m_appId);
            addField<int>("id", "effectArtefactArgId", &EffectArtefactArg::id, &EffectArtefactArg::setId);
            addField<int>("effect_artefact", "effectArtefactArgEffectArtefactId", &EffectArtefactArg::effectArtefactId, &EffectArtefactArg::setEffectArtefactId);
            addField<QString>("description", "effectArtefactArgDescription", &EffectArtefactArg::description, &EffectArtefactArg::setDescription);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectArtefactId() const;
    void setEffectArtefactId(const int &effectArtefactId_);
    QString description() const;
    void setDescription(const QString &description_);

signals:
    void idChanged();
    void effectArtefactIdChanged();
    void descriptionChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectArtefactId = -1;
    QString m_description;
    EffectArtefactArgModel *m_effectArtefactArgModel = nullptr;
};


class EffectArtefactArgModel : public ListModelBaseOfData<EffectArtefactArg, EffectArtefactArgModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArtefactArg, EffectArtefactArgModel>;

public:
    explicit EffectArtefactArgModel(QObject *parent_ = nullptr);

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


#endif // EFFECTARTEFACTARG_H
