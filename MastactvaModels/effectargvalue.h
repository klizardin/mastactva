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

#ifndef EFFECTARGVALUE_H
#define EFFECTARGVALUE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/effectarg.h"
#include "../MastactvaModels/effectarg_data.h"
#include "../MastactvaModels/effectargvalue_data.h"


class EffectArgValueModel;


class EffectArgValue : public QObject, public IListModelItem, private EffectArgValueData
{
    Q_OBJECT
public:
    explicit EffectArgValue(EffectArgValueModel *parent_ = nullptr);
    EffectArgValue(EffectArgValueData &&data_, EffectArgValueModel *parent_ = nullptr);
    virtual ~EffectArgValue() override;

    Q_PROPERTY(int effectArgValueId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgValueArgSetId READ argSetId WRITE setArgSetId NOTIFY argSetIdChanged)
    Q_PROPERTY(QString effectArgValueValue READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString effectArgValueDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectArgValueCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant effectArgValueArg READ arg WRITE setArg NOTIFY argChanged)
    Q_PROPERTY(QString effectArgValueMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

    class DefaultLayout : public LayoutBase<EffectArgValue>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg-value");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArgValue::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArgValue::m_appId);
            addField<int>("id", "effectArgValueId", &EffectArgValue::id, &EffectArgValue::setId);
            addField<int>("arg_set", "effectArgValueArgSetId", &EffectArgValue::argSetId, &EffectArgValue::setArgSetId);
            addField<int>("arg", "", &EffectArgValue::argId, &EffectArgValue::setArgId);
            addField<QString>("value", "effectArgValueValue", &EffectArgValue::value, &EffectArgValue::setValue);
            addField<QString>("description", "effectArgValueDescription", &EffectArgValue::description, &EffectArgValue::setDescription);
            addField<QDateTime>("created", "effectArgValueCreated", &EffectArgValue::created, &EffectArgValue::setCreated);
            addField<QString>("mergeid", "effectArgValueMergeId", &EffectArgValue::mergeid, &EffectArgValue::setMergeid);
            addModel<EffectArgModel>("effectArgValueArg", &EffectArgValue::m_effectArgModel, &EffectArgValue::createEffectArgModel);
            /* 1:1 */
            setIdField("id");
        }
    };


public:
    Q_INVOKABLE int argSetId() const;
    Q_INVOKABLE void setArgSetId(const int &argSetId_);
    Q_INVOKABLE int argId() const;
    Q_INVOKABLE void setArgId(const int &argId_);

public:
    int id() const;
    void setId(const int &id_);
    QVariant arg() const;
    void setArg(const QVariant &obj_);
    QString value() const;
    void setValue(const QString &value_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QString mergeid() const;
    void setMergeid(const QString &mergeid_);

    EffectArgModel *getArg();
    int getArgId() const;

protected:
    EffectArgModel *createEffectArgModel();

signals:
    void idChanged();
    void argSetIdChanged();
    void argChanged();
    void valueChanged();
    void descriptionChanged();
    void createdChanged();
    void mergeidChanged();

private:
    EffectArgValueModel *m_effectArgValueModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    EffectArgModel *m_effectArgModel = nullptr;

    friend class ListModelBaseOfData<EffectArgValueData, EffectArgValueModel, EffectArgValue>;
};


class EffectArgValueModel : public ListModelBaseOfData<EffectArgValueData, EffectArgValueModel, EffectArgValue>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArgValueData, EffectArgValueModel, EffectArgValue>;

public:
    explicit EffectArgValueModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<EffectArgValueData *>> data_ = std::shared_ptr<QVector<EffectArgValueData *>>{nullptr}
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


#endif // EFFECTARGVALUE_H
