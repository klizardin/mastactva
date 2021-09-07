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

#ifndef IMAGEPOINTEFFECT_H
#define IMAGEPOINTEFFECT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/effectargset.h"


class ImagePointEffectModel;


class ImagePointEffect : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit ImagePointEffect(ImagePointEffectModel *parent_ = nullptr);
    virtual ~ImagePointEffect() override;

    Q_PROPERTY(int imagePointEffectId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int imagePointEffectImagePointId READ imagePointId WRITE setImagePointId NOTIFY imagePointIdChanged)
    Q_PROPERTY(QVariant imagePointEffectEffect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QVariant imagePointEffectArgSet READ argSet WRITE setArgSet NOTIFY argSetChanged)
    Q_PROPERTY(int imagePointEffectDuration READ duration WRITE setDuration NOTIFY durationChanged)

    class DefaultLayout : public LayoutBase<ImagePointEffect>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point-effect");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImagePointEffect::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePointEffect::m_appId);
            addField<int>("id", "imagePointEffectId", &ImagePointEffect::id, &ImagePointEffect::setId);
            addField<int>("image_point", "imagePointEffectImagePointId", &ImagePointEffect::imagePointId, &ImagePointEffect::setImagePointId);
            addField<int>("effect", "", &ImagePointEffect::effectId, &ImagePointEffect::setEffectId);
            addField<int>("arg_set", "", &ImagePointEffect::argSetId, &ImagePointEffect::setArgSetId);
            addField<int>("duration", "imagePointEffectDuration", &ImagePointEffect::duration, &ImagePointEffect::setDuration);
            addModel<EffectModel>("imagePointEffectEffect", &ImagePointEffect::m_effectModel, &ImagePointEffect::createEffectModel);
            /* 1:1 */
            addModel<EffectArgSetModel>("imagePointEffectArgSet", &ImagePointEffect::m_effectArgSetModel, &ImagePointEffect::createEffectArgSetModel);
            /* 1:1 */
            setIdField("id");
        }
    };

    Q_INVOKABLE void setImagePointId(const int &imagePointId_);
    Q_INVOKABLE int effectId() const;
    Q_INVOKABLE void setEffectId(const int &effectId_);
    Q_INVOKABLE int argSetId() const;
    Q_INVOKABLE void setArgSetId(const int &argSetId_);
    Q_INVOKABLE void setDuration(const int &duration_);

    int id() const;
    void setId(const int &id_);
    int imagePointId() const;
    QVariant effect() const;
    void setEffect(const QVariant &obj_);
    QVariant argSet() const;
    void setArgSet(const QVariant &obj_);
    int duration() const;

protected:
    EffectModel *createEffectModel();
    EffectArgSetModel *createEffectArgSetModel();

signals:
    void idChanged();
    void imagePointIdChanged();
    void effectChanged();
    void argSetChanged();
    void stepIndexChanged();
    void durationChanged();

private:
    ImagePointEffectModel *m_imagePointEffectModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_imagePointId = -1;
    int m_effectId = -1;
    int m_effectArgSetId = -1;
    int m_duration = 1;
    EffectModel *m_effectModel = nullptr;
    EffectArgSetModel *m_effectArgSetModel = nullptr;
};


class ImagePointEffectModel : public ListModelBaseOfData<
        ImagePointEffect,
        ImagePointEffectModel
        >
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePointEffect, ImagePointEffectModel>;

public:
    explicit ImagePointEffectModel(QObject *parent_ = nullptr);

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


#endif // IMAGEPOINTEFFECT_H
