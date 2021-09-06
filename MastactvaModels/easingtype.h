/*
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

#ifndef EASINGTYPE_H
#define EASINGTYPE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class EasingTypeModel;


class EasingType : public QObject, public IListModelItem
{
    Q_OBJECT

public:
    explicit EasingType(EasingTypeModel *parent_ = nullptr);

    Q_PROPERTY(int easingTypeId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString easingTypeType READ type WRITE setType NOTIFY typeChanged)

    class DefaultLayout : public LayoutBase<EasingType>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("easing-type");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &EasingType::m_parentMOdelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &EasingType::m_appId);
            addField<int>("id", "easingTypeId", &EasingType::id, &EasingType::setId);
            addField<QString>("type", "easingTypeType", &EasingType::type, &EasingType::setType);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString type() const;
    void setType(const QString &type_);

signals:
    void idChanged();
    void typeChanged();

private:
    EasingTypeModel *m_easingTypeModel = nullptr;
    IListModelInfo *m_parentMOdelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_type;
};


class EasingTypeModel : public ListModelBaseOfData<EasingType, EasingTypeModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<EasingType, EasingTypeModel>;

public:
    explicit EasingTypeModel(QObject *parent_ = nullptr);

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


#endif // EASINGTYPE_H
