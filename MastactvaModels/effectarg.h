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

#ifndef EFFECTARG_H
#define EFFECTARG_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/artefactarg.h"
#include "../MastactvaModels/effectarg_data.h"


class EffectArgModel;


class EffectArg : public QObject, public IListModelItem, private EffectArgData
{
    Q_OBJECT
public:
    explicit EffectArg(EffectArgModel *parent_ = nullptr);
    EffectArg(EffectArgData &&data_, EffectArgModel *parent_ = nullptr);
    virtual ~EffectArg() override;

    Q_PROPERTY(int effectArgId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int effectArgEffectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(int effectArgObjectArtefactId READ objectArtefactId WRITE setObjectArtefactId NOTIFY objectArtefactIdChanged)
    Q_PROPERTY(int effectArgArgTypeId READ argTypeId WRITE setArgTypeId NOTIFY argTypeChanged)
    Q_PROPERTY(int effectArgArgStorageId READ argStorageId WRITE setArgStorageId NOTIFY argStorageChanged)
    Q_PROPERTY(QString effectArgName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString effectArgDefaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString effectArgDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime effectArgCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant effectArgObjectArtefact READ objectArtefact WRITE setObjectArtefact NOTIFY objectArtefactChanged)


    class DefaultLayout : public LayoutBase<EffectArg>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("effect-arg");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EffectArg::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EffectArg::m_appId);
            addField<int>("id", "effectArgId", &EffectArg::id, &EffectArg::setId);
            addField<int>("effect", "effectArgEffectId", &EffectArg::effectId, &EffectArg::setEffectId);
            addField<int>("object_artefact", "effectArgObjectArtefactId", &EffectArg::objectArtefactId, &EffectArg::setObjectArtefactId);
            addField<int>("arg_type", "effectArgArgTypeId", &EffectArg::argTypeId, &EffectArg::setArgTypeId);
            addField<int>("arg_storage", "effectArgArgStorageId", &EffectArg::argStorageId, &EffectArg::setArgStorageId);
            addField<QString>("name", "effectArgName", &EffectArg::name, &EffectArg::setName);
            addField<QString>("default_value", "effectArgDefaultValue", &EffectArg::defaultValue, &EffectArg::setDefaultValue);
            addField<QString>("description", "effectArgDescription", &EffectArg::description, &EffectArg::setDescription);
            addField<QDateTime>("created", "effectArgCreated", &EffectArg::created, &EffectArg::setCreated);
            addModel<ObjectArtefactModel>("effectArgObjectArtefact", &EffectArg::m_objectArtefactModel, &EffectArg::createObjectArtefactModel);
            /* 1:1 */
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    int effectId() const;
    void setEffectId(const int &effectId_);
    int objectArtefactId() const;
    void setObjectArtefactId(const int &objectArtefactId_);
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
    QVariant objectArtefact() const;
    void setObjectArtefact(const QVariant &objectArtefact_);

    void copyFrom(const ArtefactArg *artefactArg_, int effectId, int objectArtefactId_);

protected:
    ObjectArtefactModel *createObjectArtefactModel();

signals:
    void idChanged();
    void effectIdChanged();
    void argTypeChanged();
    void argStorageChanged();
    void nameChanged();
    void defaultValueChanged();
    void descriptionChanged();
    void objectArtefactIdChanged();
    void createdChanged();
    void objectArtefactChanged();

private:
    EffectArgModel *m_effectArgModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    ObjectArtefactModel *m_objectArtefactModel = nullptr;

    friend class ListModelBaseOfData<EffectArgData, EffectArgModel, EffectArg>;
};


class EffectArgModel : public ListModelBaseOfData<EffectArgData, EffectArgModel, EffectArg>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EffectArgData, EffectArgModel, EffectArg>;

public:
    explicit EffectArgModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<EffectArgData *>> data_ = std::shared_ptr<QVector<EffectArgData *>>{nullptr}
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


#endif // EFFECTARG_H
