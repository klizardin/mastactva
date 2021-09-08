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

#ifndef QUIZUSER_H
#define QUIZUSER_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


QString getAndroidUuid();


class QuizUser : public QObject, public IListModelItem
{
    Q_OBJECT

public:
    class DefaultLayout : public LayoutBase<QuizUser>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("");
            addSpecial<int>(layout::SpecialFieldEn::appId, &QuizUser::m_appId);
            addField<QString>("deviceid", "deviceId", &QuizUser::deviceId, &QuizUser::setDeviceId);
            setIdField("deviceid");
        }
    };

public:
    explicit QuizUser(QObject *parent_ = nullptr);

    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)

    QString deviceId() const;
    void setDeviceId(const QString &deviceId_);


signals:
    void deviceIdChanged();


private:
    QString m_deviceId;
    int m_appId = 0;
};


class QuizUserModel : public ListModelBaseOfData<QuizUser, QuizUserModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<QuizUser, QuizUserModel>;

public:
    explicit QuizUserModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
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

public:
    void setEmptyModel();

protected:
    virtual void modelListLoaded(const QJsonDocument &reply_) override;

private:
    bool m_emptyModel = false;
};


#endif // QUIZUSER_H
