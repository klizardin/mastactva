#ifndef EFFECTSHADER_H
#define EFFECTSHADER_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "shader.h"


class EffectShaderModel;


class EffectShader : public QObject
{
    Q_OBJECT
public:
    explicit EffectShader(EffectShaderModel *parent_ = nullptr);

    Q_PROPERTY(int effectShaderId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectShaderEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QVariant effectShaderShader READ shader WRITE setShader NOTIFY shaderChanged)

    class DefaultLayout : public LayoutBase<EffectShader>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-shader");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectShader::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectShader::m_appId);
            addField<int>("id", "effectShaderId", &EffectShader::id, &EffectShader::setId);
            addField<int>("effect", "effectShaderEffectId", &EffectShader::effectId, &EffectShader::setEffectId);
            addField<int>("shader", "", &EffectShader::shaderId, &EffectShader::setShaderId);
            addModel<ShaderModel>("effectShaderShader", &EffectShader::m_shaderModel, &EffectShader::createShaderModel);
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    int shaderId() const;
    void setShaderId(const int &shaderId_);
    QVariant shader() const;
    void setShader(const QVariant &obj_);

protected:
    ShaderModel *createShaderModel();

signals:
    void idChanged();
    void effectIdChanged();
    void shaderChanged();

private:
    EffectShaderModel *m_effectShaderModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_effectId = -1;
    int m_shaderId = -1;
    ShaderModel *m_shaderModel = nullptr;
};


class EffectShaderModel : public ListModelBaseOfData<EffectShader, EffectShaderModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectShader, EffectShaderModel>;

public:
    explicit EffectShaderModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
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
    void error(int errorCode_, const QString &description_);
};


#endif // EFFECTSHADER_H
