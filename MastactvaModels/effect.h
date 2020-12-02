#ifndef EFFECT_H
#define EFFECT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "effectshader.h"
#include "effectarg.h"
#include "effectargset.h"


class EffectModel;


class Effect : public QObject
{
    Q_OBJECT

public:
    explicit Effect(EffectModel *parent_ = nullptr);

    Q_PROPERTY(int effectId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString effectName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariant effectShaders READ effectShaders WRITE setEffectShaders NOTIFY effectShadersChanged)
    Q_PROPERTY(QVariant effectArgs READ args WRITE setArgs NOTIFY argsChanged)
    Q_PROPERTY(QVariant effectArgSets READ argSets WRITE setArgSets NOTIFY argSetsChanged)

    class DefaultLayout : public LayoutBase<Effect>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Effect::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Effect::m_appId);
            addField<int>("id", "effectId", &Effect::id, &Effect::setId);
            addField<QString>("name", "effectName", &Effect::name, &Effect::setName);
            addField<QString>("description", "effectDescription", &Effect::description, &Effect::setDescription);
            addModel<EffectShaderModel>("effectShaders", &Effect::m_effectShadersModel, &Effect::createEffectShadersModel);
            addModel<EffectArgModel>("effectArgs", &Effect::m_effectArgModel, &Effect::createEffectArgModel);
            addModel<EffectArgSetModel>("effectArgSets", &Effect::m_effectArgSetModel, &Effect::createEffectArgSetModel);
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString name() const;
    void setName(const QString &name_);
    QString description() const;
    void setDescription(const QString &description_);
    QVariant effectShaders() const;
    void setEffectShaders(const QVariant &obj_);
    QVariant args() const;
    void setArgs(const QVariant &obj_);
    QVariant argSets() const;
    void setArgSets(const QVariant &obj_);

public:
    Q_INVOKABLE bool startRefreshArguments();
    Q_INVOKABLE void cancelRefreshArguments();
    Q_INVOKABLE void applyRefreshArguments();

protected slots:
    void refreshArgumentsShaderDownloadedSlot(const QString &url_);
    void refreshArgumentsProgressSlot();

protected:
    EffectShaderModel *createEffectShadersModel();
    EffectArgModel *createEffectArgModel();
    EffectArgSetModel *createEffectArgSetModel();

signals:
    void idChanged();
    void nameChanged();
    void descriptionChanged();
    void effectShadersChanged();
    void argsChanged();
    void argSetsChanged();
    void refreshArgumentsProgress(qreal rate_);
    void refreshArgumentsBeforeApply();

private:
    EffectModel *m_effectModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_name;
    QString m_description;
    EffectShaderModel *m_effectShadersModel = nullptr;
    EffectArgModel *m_effectArgModel = nullptr;
    EffectArgSetModel *m_effectArgSetModel = nullptr;

    QStringList m_shaderUrls;
    QHash<QString, QString> m_shaderLocalUrls;
};


class EffectModel : public ListModelBaseOfData<Effect, EffectModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Effect, EffectModel>;

public:
    explicit EffectModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, const QString& errorCodeStr_, RequestData *request_, const QJsonDocument &reply_)
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
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // EFFECT_H